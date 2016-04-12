//
// Copyright (c) 2008-2015 the Sviga project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "IDE.h"

#ifdef MessageBox
#undef MessageBox
#endif

#include "IDESettings.h"
#include "UI/ModalWindow.h"
#include "UI/ResourcePicker.h"
#include "Editor/EditorSelection.h"
#include "UI/MenuBarUI.h"
#include "UI/ToolBarUI.h"
#include "UI/MiniToolBarUI.h"
#include "Editor/EditorPlugin.h"
#include "Editor/Editor.h"
#include "Editor/EditorView.h"
#include "UI/UIGlobals.h"
#include "UI/TabWindow.h"
#include "UI/HierarchyWindow.h"
#include "ProjectManager.h"
#include "TemplateManager.h"
#include "ProjectWindow.h"
#include "UI/AttributeInspector.h"
#include "Editor/ResourceBrowser.h"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/UI/MessageBox.h>

using namespace Urho3D;

URHO3D_DEFINE_APPLICATION_MAIN(IDE)

IDE::IDE(Urho3D::Context* context) : Application(context)
{
	prjMng_ = NULL;
	cache_ = NULL;
	ui_ = NULL;
	console_ = NULL;
	debugHud_ = NULL;
	graphics_ = NULL;

	touchEnabled_ = false;
	screenJoystickIndex_ = M_MAX_UNSIGNED;
	screenJoystickSettingsIndex_ = M_MAX_UNSIGNED;
	paused_ = false;
}

void IDE::Setup()
{
	// Register Urho3D Plus
	ModalWindow::RegisterObject(context_);
	IDESettings::RegisterObject(context_);
	ProjectManager::RegisterObject(context_);
	ProjectSettings::RegisterObject(context_);
	Editor::RegisterObject(context_);
	ProjectWindow::RegisterObject(context_);
	ResourcePickerManager::RegisterObject(context_);
	MenuBarUI::RegisterObject(context_);
	ToolBarUI::RegisterObject(context_);
	MiniToolBarUI::RegisterObject(context_);
	TemplateManager::RegisterObject(context_);
	TabWindow::RegisterObject(context_);

	// create IDESettings subsystem
	context_->RegisterSubsystem(new IDESettings(context_));
	settings_ = GetSubsystem<IDESettings>();

	// load configuration file
	settings_->LoadConfigFile();

	ui_ = GetSubsystem<UI>();
	UIElement *root = ui_->GetRoot();
	root->SetSize(settings_->windowWidth_, settings_->windowHeight_);

	engineParameters_ = settings_->ToVariantMap();
	// Modify engine startup parameters
	engineParameters_["LogName"] = GetTypeName() + ".log";
	//GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")
	//	engineParameters_["AutoloadPaths"] = "Data;CoreData;IDEData";
	//engineParameters_["WindowResizable"] = true;
	
}

void IDE::Start()
{
	// cache subsystems
	cache_ = GetSubsystem<ResourceCache>();
	ui_ = GetSubsystem<UI>();
	graphics_ = GetSubsystem<Graphics>();

	context_->RegisterSubsystem(new ProjectManager(context_));
	prjMng_ = GetSubsystem<ProjectManager>();
	prjMng_->SetProjectRootFolder(settings_->projectsRootDir_);

	// Get default style
	XMLFile* xmlFile = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* styleFile = cache_->GetResource<XMLFile>("UI/IDEStyle.xml");
	XMLFile* iconstyle = cache_->GetResource<XMLFile>("UI/IDEIcons.xml");

	ui_->GetRoot()->SetDefaultStyle(styleFile);

	// Create console
	console_ = engine_->CreateConsole();
	console_->SetDefaultStyle(xmlFile);
	console_->SetAutoVisibleOnError(true);

	// Create debug HUD.
	debugHud_ = engine_->CreateDebugHud();
	debugHud_->SetDefaultStyle(xmlFile);

	// Subscribe key down event
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(IDE, HandleKeyDown));
	SubscribeToEvent(E_MENUBAR_ACTION, URHO3D_HANDLER(IDE, HandleMenuBarAction));
	SubscribeToEvent(E_OPENPROJECT, URHO3D_HANDLER(IDE, HandleOpenProject));

	// Create Cursor
	Cursor* cursor_ = new Cursor(context_);
	cursor_->SetStyleAuto(xmlFile);
	ui_->SetCursor(cursor_);
	if (GetPlatform() == "Android" || GetPlatform() == "iOS")
		ui_->GetCursor()->SetVisible(false);
	Input* input = GetSubsystem<Input>();
	// Use OS mouse without grabbing it
	input->SetMouseVisible(true);

	// create main ui
	rootUI_ = ui_->GetRoot()->CreateChild<UIElement>("IDERoot");
	rootUI_->SetSize(ui_->GetRoot()->GetSize());
	rootUI_->SetTraversalMode(TM_DEPTH_FIRST);     // This is needed for root-like element to prevent artifacts
	rootUI_->SetDefaultStyle(styleFile);

	editor_ = new Editor(context_);

	editor_->Create(NULL, NULL);

	if (!settings_->projectName_.Empty())
	{
		if ( prjMng_->SelectProject(settings_->projectsRootDir_, settings_->projectName_) )
			SendEvent(E_OPENPROJECT);
	}
	else
		ShowWelcomeScreen();	
}

void IDE::Stop()
{
	settings_->projectsRootDir_= prjMng_->GetRootDir();
	settings_->Save();
}

void IDE::HandleQuitMessageAck(StringHash eventType, VariantMap& eventData)
{
	using namespace MessageACK;

	bool ok_ = eventData[P_OK].GetBool();

	if (ok_)
		GetSubsystem<Engine>()->Exit();
}

void IDE::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;

	int key = eventData[P_KEY].GetInt();

	// Close console (if open) or exit when ESC is pressed
	if (key == KEY_ESC)
	{
		Console* console = GetSubsystem<Console>();
		if (console->IsVisible())
			console->SetVisible(false);
		else
		{
			Quit();
		}
	}

	// Toggle console with F1
	else if (key == KEY_F1)
		GetSubsystem<Console>()->Toggle();

	// Toggle debug HUD with F2
	else if (key == KEY_F2)
	{
		DebugHud* debugHud = GetSubsystem<DebugHud>();
		if (debugHud->GetMode() == 0 || debugHud->GetMode() == DEBUGHUD_SHOW_ALL_MEMORY)
			debugHud->SetMode(DEBUGHUD_SHOW_ALL);
		else
			debugHud->SetMode(DEBUGHUD_SHOW_NONE);
	}
	else if (key == KEY_F3)
	{
		DebugHud* debugHud = GetSubsystem<DebugHud>();
		if (debugHud->GetMode() == 0 || debugHud->GetMode() == DEBUGHUD_SHOW_ALL)
			debugHud->SetMode(DEBUGHUD_SHOW_ALL_MEMORY);
		else
			debugHud->SetMode(DEBUGHUD_SHOW_NONE);
	}

	// Common rendering quality controls, only when UI has no focused element
	else if (!GetSubsystem<UI>()->GetFocusElement())
	{
		Renderer* renderer = GetSubsystem<Renderer>();

		// Preferences / Pause
		if (key == KEY_SELECT && touchEnabled_)
		{
			paused_ = !paused_;

			Input* input = GetSubsystem<Input>();
			if (screenJoystickSettingsIndex_ == M_MAX_UNSIGNED)
			{
				// Lazy initialization
				ResourceCache* cache = GetSubsystem<ResourceCache>();
				screenJoystickSettingsIndex_ = input->AddScreenJoystick(cache->GetResource<XMLFile>("UI/ScreenJoystickSettings_Samples.xml"), cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
			}
			else
				input->SetScreenJoystickVisible(screenJoystickSettingsIndex_, paused_);
		}

		// Texture quality
		else if (key == '1')
		{
			int quality = renderer->GetTextureQuality();
			++quality;
			if (quality > QUALITY_HIGH)
				quality = QUALITY_LOW;
			renderer->SetTextureQuality(quality);
		}

		// Material quality
		else if (key == '2')
		{
			int quality = renderer->GetMaterialQuality();
			++quality;
			if (quality > QUALITY_HIGH)
				quality = QUALITY_LOW;
			renderer->SetMaterialQuality(quality);
		}

		// Specular lighting
		else if (key == '3')
			renderer->SetSpecularLighting(!renderer->GetSpecularLighting());

		// Shadow rendering
		else if (key == '4')
			renderer->SetDrawShadows(!renderer->GetDrawShadows());

		// Shadow map resolution
		else if (key == '5')
		{
			int shadowMapSize = renderer->GetShadowMapSize();
			shadowMapSize *= 2;
			if (shadowMapSize > 2048)
				shadowMapSize = 512;
			renderer->SetShadowMapSize(shadowMapSize);
		}

		// Shadow depth and filtering quality
		else if (key == '6')
		{
			ShadowQuality quality = renderer->GetShadowQuality();
			quality = (ShadowQuality)(quality + 1);
			if (quality > SHADOWQUALITY_BLUR_VSM)
				quality = SHADOWQUALITY_SIMPLE_16BIT;
			renderer->SetShadowQuality(quality);
		}

		// Occlusion culling
		else if (key == '7')
		{
			bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
			occlusion = !occlusion;
			renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
		}

		// Instancing
		else if (key == '8')
			renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());

		// Take screenshot
		else if (key == '9')
		{
			Graphics* graphics = GetSubsystem<Graphics>();
			Image screenshot(context_);
			graphics->TakeScreenShot(screenshot);
			// Here we save in the Data folder with date and time appended
			screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
				Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
		}
	}
}

void IDE::HandleMenuBarAction(StringHash eventType, VariantMap& eventData)
{
	using namespace  MenuBarAction;

	StringHash action = eventData[P_ACTION].GetStringHash();

	if (action == A_QUITEDITOR_VAR)
	{
		Quit();
	}
}

void IDE::ShowWelcomeScreen()
{
	welcomeUI_ = prjMng_->CreateWelcomeScreen();
	rootUI_->AddChild(welcomeUI_);
	//welcomeUI_->SetSize(rootUI_->GetSize());
	editor_->GetEditorView()->SetLeftFrameVisible(false);
	editor_->GetEditorView()->SetRightFrameVisible(false);
	unsigned index = editor_->GetEditorView()->GetMiddleFrame()->AddTab("Welcome", welcomeUI_);
	editor_->GetEditorView()->GetMiddleFrame()->SetActiveTab(index);
}

void IDE::HandleOpenProject(StringHash eventType, VariantMap& eventData)
{
	using namespace  OpenProject;

	prjMng_->ShowWelcomeScreen(false);
	
	editor_->GetResourceBrowser()->RebuildResourceDatabase();

	editor_->GetEditorView()->GetMiddleFrame()->RemoveTab("Welcome");
	editor_->GetEditorView()->SetLeftFrameVisible(true);
	editor_->GetEditorView()->SetRightFrameVisible(true);
	editor_->LoadPlugins();

	editor_->OpenProject(prjMng_->GetProject());

	/// \todo open project in the editor
}

void IDE::Quit()
{
	SharedPtr<Urho3D::MessageBox> messageBox(new Urho3D::MessageBox(context_, "Do you really want to exit the Editor ?", "Quit Editor ?"));

	if (messageBox->GetWindow() != NULL)
	{
		Button* cancelButton = (Button*)messageBox->GetWindow()->GetChild("CancelButton", true);
		cancelButton->SetVisible(true);
		cancelButton->SetFocus(true);
		SubscribeToEvent(messageBox, E_MESSAGEACK, URHO3D_HANDLER(IDE, HandleQuitMessageAck));
	}

	messageBox->AddRef();
}
