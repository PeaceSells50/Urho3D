#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include "EditorData.h"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/IOEvents.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/ScrollBar.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Texture.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/XMLFile.h>
#include "Editor.h"
#include "EditorPlugin.h"
#include <Urho3D/IO/FileSystem.h>

namespace Urho3D
{
	EditorData::EditorData(Context* context, Editor* editor) : Object(context),
		editor_(editor)
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();

		uiSceneFilters.Push("*.xml");
		uiSceneFilters.Push("*.bin");
		uiSceneFilters.Push("*.*");
		uiElementFilters.Push("*.xml");
		uiAllFilters.Push("*.*");
		uiScriptFilters.Push("*.*");
		uiScriptFilters.Push("*.as");
		uiScriptFilters.Push("*.lua");
		uiParticleFilters.Push("*.xml");
		uiRenderPathFilters.Push("*.xml");
		uiSceneFilter = 0;
		uiElementFilter = 0;
		uiNodeFilter = 0;
		uiImportFilter = 0;
		uiScriptFilter = 0;
		uiParticleFilter = 0;
		uiRenderPathFilter = 0;
		uiScenePath = fileSystem->GetProgramDir() + "Data/Scenes";
		uiElementPath = fileSystem->GetProgramDir() + "Data/UI";
		uiNodePath = fileSystem->GetProgramDir() + "Data/Objects";
		uiScriptPath = fileSystem->GetProgramDir() + "Data/Scripts";
		uiParticlePath = fileSystem->GetProgramDir() + "Data/Particles";
		uiRenderPathPath = fileSystem->GetProgramDir() + "CoreData/RenderPaths";
		screenshotDir = fileSystem->GetProgramDir() + "Screenshots";
	}

	EditorData::~EditorData()
	{
	}

	void EditorData::RegisterObject(Context* context)
	{
		context->RegisterFactory<EditorData>();
	}

	void EditorData::Load()
	{
		/// \todo load from settings file ...
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		UI* ui_ = GetSubsystem<UI>();
		defaultStyle_ = cache->GetResource<XMLFile>("UI/IDEStyle.xml");
		iconStyle_ = cache->GetResource<XMLFile>("UI/IDEIcons.xml");

		rootUI_ = ui_->GetRoot()->CreateChild<UIElement>("EditorRoot");
		rootUI_->SetSize(ui_->GetRoot()->GetSize());
		rootUI_->SetTraversalMode(TM_DEPTH_FIRST);     // This is needed for root-like element to prevent artifacts
		rootUI_->SetDefaultStyle(defaultStyle_);
		rootUI_->SetLayout(LM_VERTICAL);
	}

	void EditorData::SetGlobalVarNames(const String& name)
	{
		globalVarNames_[name] = name;
	}

	const Variant& EditorData::GetGlobalVarNames(StringHash& name)
	{
		return globalVarNames_[name];
	}

	Scene* EditorData::GetEditorScene()
	{
		return scene_;
	}

	void EditorData::SetEditorScene(Scene* scene)
	{
		scene_ = scene;
	}

	EditorPlugin* EditorData::GetEditor(Object *object)
	{
		for (unsigned i = 0; i < editorPlugins_.Size(); i++)
		{
			if (editorPlugins_[i]->HasMainScreen() && editorPlugins_[i]->Handles(object))
				return editorPlugins_[i];
		}

		return NULL;
	}

	EditorPlugin* EditorData::GetEditor(const String& name)
	{
		for (unsigned i = 0; i < editorPlugins_.Size(); i++)
		{
			if (editorPlugins_[i]->GetName() == name)
				return editorPlugins_[i];
		}

		return NULL;
	}

	Editor* EditorData::GetEditor()
	{
		return editor_;
	}

	EditorPlugin* EditorData::GetSubeditor(Object *object)
	{
		for (unsigned i = 0; i < editorPlugins_.Size(); i++)
		{
			if (!editorPlugins_[i]->HasMainScreen() && editorPlugins_[i]->Handles(object))
				return editorPlugins_[i];
		}

		return NULL;
	}

	void EditorData::AddEditorPlugin(EditorPlugin* plugin)
	{
		//p_plugin->undo_redo = &undo_redo;
		editorPlugins_.Push(SharedPtr<EditorPlugin>(plugin));
	}

	void EditorData::RemoveEditorPlugin(EditorPlugin* plugin)
	{
		//p_plugin->undo_redo = NULL;
		editorPlugins_.Remove(SharedPtr<EditorPlugin>(plugin));
	}
}