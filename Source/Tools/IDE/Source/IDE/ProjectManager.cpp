#include "Urho3d/Urho3D.h"

#include "IDESettings.h"
#include "ProjectManager.h"

#include <Urho3d/Core/Context.h>
#include <Urho3d/Core/ProcessUtils.h>

#include <Urho3d/Resource/XMLFile.h>
#include <Urho3d/Resource/ResourceCache.h>

#include <Urho3d/IO/FileSystem.h>
#include <Urho3d/IO/Log.h>
#include <Urho3d/IO/File.h>

#include <Urho3d/UI/UIElement.h>
#include <Urho3d/UI/Text.h>
#include <Urho3d/UI/Window.h>
#include <Urho3d/UI/UIEvents.h>
#include <Urho3d/UI/UI.h>
#include <Urho3d/UI/Button.h>
#include <Urho3d/UI/BorderImage.h>
#include <Urho3d/UI/Font.h>
#include <Urho3d/UI/ListView.h>

#include <Urho3d/Math/Color.h>

#include <Urho3d/Graphics/Texture2D.h>
#include <Urho3d/Graphics/Graphics.h>
#include <Urho3d/Graphics/Texture.h>

#include <Urho3d/Scene/Serializable.h>
#include <Urho3d/Container/Str.h>

#include "UI/DirSelector.h"
#include "UI/ModalWindow.h"
#include "UI/AttributeContainer.h"
#include "UI/UIGlobals.h"
#include "TemplateManager.h"
#include "Utils/Helpers.h"

#ifdef _WINDOWS
#include <stdio.h>

// used to modifiy text files cmakelists.txt and main.cpp
// when creating a new project
static char* readfile( const char* filename, long* size )
{
	FILE* fp;
	char* buffer;
	fopen_s(&fp, filename, "rb");
	if (!fp)
	{
		URHO3D_LOGINFOF("Reading error. %s", filename);
		return NULL;
	}

	// obtain file size:
	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);
	rewind(fp);

	// allocate memory to contain the whole file:
	buffer = (char*)malloc(sizeof(char)* (*size+1));
	if (!buffer)
	{
		URHO3D_LOGINFOF("Reading error. %s", filename);
		free(buffer);
		fclose(fp);
		return NULL;
	}

	// copy the file into the buffer:
	size_t result = fread(buffer, 1, *size, fp);
	if (result != *size)
	{
		URHO3D_LOGINFOF("Reading error. %s", filename);
		free(buffer);
		fclose(fp);
		return NULL;
	}

	buffer[*size] = '\0';
	fclose(fp);

	return buffer;	
}

static void writefile(const char* filename, const char* buffer, int size)
{
	FILE* fp;
	fopen_s(&fp, filename, "w");
	if (!fp)
	{
		URHO3D_LOGINFOF("Writing error. %s", filename);
		fclose(fp);
		return;
	}

	size_t result = fwrite(buffer, 1, size, fp);

	fclose(fp);
}

#endif

namespace Urho3D
{
	void ProjectManager::RegisterObject(Context* context)
	{
		context->RegisterFactory<ProjectManager>();
	}

	void ProjectManager::SetProjectRootFolder(const String& path)
	{
		settings_->projectsRootDir_ = path;
	}

	ProjectManager::~ProjectManager()
	{
		SAFE_DELETE(dirSelector_);
	}

	ProjectManager::ProjectManager(Context* context) :Object(context)
	{
		cache_ = GetSubsystem<ResourceCache>();
		graphics_ = GetSubsystem<Graphics>();
		ui_ = GetSubsystem<UI>();
		settings_ = GetSubsystem<IDESettings>();
		dirSelector_ = NULL;
		selectedtemplate_ = NULL;
	}

	void ProjectManager::NewProject()
	{
		if (newProject_ && selectedtemplate_)
		{
			FileSystem* fileSystem = GetSubsystem<FileSystem>();

			String projectdir = settings_->projectsRootDir_ + newProject_->name_;
			String templatedir = selectedtemplate_->path_;
			if (templatedir.EndsWith("/") )
			{
				templatedir.Erase(templatedir.Length() - 1);
			}

			if (!fileSystem->DirExists(projectdir))
			{
				int  ret = -1;
 
#ifdef _WINDOWS
				// copies from template choice
				String command = "xcopy " + GetNativePath(templatedir) + " " + GetNativePath(projectdir) + " /e /i /h";
				ret  = system(command.CString());
				URHO3D_LOGINFOF("%s", command.CString());
				if (!ret)
				{
					// this copies Data & CoreData into specific project
					// this is good when you want Data & CoreData to be unique to this project
#if 0
					String datapath = templateManager_->GetTemplatesPath() +"Data";
					command = "xcopy " + GetNativePath(datapath) + " " + GetNativePath(projectdir + "/Data") + " /e /i /h";
					ret = system(command.CString());

					String datacorepath = templateManager_->GetTemplatesPath() + "CoreData";
					command = "xcopy " + GetNativePath(datacorepath) + " " + GetNativePath(projectdir + "/CoreData") + " /e /i /h";
					ret = system(command.CString());
#endif
					// modify main.cpp
					String maincpp = GetNativePath(projectdir + "/main.cpp");
					if (!fileSystem->DirExists(maincpp))
					{
						long size;
						char * buffer = readfile(maincpp.CString(), &size);
						String test = buffer;
						test.Replace("MyApp", newProject_->name_);
						free(buffer);
						writefile(maincpp.CString(), test.CString(), test.Length());
					}

					// modify cmakelists.txt
					String cmakeliststxt = GetNativePath(projectdir + "/cmakelists.txt");
					if (!fileSystem->DirExists(cmakeliststxt))
					{
						long size;
						char * buffer = readfile(cmakeliststxt.CString(), &size);
						String test = buffer;
						test.Replace("MyProjectName", newProject_->name_);
						test.Replace("MyExecutableName", newProject_->name_);
						free(buffer);
						writefile(cmakeliststxt.CString(), test.CString(), test.Length());
					}

					// modify Urho3DProject.xml
					String urho3dprojecttxt = GetNativePath(projectdir + "/Urho3DProject.xml");
					if (!fileSystem->DirExists(urho3dprojecttxt))
					{
						long size;
						char * buffer = readfile(urho3dprojecttxt.CString(), &size);
						String test = buffer;
						unsigned int found = test.Find("value=\"Project");
						if (found > 0)
						{
							unsigned int start = found + 7;
							found = found + 13;
							char c = test[++found];
							while (c!='\"')
							{
								c = test[++found];
							}
							String foundProjectName = test.Substring(start, found-start);
							test.Replace(foundProjectName, newProject_->name_);
						}

						free(buffer);
						writefile(urho3dprojecttxt.CString(), test.CString(), test.Length());
					}
				}
#endif

				if (ret)
					URHO3D_LOGERRORF("Could not create Folder %s", projectdir.CString());
				else
				{
					UpdateProjects(settings_->projectsRootDir_);
					selectedProject_ = newProject_;
					
					FileSystem* fileSystem = GetSubsystem<FileSystem>();

					File saveFile(context_, projectdir + "/Urho3DProject.xml", FILE_WRITE);
					XMLFile xmlFile(context_);
					XMLElement rootElem = xmlFile.CreateRoot("Urho3DProject");
					selectedProject_->SaveXML(rootElem);
					xmlFile.Save(saveFile);

					newProject_ = NULL;
					templateSlectedText_ = NULL;
					Text* b = NULL;
					b = dynamic_cast<Text*>(welcomeUI_->GetChild("OpenText", true));
					if (b)
						b->SetText(selectedProject_->name_);
				}	
			}
			else
				URHO3D_LOGERRORF("Project %s does already exists", newProject_->name_.CString());
			
			selectedtemplate_ = NULL;
		}

}

	bool ProjectManager::ChooseRootDir()
	{
		CreateDirSelector("Choose Project Root Dir", "Open", "Cancel", settings_->projectsRootDir_);

		SubscribeToEvent(dirSelector_, E_FILESELECTED, URHO3D_HANDLER(ProjectManager, HandleRootSelected));

		return false;
	}

	bool ProjectManager::OpenProject(const String& dir)
	{
		if (dir.Empty())
			return false;
		/// \todo open project and set selected project
		// Open Project event
		SendEvent(E_OPENPROJECT);
		return true;
	}

	bool ProjectManager::OpenProject(const ProjectSettings* proj)
	{
		if (proj == NULL)
			return false;
		// Open Project event
		SendEvent(E_OPENPROJECT);
		return true;
	}

	void ProjectManager::CreateDirSelector(const String& title, const String& ok, const String& cancel, const String& initialPath)
	{
		// Within the editor UI, the file selector is a kind of a "singleton". When the previous one is overwritten, also
		// the events subscribed from it are disconnected, so new ones are safe to subscribe.
		dirSelector_ = new  DirSelector(context_);
		dirSelector_->SetDefaultStyle(cache_->GetResource<XMLFile>("UI/DefaultStyle.xml"));
		dirSelector_->SetTitle(title);
		dirSelector_->SetPath(initialPath);
		dirSelector_->SetButtonTexts(ok, cancel);
		dirSelector_->SetDirectoryMode(true);

		IntVector2 size = dirSelector_->GetWindow()->GetSize();
		dirSelector_->GetWindow()->SetPosition((graphics_->GetWidth() - size.x_) / 2, (graphics_->GetHeight() - size.y_) / 2);
	}

	void ProjectManager::HandleRootSelected(StringHash eventType, VariantMap& eventData)
	{
		using namespace FileSelected;
		CloseDirSelector();

		// Check for OK
		if (eventData[P_OK].GetBool())
		{
			String fileName = eventData[P_FILENAME].GetString();
			UpdateProjects(fileName);
		}
	}

	void ProjectManager::HandleRescanProjects(StringHash eventType, VariantMap& eventData)
	{
		UpdateProjects(settings_->projectsRootDir_);
	}

	void ProjectManager::HandleOnTemplateClick(StringHash eventType, VariantMap& eventData)
	{
		using namespace UIMouseClick;


		UIElement* element = (UIElement*)eventData[P_ELEMENT].GetPtr();
		if (element)
		{
			unsigned index = element->GetVar(PROJECTINDEX_VAR).GetUInt();
			selectedtemplate_ = templateManager_->GetTemplateProjects()[index];
			if (templateSlectedText_)
			{
				templateSlectedText_->SetText("Selected Template: " + selectedtemplate_->name_);
			}

			if (newProject_)
			{
				newProject_->CopyAttr(selectedtemplate_);

				if (newProjectAttrCont_)
					newProjectAttrCont_->SetSerializableAttributes(newProject_);
			}
			
		}
	}

	void ProjectManager::CloseDirSelector(String& path)
	{
		// Save filter & path for next time
		path = dirSelector_->GetPath();
		CloseDirSelector();
	}

	void ProjectManager::CloseDirSelector()
	{
		if (dirSelector_)
		{
			delete dirSelector_;
			dirSelector_ = NULL;
		}
	}

	bool ProjectManager::SelectProject(const String& dir, const String& projectName)
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		Vector<String> result;
		fileSystem->ScanDir(result, dir, "*", SCAN_DIRS, false);
		projects_.Clear();
		for (unsigned i = 0; i < result.Size(); i++)
		{
			if (result[i] == ".." || result[i] == ".")
				continue;

			String path = dir + result[i];

			if (fileSystem->FileExists(path + "/Urho3DProject.xml"))
			{
				XMLFile* xmlFile = cache_->GetResource<XMLFile>(path + "/Urho3DProject.xml");

				SharedPtr<ProjectSettings> proj(new ProjectSettings(context_));

				proj->LoadXML(xmlFile->GetRoot());
				proj->path_ = path;

				if (proj->name_ == projectName)
				{
					projects_.Push(proj);
					selectedProject_ = proj.Get();
					return true;
				}
			}
		}
		return false;
	}

	void ProjectManager::UpdateProjects(const String& dir)
	{
		if (dir.Empty())
			return;

		settings_->projectsRootDir_ = dir;

		Text* tt = NULL;
		tt = dynamic_cast<Text*>(welcomeUI_->GetChild("RootPath", true));
		if (tt)
			tt->SetText(dir);

		FileSystem* fileSystem = GetSubsystem<FileSystem>();

		ListView* projectList_ = dynamic_cast<ListView*>(welcomeUI_->GetChild("ProjectListView", true));
		Vector<String> result;

		fileSystem->ScanDir(result, dir, "*", SCAN_DIRS, false);
		projectList_->RemoveAllItems();

		selectedProject_ = NULL;
		projects_.Clear();

		for (unsigned i = 0; i < result.Size(); i++)
		{
			if (result[i] == ".." || result[i] == ".")
				continue;
			String path = dir + result[i];

			if (fileSystem->FileExists(path + "/Urho3DProject.xml"))
			{
				XMLFile* xmlFile = cache_->GetResource<XMLFile>(path + "/Urho3DProject.xml");

				SharedPtr<ProjectSettings> proj(new ProjectSettings(context_));

				proj->LoadXML(xmlFile->GetRoot());
				proj->path_ = path;

				UIElement* panel = new UIElement(context_);
				panel->SetLayout(LM_HORIZONTAL, 4, IntRect(4, 4, 4, 4));
				panel->SetVar(PROJECTINDEX_VAR, projects_.Size());

				BorderImage* imag = new BorderImage(context_);
				Texture2D* iconTexture = cache_->GetResource<Texture2D>(path + "/" + proj->icon_);
				if (!iconTexture)
					iconTexture = cache_->GetResource<Texture2D>("Textures/Logo.png");
				imag->SetTexture(iconTexture);
				panel->AddChild(imag);
				projectList_->AddItem(panel);
				imag->SetFixedHeight(128);
				imag->SetFixedWidth(128);

				UIElement* panelInfo = new UIElement(context_);
				panelInfo->SetLayout(LM_VERTICAL);
				panel->AddChild(panelInfo);

				Text* text = new Text(context_);
				panelInfo->AddChild(text);
				text->SetStyle("FileSelectorListText");
				text->SetText(proj->name_);

				text = new Text(context_);
				panelInfo->AddChild(text);
				text->SetStyle("FileSelectorListText");
				text->SetText(dir + result[i]);

				projects_.Push(proj);
			}
		}
	}

	void ProjectManager::ShowWelcomeScreen(bool value)
	{
		if (welcomeUI_.NotNull())
		{
			if (value)
			{
				welcomeUI_->SetVisible(true);
				welcomeUI_->SetEnabled(true);
				UpdateProjects(settings_->projectsRootDir_);
				templateManager_->LoadTemplates();
			}
			else
			{
				welcomeUI_->SetVisible(false);
				welcomeUI_->SetEnabled(false);
			}
			return;
		}
	}

	UIElement* ProjectManager::CreateWelcomeScreen()
	{
		if (welcomeUI_.NotNull())
			return welcomeUI_;

		templateManager_ = new TemplateManager(context_);
		templateManager_->LoadTemplates();
		cache_		= GetSubsystem<ResourceCache>();
		graphics_	= GetSubsystem<Graphics>();
		ui_			= GetSubsystem<UI>();

		XMLFile* styleFile = cache_->GetResource<XMLFile>("UI/IDEStyle.xml");

		welcomeUI_ = new UIElement(context_);
		welcomeUI_->SetDefaultStyle(styleFile);
		welcomeUI_->SetName("WelcomeScreen");
		welcomeUI_->LoadXML(cache_->GetResource<XMLFile>("UI/WelcomeScreen.xml")->GetRoot());

		Button* b = NULL;

		b = dynamic_cast<Button*>(welcomeUI_->GetChild("NewProject", true));
		SubscribeToEvent(b, E_RELEASED, URHO3D_HANDLER(ProjectManager, HandleNewProject));
		b = dynamic_cast<Button*>(welcomeUI_->GetChild("ChangeRoot", true));
		SubscribeToEvent(b, E_RELEASED, URHO3D_HANDLER(ProjectManager, HandleChangeRootDir));
		b = dynamic_cast<Button*>(welcomeUI_->GetChild("OpenProject", true));
		SubscribeToEvent(b, E_RELEASED, URHO3D_HANDLER(ProjectManager, HandleOpenProject));
		b = dynamic_cast<Button*>(welcomeUI_->GetChild("RescanButton", true));
		SubscribeToEvent(b, E_RELEASED, URHO3D_HANDLER(ProjectManager, HandleRescanProjects));

		UpdateProjects(settings_->projectsRootDir_);

		ListView*	hierarchyList = dynamic_cast<ListView*>(welcomeUI_->GetChild("ProjectListView", true));

		SubscribeToEvent(hierarchyList, E_ITEMCLICKED, URHO3D_HANDLER(ProjectManager, HandleProjectListClick));

		Text* tt = NULL;
		tt = dynamic_cast<Text*>(welcomeUI_->GetChild("RootPath", true));
		if (tt)
			tt->SetText(settings_->projectsRootDir_);

		return welcomeUI_;
	}

	void ProjectManager::HandleNewProject(StringHash eventType, VariantMap& eventData)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		XMLFile* styleFile = cache_->GetResource<XMLFile>("UI/IDEStyle.xml");
		XMLFile* iconstyle = cache_->GetResource<XMLFile>("UI/IDEIcons.xml");

		UIElement* tempview = new UIElement(context_);
		tempview->SetLayout(LM_VERTICAL, 2);

		ScrollView* TemplateScrollView = new ScrollView(context_);
		TemplateScrollView->SetDefaultStyle(styleFile);
		Font* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");
		UIElement* templist = new UIElement(context_);
		templist->SetLayout(LM_HORIZONTAL, 4);
		TemplateScrollView->SetContentElement(templist);

		TemplateScrollView->SetStyleAuto();

		templateSlectedText_ = tempview->CreateChild<Text>();
		templateSlectedText_->SetFont(font, 12);
		templateSlectedText_->SetText("Selected Template:");

		for (unsigned i = 0; i < templateManager_->GetTemplateProjects().Size(); i++)
		{
			Button* panel = new Button(context_);
			panel->SetLayout(LM_VERTICAL, 4, IntRect(4, 4, 4, 4));
			panel->SetVar(PROJECTINDEX_VAR, i);

			BorderImage* imag = new BorderImage(context_);
			Texture2D* iconTexture = cache->GetResource<Texture2D>(templateManager_->GetTemplateProjects()[i]->path_ + templateManager_->GetTemplateProjects()[i]->icon_);
			if (!iconTexture)
				iconTexture = cache->GetResource<Texture2D>("Textures/Logo.png");

			imag->SetTexture(iconTexture);
			panel->AddChild(imag);
			templist->AddChild(panel);
			imag->SetFixedHeight(96);
			imag->SetFixedWidth(96);
			panel->SetStyleAuto();
			Text* nametile = panel->CreateChild<Text>();
			nametile->SetFont(font, 12);
			nametile->SetText(templateManager_->GetTemplateProjects()[i]->name_);

			SubscribeToEvent(panel, E_RELEASED, URHO3D_HANDLER(ProjectManager, HandleOnTemplateClick));
		}

		tempview->AddChild(TemplateScrollView);
	
		newProject_ = new ProjectSettings(context_);

		newProjectAttrCont_ = new AttributeContainer(context_);
		newProjectAttrCont_->SetDefaultStyle(styleFile);
		newProjectAttrCont_->SetTitle("ProjectSettings");
		newProjectAttrCont_->SetSerializableAttributes(newProject_);
		newProjectAttrCont_->GetResetToDefault()->SetVisible(false);
		// Resize the node editor according to the number of variables, up to a certain maximum
		unsigned int maxAttrs = newProjectAttrCont_->GetAttributeList()->GetContentElement()->GetNumChildren();
		newProjectAttrCont_->GetAttributeList()->SetHeight(maxAttrs * ATTR_HEIGHT + 4);
		newProjectAttrCont_->SetFixedWidth(ATTRNAME_WIDTH * 2);
		newProjectAttrCont_->SetStyleAuto(styleFile);

		tempview->AddChild(newProjectAttrCont_);

		SharedPtr<Urho3D::ModalWindow> newProjectWindow(new Urho3D::ModalWindow(context_, tempview, "Create New Project"));

		if (newProjectWindow->GetWindow() != NULL)
		{
			Button* cancelButton = (Button*)newProjectWindow->GetWindow()->GetChild("CancelButton", true);
			cancelButton->SetVisible(true);
			cancelButton->SetFocus(true);
			SubscribeToEvent(newProjectWindow, E_MESSAGEACK, URHO3D_HANDLER(ProjectManager, HandleNewProjectAck));
		}

		newProjectWindow->AddRef();
	}

	void ProjectManager::HandleNewProjectAck(StringHash eventType, VariantMap& eventData)
	{
		using namespace MessageACK;

		bool ok_ = eventData[P_OK].GetBool();

		if (ok_)
			NewProject();
					
	}

	void ProjectManager::HandleOpenProject(StringHash eventType, VariantMap& eventData)
	{
		if (selectedProject_)
		{
			OpenProject(selectedProject_.Get());
		}
		
	}

	void ProjectManager::HandleChangeRootDir(StringHash eventType, VariantMap& eventData)
	{
		ChooseRootDir();
	}

	const String& ProjectManager::GetRootDir()
	{
		return settings_->projectsRootDir_;
	}

	void ProjectManager::HandleProjectListClick(StringHash eventType, VariantMap& eventData)
	{
		using namespace ItemClicked;
		UIElement* e = (UIElement*)eventData[P_ITEM].GetPtr();

		if (e)
		{
			const Variant& v = e->GetVar(PROJECTINDEX_VAR);
			if (v.IsEmpty())
				return;
			selectedProject_ = projects_[v.GetUInt()];

			Text* b = NULL;
			b = dynamic_cast<Text*>(welcomeUI_->GetChild("OpenText", true));
			if (b)
				b->SetText(selectedProject_->name_);		
		}
	}

	ProjectSettings* ProjectManager::GetProject()
	{
		return selectedProject_.Get();
	}

	ProjectSettings::ProjectSettings(Context* context) : Serializable(context),
		icon_("Logo.png"),
		name_("noName"),
		resFolders_(""),
		mainScript_(""),
		mainScene_("")
	{

	}

	ProjectSettings::~ProjectSettings()
	{

	}

	void ProjectSettings::RegisterObject(Context* context)
	{
		context->RegisterFactory<ProjectSettings>();

		URHO3D_ATTRIBUTE("Name", String, name_, String("noName"), AM_FILE);
		URHO3D_ATTRIBUTE("Icon", String, icon_, String::EMPTY, AM_FILE);
		URHO3D_ATTRIBUTE("Resource Folders", String, resFolders_, String::EMPTY, AM_FILE);
		URHO3D_ATTRIBUTE("Main Script", String, mainScript_, String::EMPTY, AM_FILE);
		URHO3D_ATTRIBUTE("Main Scene", String, mainScene_, String::EMPTY, AM_FILE);
	}

	void ProjectSettings::CopyAttr(ProjectSettings* proj)
	{
		if (!proj)
			return;

		name_ = proj->name_;
		icon_ = proj->icon_;
		resFolders_ = proj->resFolders_;
		mainScript_ = proj->mainScript_;
		mainScene_ = proj->mainScene_;
	}

}