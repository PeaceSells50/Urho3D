#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include "ResourceBrowser.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/UI/View3D.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Graphics/Texture.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/IO/Serializer.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Math/BoundingBox.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Input/InputEvents.h>
#include "EditorView.h"
#include "MenuBarUI.h"
#include "UIGlobals.h"
#include "../UI/TabWindow.h"

namespace Urho3D
{
	const unsigned int BROWSER_WORKER_ITEMS_PER_TICK = 10;
	const unsigned int BROWSER_SEARCH_LIMIT = 50;
	const int BROWSER_SORT_MODE_ALPHA = 1;
	const int BROWSER_SORT_MODE_SEARCH = 2;

	enum
	{
		RESOURCE_TYPE_UNUSABLE,
		RESOURCE_TYPE_UNKNOWN,
		RESOURCE_TYPE_NOTSET,
		RESOURCE_TYPE_SCENE,
		RESOURCE_TYPE_SCRIPTFILE,
		RESOURCE_TYPE_MODEL,
		RESOURCE_TYPE_MATERIAL,
		RESOURCE_TYPE_ANIMATION,
		RESOURCE_TYPE_IMAGE,
		RESOURCE_TYPE_SOUND,
		RESOURCE_TYPE_TEXTURE,
		RESOURCE_TYPE_FONT,
		RESOURCE_TYPE_PREFAB,
		RESOURCE_TYPE_TECHNIQUE,
		RESOURCE_TYPE_PARTICLEEFFECT,
		RESOURCE_TYPE_UIELEMENT,
		RESOURCE_TYPE_UIELEMENTS,
		RESOURCE_TYPE_ANIMATION_SETTINGS,
		RESOURCE_TYPE_RENDERPATH,
		RESOURCE_TYPE_TEXTURE_ATLAS,
		RESOURCE_TYPE_2D_PARTICLE_EFFECT,
		RESOURCE_TYPE_TEXTURE_3D,
		RESOURCE_TYPE_CUBEMAP,
		RESOURCE_TYPE_PARTICLEEMITTER,
		RESOURCE_TYPE_2D_ANIMATION_SET,
		NUM_RESOURCE_TYPES
	};
	static const char *resourceTypesNames[] =
	{
		"Unusable",
		"Unknown",
		"Uninitialized",
		"Scene",
		"Script File",
		"Model",
		"Material",
		"Animation",
		"Image",
		"Sound",
		"Texture",
		"Font",
		"Prefab",
		"Render Technique",
		"Particle Effect",
		"UI Element",
		"UI Elements",
		"Animation Settings",
		"Render Path",
		"Texture Atlas",
		"2D Particle Effect",
		"Texture 3D",
		"Cubemap",
		"Particle Emitter",
		"2D Animation Set",
	};

	enum
	{
		XML_TYPE_SCENE,
		XML_TYPE_NODE,
		XML_TYPE_MATERIAL,
		XML_TYPE_TECHNIQUE,
		XML_TYPE_PARTICLEEFFECT,
		XML_TYPE_PARTICLEEMITTER,
		XML_TYPE_TEXTURE,
		XML_TYPE_ELEMENT,
		XML_TYPE_ELEMENTS,
		XML_TYPE_ANIMATION_SETTINGS,
		XML_TYPE_RENDERPATH,
		XML_TYPE_TEXTURE_ATLAS,
		XML_TYPE_2D_PARTICLE_EFFECT,
		XML_TYPE_TEXTURE_3D,
		XML_TYPE_CUBEMAP,
		XML_TYPE_SPRITER_DATA,
		NUM_XML_TYPES,

		BINARY_TYPE_SCENE,
		BINARY_TYPE_PACKAGE,
		BINARY_TYPE_COMPRESSED_PACKAGE,
		BINARY_TYPE_ANGLESCRIPT,
		BINARY_TYPE_MODEL,
		BINARY_TYPE_SHADER,
		BINARY_TYPE_ANIMATION,
		NUM_BINARY_TYPES,

		EXTENSION_TYPE_TTF,
		EXTENSION_TYPE_OTF,
		EXTENSION_TYPE_OGG,
		EXTENSION_TYPE_WAV,
		EXTENSION_TYPE_DDS,
		EXTENSION_TYPE_PNG,
		EXTENSION_TYPE_JPG,
		EXTENSION_TYPE_JPEG,
		EXTENSION_TYPE_TGA,
		EXTENSION_TYPE_OBJ,
		EXTENSION_TYPE_FBX,
		EXTENSION_TYPE_COLLADA,
		EXTENSION_TYPE_BLEND,
		EXTENSION_TYPE_ANGELSCRIPT,
		EXTENSION_TYPE_LUASCRIPT,
		EXTENSION_TYPE_HLSL,
		EXTENSION_TYPE_GLSL,
		EXTENSION_TYPE_FRAGMENTSHADER,
		EXTENSION_TYPE_VERTEXSHADER,
		EXTENSION_TYPE_HTML,
		NUM_EXTENSION_TYPES
	};
	static const char *typeNames[] =
	{
		"scene",
		"node",
		"material",
		"technique",
		"particleeffect",
		"particleemitter",
		"texture",
		"element",
		"elements",
		"animation",
		"renderpath",
		"TextureAtlas",
		"particleEmitterConfig",
		"texture3d",
		"cubemap",
		"spriter_data",
		"",
		"USCN",
		"UPAK",
		"ULZ4",
		"ASBC",
		"UMDL",
		"USHD",
		"UANI",
		"",
		"ttf",
		"otf",
		"ogg",
		"wav",
		"dds",
		"png",
		"jpg",
		"jpeg",
		"tga",
		"obj",
		"fbx",
		"dae",
		"blend",
		"as",
		"lua",
		"hlsl",
		"glsl",
		"frag",
		"vert",
		"html"
	};

	static const int resourceMapping[] =
	{
		RESOURCE_TYPE_SCENE,				// XML_TYPE_SCENE
		RESOURCE_TYPE_PREFAB,				// XML_TYPE_NODE
		RESOURCE_TYPE_MATERIAL,				// XML_TYPE_MATERIAL
		RESOURCE_TYPE_TECHNIQUE,			// XML_TYPE_TECHNIQUE
		RESOURCE_TYPE_PARTICLEEFFECT,		// XML_TYPE_PARTICLEEFFECT
		RESOURCE_TYPE_PARTICLEEMITTER,		// XML_TYPE_PARTICLEEMITTER
		RESOURCE_TYPE_TEXTURE,				// XML_TYPE_TEXTURE
		RESOURCE_TYPE_UIELEMENT,			// XML_TYPE_UIELEMENT
		RESOURCE_TYPE_UIELEMENTS,			// XML_TYPE_UIELEMENTS
		RESOURCE_TYPE_ANIMATION_SETTINGS,	// XML_TYPE_ANIMATION_SETTINGS
		RESOURCE_TYPE_RENDERPATH,			// XML_TYPE_RENDERPATH
		RESOURCE_TYPE_TEXTURE_ATLAS,		// XML_TYPE_TEXTURE_ATLAS
		RESOURCE_TYPE_2D_PARTICLE_EFFECT,	// XML_TYPE_2D_PARTICLE_EFFECT
		RESOURCE_TYPE_TEXTURE_3D,			// XML_TYPE_TEXTURE_3D
		RESOURCE_TYPE_CUBEMAP,				// XML_TYPE_CUBEMAP
		RESOURCE_TYPE_2D_ANIMATION_SET,		// XML_TYPE_2D_ANIMATION_SET

		RESOURCE_TYPE_SCENE,				// BINARY_TYPE_SCENE
		RESOURCE_TYPE_UNUSABLE,				// BINARY_TYPE_PACKAGE
		RESOURCE_TYPE_UNUSABLE,				// BINARY_TYPE_COMPRESSED_PACKAGE
		RESOURCE_TYPE_SCRIPTFILE,			// BINARY_TYPE_ANGLESCRIPT
		RESOURCE_TYPE_MODEL,				// BINARY_TYPE_MODEL
		RESOURCE_TYPE_UNUSABLE,				// BINARY_TYPE_SHADER
		RESOURCE_TYPE_ANIMATION,			// BINARY_TYPE_ANIMATION

		RESOURCE_TYPE_FONT,					// EXTENSION_TYPE_TTF
		RESOURCE_TYPE_FONT,					// EXTENSION_TYPE_OTF
		RESOURCE_TYPE_SOUND,				// EXTENSION_TYPE_OGG
		RESOURCE_TYPE_SOUND,				// EXTENSION_TYPE_WAV
		RESOURCE_TYPE_IMAGE,				// EXTENSION_TYPE_DDS
		RESOURCE_TYPE_IMAGE,				// EXTENSION_TYPE_PNG
		RESOURCE_TYPE_IMAGE,				// EXTENSION_TYPE_JPG
		RESOURCE_TYPE_IMAGE,				// EXTENSION_TYPE_JPEG
		RESOURCE_TYPE_IMAGE,				// EXTENSION_TYPE_TGA
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_OBJ
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_FBX
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_COLLADA
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_BLEND
		RESOURCE_TYPE_SCRIPTFILE,			// EXTENSION_TYPE_ANGELSCRIPT
		RESOURCE_TYPE_SCRIPTFILE,			// EXTENSION_TYPE_LUASCRIPT
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_HLSL
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_GLSL
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_FRAGMENTSHADER
		RESOURCE_TYPE_UNUSABLE,				// EXTENSION_TYPE_VERTEXSHADER
		RESOURCE_TYPE_UNUSABLE				// EXTENSION_TYPE_HTML
	};

	const StringHash TEXT_VAR_FILE_ID("browser_file_id");
	const StringHash TEXT_VAR_DIR_ID("browser_dir_id");
	const StringHash TEXT_VAR_RESOURCE_TYPE("resource_type");
	const StringHash TEXT_VAR_RESOURCE_DIR_ID("resource_dir_id");

	const int BROWSER_FILE_SOURCE_RESOURCE_DIR = 1;

	unsigned int BrowserFile::browserFileIndex = 1;
	unsigned int BrowserDir::browserDirIndex = 1;

	bool BrowserFileopCmp(BrowserFile* a, BrowserFile* b)
	{
		return a->fullname == (b->fullname);
	}
	bool BrowserFileopCmpScore(BrowserFile* a, BrowserFile* b)
	{
		return a->sortScore > b->sortScore;
	}
	bool ResourceTypeopCmp(ResourceType& a, ResourceType& b)
	{
		return a.name == b.name;
	}
	bool BrowserDiropCmp(BrowserDir* a, BrowserDir* b)
	{
		return a->name == b->name;
	}
	String Join(const Vector<String>& strings, String seperator)
	{
		String ret;
		if (strings.Empty())
			return ret;
		ret.Append(strings[0]);

		for (unsigned i = 1; i < strings.Size(); i++)
		{
			ret.Append(seperator);
			ret.Append(strings[i]);
		}
		return ret;
	}

	//////////////////////////////////////////////////////////////////////////
	/// get ResourceType functions
	namespace Res
	{
		int GetResourceType(Context* context, String path, int& fileType, ResourceCache* cache = NULL);

		int GetResourceType(int fileType)
		{
			return resourceMapping[fileType];
		}

		bool GetExtensionType(String path, int& fileType)
		{
			for (int i = EXTENSION_TYPE_TTF; i < NUM_EXTENSION_TYPES; i++)
			{
				if (path == String(typeNames[i]))
				{
					fileType = i;
					return true;
				}
			}
		
			return false;
		}

		bool GetBinaryType(Context* context, String path, int& fileType, ResourceCache* cache = NULL)
		{
			String typeName;
			if (cache)
			{
				SharedPtr<File> file = cache->GetFile(path);
				if (file.Null())
					return false;

				if (file->GetSize() == 0)
					return false;

				typeName = file->ReadFileID();
			}
			else
			{
				File file(context);
				if (!file.Open(path))
					return false;

				if (file.GetSize() == 0)
					return false;

				typeName = file.ReadFileID();
			}

			// return index that gives resource enumerator

			int type = 0;
			for (int i = BINARY_TYPE_SCENE; i < NUM_BINARY_TYPES; i++)
			{
				if (path == String(typeNames[i]))
				{
					fileType = resourceMapping[i];
					return true;
				}
			}
		
			return false;
		}

		bool GetXmlType(Context* context, String path, int& fileType, ResourceCache* cache = NULL)
		{
			// I added this because it was actually testing .txt files by trying to load them as XML.
			String ext;
			ext = GetExtension(path, true);
			if (ext != ".xml")
				return false;

			String name;
			if (cache)
			{
				XMLFile* xml = cache->GetResource<XMLFile>(path);
				if (xml == NULL)
					return false;

				name = xml->GetRoot().GetName();
			}
			else
			{
				File file(context);
				if (!file.Open(path))
					return false;

				if (file.GetSize() == 0)
					return false;

				XMLFile xml(context);
				if (xml.Load(file))
					name = xml.GetRoot().GetName();
				else
					return false;
			}

			bool found = false;
			if (!name.Empty())
			{
				found = false;
				for (int i = 0; i < NUM_XML_TYPES; i++)
				{
					if (name == String(typeNames[i]))
					{
						fileType = i;
						found = true;
					}
				}
			}
			return found;
		}

		String ResourceTypeName(int resourceType)
		{
			return resourceTypesNames[resourceType];
		}

		int GetResourceType(Context* context, String path)
		{
			int fileType;
			return GetResourceType(context, path, fileType);
		}

		int GetResourceType(Context* context, String path, int & fileType, ResourceCache* cache)
		{
			if (GetExtensionType(path, fileType) || GetBinaryType(context, path, fileType, cache) || GetXmlType(context, path, fileType, cache))
				return GetResourceType(fileType);

			return RESOURCE_TYPE_UNKNOWN;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// class : BrowserDir
	BrowserDir::BrowserDir(String path_, ResourceCache* cache, ResourceBrowser* resBrowser)
	{
		resourceKey = path_;
		String parent = GetParentPath(path_);
		name = path_;
		name.Replace(parent, "");
		id = browserDirIndex++;
		cache_ = cache;
		resBrowser_ = resBrowser;
	}

	BrowserDir::~BrowserDir()
	{
		for (unsigned i = 0; i < files.Size(); i++)
		{
			if (files[i])
			{
				delete files[i];
				files[i] = NULL;
			}
		}

		files.Clear();
	}

	int BrowserDir::opCmp(BrowserDir& b)
	{
		return name == b.name;
	}

	//////////////////////////////////////////////////////////////////////////
	/// class : BrowserFile
	BrowserFile* BrowserDir::AddFile(String name, unsigned int resourceSourceIndex, unsigned int sourceType)
	{
		String path = resourceKey + "/" + name;
		BrowserFile* file = new BrowserFile(path, resourceSourceIndex, sourceType, cache_, resBrowser_);
		files.Push(file);
		return file;
	}

	BrowserFile::BrowserFile(String path_, unsigned int resourceSourceIndex_, int sourceType_, ResourceCache* cache, ResourceBrowser* resBrowser)
	{
		sourceType = sourceType_;
		resourceSourceIndex = resourceSourceIndex_;
		resourceKey = path_;
		name = GetFileName(path_);
		extension = GetExtension(path_);
		fullname = GetFileNameAndExtension(path_);
		id = browserFileIndex++;
		cache_ = cache;
		resBrowser_ = resBrowser;
	}

	int BrowserFile::opCmp(BrowserFile& b)
	{
		//if (browserSearchSortMode == 1)
		return fullname== (b.fullname);
	}

	int BrowserFile::opCmpScore(BrowserFile& b)
	{
		return sortScore - b.sortScore;
	}

	Urho3D::String BrowserFile::GetResourceSource()
	{
		if (sourceType == BROWSER_FILE_SOURCE_RESOURCE_DIR)
			return cache_->GetResourceDirs()[resourceSourceIndex];
		else
			return "Unknown";
	}

	Urho3D::String BrowserFile::GetFullPath()
	{
		return String(cache_->GetResourceDirs()[resourceSourceIndex] + resourceKey);
	}

	Urho3D::String BrowserFile::GetPath()
	{
		return resourceKey;
	}

	void BrowserFile::DetermainResourceType()
	{
		resourceType = Res::GetResourceType(cache_->GetContext(), GetFullPath(), fileType, NULL);
		Text* browserFileListRow_ = browserFileListRow.Get();
		if (browserFileListRow_ != NULL)
		{
			resBrowser_->InitializeBrowserFileListRow(browserFileListRow_, this);
		}
	}

	Urho3D::String BrowserFile::ResourceTypeName()
	{
		return Res::ResourceTypeName(resourceType);
	}

	void BrowserFile::FileChanged()
	{
		// 		if (!fileSystem.FileExists(GetFullPath()))
		// 		{
		// 		}
		// 		else
		// 		{
		// 		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// class : ResourceBrowser
	ResourceBrowser::ResourceBrowser(Context* context) : Object(context)
	{
		selectedBrowserDirectory = NULL;
		selectedBrowserFile = NULL;
		browserDragFile = NULL;
		rootDir = NULL;
		cache_ = GetSubsystem<ResourceCache>();
		ui_ = GetSubsystem<UI>();
		fileSystem_ = GetSubsystem<FileSystem>();
	}

	ResourceBrowser::~ResourceBrowser()
	{
		HashMap<String, BrowserDir*>::Iterator it;

		for (it = browserDirs.Begin(); it != browserDirs.End(); it++)
		{
			if (it->second_)
			{
				delete it->second_;
				it->second_ = NULL;
			}
		}
		browserDirs.Clear();
	}

	void ResourceBrowser::CreateResourceBrowser()
	{
		if (browserWindow != NULL) return;

		CreateResourceBrowserUI();
		InitResourceBrowserPreview();
		RebuildResourceDatabase();

		/// add an Window Menu Item and add it 
		EditorView* editorView = GetSubsystem<EditorView>();
		Menu* sceneMenu_ = editorView->GetGetMenuBar()->CreateMenu("Window");
		editorView->GetGetMenuBar()->CreateMenuItem("Window", "Resource Browser", A_SHOWRESOURCE_VAR);
		SubscribeToEvent(editorView->GetGetMenuBar(), E_MENUBAR_ACTION, URHO3D_HANDLER(ResourceBrowser, HandleMenuBarAction));
	}

	void ResourceBrowser::Update()
	{
		if (browserFilesToScan.Size() == 0)
			return;

		int counter = 0;
		bool updateBrowserUI = false;
		BrowserFile* scanItem = browserFilesToScan[0];
		while (counter < BROWSER_WORKER_ITEMS_PER_TICK)
		{
			scanItem->DetermainResourceType();

			// next
			browserFilesToScan.Erase(0);
			if (browserFilesToScan.Size() > 0)
				scanItem = browserFilesToScan[0];
			else
				break;
			counter++;
		}

		if (browserFilesToScan.Size() > 0)
			browserStatusMessage->SetText("Files left to scan: " + String(browserFilesToScan.Size()));
		else
			browserStatusMessage->SetText("Scan complete");
	}

	bool ResourceBrowser::IsVisible()
	{
		return browserWindow->IsVisible();
	}

	void ResourceBrowser::HideResourceBrowserWindow(StringHash eventType, VariantMap& eventData)
	{
		browserWindow->SetVisible(false);
		EditorView* editorView = GetSubsystem<EditorView>();
		editorView->GetLeftFrame()->RemoveTab("Resources");
	}

	bool ResourceBrowser::ShowResourceBrowserWindow()
	{
		browserWindow->SetVisible(true);
		browserWindow->BringToFront();
		ui_->SetFocusElement(browserSearch);

		EditorView* editorView = GetSubsystem<EditorView>();
		unsigned index = editorView->GetLeftFrame()->AddTab("Resources", browserWindow);
		editorView->GetLeftFrame()->SetActiveTab(index);

		return true;
	}

	void ResourceBrowser::ToggleResourceFilterWindow(StringHash eventType, VariantMap& eventData)
	{
		if (browserFilterWindow->IsVisible())
			HideResourceFilterWindow();
		else
			ShowResourceFilterWindow();
	}

	void ResourceBrowser::HideResourceFilterWindow()
	{
		browserFilterWindow->SetVisible(false);
	}

	void ResourceBrowser::ShowResourceFilterWindow()
	{
		int x = browserWindow->GetPosition().x_ + browserWindow->GetWidth();
		int y = browserWindow->GetPosition().y_;
		browserFilterWindow->SetPosition(IntVector2(x, y));
		browserFilterWindow->SetVisible(true);
		browserFilterWindow->BringToFront();
	}

	void ResourceBrowser::RefreshBrowserPreview()
	{
		resourceBrowserPreview->QueueUpdate();
	}

	void ResourceBrowser::ScanResourceDirectories()
	{
		HashMap<String, BrowserDir*>::Iterator it;

		for (it = browserDirs.Begin(); it != browserDirs.End(); it++)
		{
			if (it->second_)
			{
				delete it->second_;
				it->second_ = NULL;
			}
		}
		browserDirs.Clear();
		browserFiles.Clear();
		browserFilesToScan.Clear();

		rootDir = new BrowserDir("", cache_, this);
		browserDirs[""] = rootDir;

		// collect all of the items and sort them afterwards
		for (unsigned int i = 0; i < cache_->GetResourceDirs().Size(); ++i)
		{
			if (activeResourceDirFilters.Find(i) != activeResourceDirFilters.End())
				continue;

			ScanResourceDir(i);
		}
	}

	void ResourceBrowser::ScanResourceDir(unsigned int resourceDirIndex)
	{
		String resourceDir = cache_->GetResourceDirs()[resourceDirIndex];

		ScanResourceDirFiles("", resourceDirIndex);

		Vector<String> dirs;
		fileSystem_->ScanDir(dirs, resourceDir, "*", SCAN_DIRS, true);

		for (unsigned int i = 0; i < dirs.Size(); ++i)
		{
			String path = dirs[i];
			if (path.EndsWith("."))
				continue;

			InitBrowserDir(path);
			ScanResourceDirFiles(path, resourceDirIndex);
		}
	}

	void ResourceBrowser::ScanResourceDirFiles(String path, unsigned int resourceDirIndex)
	{
		String fullPath = cache_->GetResourceDirs()[resourceDirIndex] + path;
		if (!fileSystem_->DirExists(fullPath))
			return;

		BrowserDir* dir = GetBrowserDir(path);

		if (dir == NULL)
			return;

		// get files in directory
		Vector<String> dirFiles;
		fileSystem_->ScanDir(dirFiles, fullPath, "*.*", SCAN_FILES, false);

		// add new files
		for (unsigned int x = 0; x < dirFiles.Size(); x++)
		{
			String filename = dirFiles[x];
			BrowserFile* browserFile = dir->AddFile(filename, resourceDirIndex, BROWSER_FILE_SOURCE_RESOURCE_DIR);
			browserFiles.Push(browserFile);
			browserFilesToScan.Push(browserFile);
		}
	}

	void ResourceBrowser::HandleMenuBarAction(StringHash eventType, VariantMap& eventData)
	{
		using namespace MenuBarAction;

		StringHash action = eventData[P_ACTION].GetStringHash();
		if (action == A_SHOWRESOURCE_VAR)
		{
			ui_->SetFocusElement(NULL);
			ShowResourceBrowserWindow();
		}
	}

	void ResourceBrowser::HandleRescanResourceBrowserClick(StringHash eventType, VariantMap& eventData)
	{
		RebuildResourceDatabase();
	}

	void ResourceBrowser::HandleResourceBrowserDirListSelectionChange(StringHash eventType, VariantMap& eventData)
	{
		if (browserDirList->GetSelection() == M_MAX_UNSIGNED)
			return;

		UIElement* uiElement = browserDirList->GetItems()[browserDirList->GetSelection()];
		BrowserDir* dir = GetBrowserDir(uiElement->GetVar(TEXT_VAR_DIR_ID).GetString());
		if (dir == NULL)
			return;

		PopulateResourceBrowserFilesByDirectory(dir);
	}

	void ResourceBrowser::HandleResourceBrowserFileListSelectionChange(StringHash eventType, VariantMap& eventData)
	{
		if (browserFileList->GetSelection() == M_MAX_UNSIGNED)
			return;

		UIElement* uiElement = browserFileList->GetItems()[browserFileList->GetSelection()];
		BrowserFile* file = GetBrowserFileFromUIElement(uiElement);
		if (file == NULL)
			return;

		if (resourcePreviewNode != NULL)
			resourcePreviewNode->Remove();

		resourcePreviewNode = resourcePreviewScene->CreateChild("PreviewNodeContainer");
		CreateResourcePreview(file->GetFullPath(), resourcePreviewNode);

		if (resourcePreviewNode != NULL)
		{
			Vector<BoundingBox> boxes;

			PODVector<StaticModel*> staticModels;
			resourcePreviewNode->GetComponents<StaticModel>(staticModels, true);
			PODVector<AnimatedModel*> animatedModels;
			resourcePreviewNode->GetComponents<AnimatedModel>(animatedModels, true);

			for (unsigned i = 0; i < staticModels.Size(); ++i)
				boxes.Push(staticModels[i]->GetWorldBoundingBox());

			for (unsigned i = 0; i < animatedModels.Size(); ++i)
				boxes.Push(animatedModels[i]->GetWorldBoundingBox());

			if (boxes.Size() > 0)
			{
				Vector3 camPosition = Vector3(0.0f, 0.0f, -1.2f);
				BoundingBox biggestBox = boxes[0];
				for (unsigned i = 1; i < boxes.Size(); ++i)
				{
					if (boxes[i].Size().Length() > biggestBox.Size().Length())
						biggestBox = boxes[i];
				}
				resourcePreviewCameraNode->SetPosition(biggestBox.Center() + camPosition * biggestBox.Size().Length());
			}

			resourcePreviewScene->AddChild(resourcePreviewNode);
			RefreshBrowserPreview();
		}
	}

	void ResourceBrowser::HandleResourceDirFilterToggled(StringHash eventType, VariantMap& eventData)
	{
		using namespace Toggled;
		CheckBox* checkbox = dynamic_cast<CheckBox*>(eventData[P_ELEMENT].GetPtr());
		if (!checkbox || !checkbox->GetVars().Contains(TEXT_VAR_RESOURCE_DIR_ID))
			return;

		int resourceDir = checkbox->GetVar(TEXT_VAR_RESOURCE_DIR_ID).GetInt();
		Vector<int>::Iterator find = activeResourceDirFilters.Find(resourceDir);

		if (checkbox->IsChecked() && find != activeResourceDirFilters.End())
			activeResourceDirFilters.Erase(find);
		else if (!checkbox->IsChecked() && find == activeResourceDirFilters.End())
			activeResourceDirFilters.Push(resourceDir);

		if (ignoreRefreshBrowserResults == false)
			RebuildResourceDatabase();
	}

	void ResourceBrowser::HandleResourceBrowserSearchTextChange(StringHash eventType, VariantMap& eventData)
	{
		RefreshBrowserResults();
	}

	void ResourceBrowser::HandleResourceTypeFilterToggled(StringHash eventType, VariantMap& eventData)
	{
		using namespace Toggled;
		CheckBox* checkbox = dynamic_cast<CheckBox*>(eventData[P_ELEMENT].GetPtr());

		if (!checkbox || !checkbox->GetVars().Contains(TEXT_VAR_RESOURCE_TYPE))
			return;

		int resourceType = checkbox->GetVar(TEXT_VAR_RESOURCE_TYPE).GetInt();
		Vector<int>::Iterator find = activeResourceTypeFilters.Find(resourceType);

		if (checkbox->IsChecked() && find != activeResourceTypeFilters.End())
			activeResourceTypeFilters.Erase(find);
		else if (!checkbox->IsChecked() && find == activeResourceTypeFilters.End())
			activeResourceTypeFilters.Push(resourceType);

		if (ignoreRefreshBrowserResults == false)
			RefreshBrowserResults();
	}

	void ResourceBrowser::HandleHideResourceFilterWindow(StringHash eventType, VariantMap& eventData)
	{
		HideResourceFilterWindow();
	}

	void ResourceBrowser::HandleResourceTypeFilterToggleAllTypesToggled(StringHash eventType, VariantMap& eventData)
	{
		using namespace Toggled;
		CheckBox* checkbox = dynamic_cast<CheckBox*>(eventData[P_ELEMENT].GetPtr());

		UIElement* filterHolder = browserFilterWindow->GetChild("TypeFilters", true);
		PODVector<UIElement*>  children;
		filterHolder->GetChildren(children, true);

		ignoreRefreshBrowserResults = true;
		for (unsigned i = 0; i < children.Size(); ++i)
		{
			CheckBox* filter = dynamic_cast<CheckBox*>(children[i]);
			if (filter != NULL)
				filter->SetChecked(checkbox->IsChecked());
		}
		ignoreRefreshBrowserResults = false;
		RefreshBrowserResults();
	}

	void ResourceBrowser::HandleResourceDirFilterToggleAllTypesToggled(StringHash eventType, VariantMap& eventData)
	{
		using namespace Toggled;
		CheckBox* checkbox = dynamic_cast<CheckBox*>(eventData[P_ELEMENT].GetPtr());

		UIElement* filterHolder = browserFilterWindow->GetChild("DirFilters", true);
		PODVector<UIElement*>  children;
		filterHolder->GetChildren(children, true);

		ignoreRefreshBrowserResults = true;
		for (unsigned i = 0; i < children.Size(); ++i)
		{
			CheckBox* filter = dynamic_cast<CheckBox*>(children[i]);
			if (filter != NULL)
				filter->SetChecked(checkbox->IsChecked());
		}
		ignoreRefreshBrowserResults = false;
		RebuildResourceDatabase();
	}

	void ResourceBrowser::HandleBrowserFileDragBegin(StringHash eventType, VariantMap& eventData)
	{
		using namespace DragBegin;
		UIElement* uiElement = (UIElement*)eventData[P_ELEMENT].GetPtr();
		browserDragFile = GetBrowserFileFromUIElement(uiElement);
	}

	void ResourceBrowser::HandleBrowserFileDragEnd(StringHash eventType, VariantMap& eventData)
	{
		if (browserDragFile == NULL)
			return;

		UIElement* element = ui_->GetElementAt(ui_->GetCursor()->GetScreenPosition());
		if (element != NULL)
			return;

		if (browserDragFile->resourceType == RESOURCE_TYPE_MATERIAL)
		{
			/// \todo
			// 			StaticModel* model = dynamic_cast<StaticModel*>(GetDrawableAtMousePostion());
			// 			if (model !is null)
			// 			{
			// 				AssignMaterial(model, browserDragFile.resourceKey);
			// 			}
		}

		browserDragFile = NULL;
		browserDragComponent = NULL;
		browserDragNode = NULL;
	}

	void ResourceBrowser::HandleFileChanged(StringHash eventType, VariantMap& eventData)
	{
		using namespace FileChanged;

		String filename = eventData[P_FILENAME].GetString();
		BrowserFile* file = GetBrowserFileFromPath(filename);

		if (file == NULL)
		{
			// TODO: new file logic when watchers are supported
			return;
		}
		else
		{
			file->FileChanged();
		}
	}

	void ResourceBrowser::RotateResourceBrowserPreview(StringHash eventType, VariantMap& eventData)
	{
		using namespace DragMove;

		int elemX = eventData[P_ELEMENTX].GetInt();
		int elemY = eventData[P_ELEMENTY].GetInt();

		if (resourceBrowserPreview->GetHeight() > 0 && resourceBrowserPreview->GetWidth() > 0)
		{
			float yaw = ((resourceBrowserPreview->GetHeight() / 2) - elemY) * (90.0f / resourceBrowserPreview->GetHeight());
			float pitch = ((resourceBrowserPreview->GetWidth() / 2) - elemX) * (90.0f / resourceBrowserPreview->GetWidth());

			resourcePreviewNode->SetRotation(resourcePreviewNode->GetRotation().Slerp(Quaternion(yaw, pitch, 0.0f), 0.1f));
			RefreshBrowserPreview();
		}
	}

	void ResourceBrowser::HandleBrowserFileClick(StringHash eventType, VariantMap& eventData)
	{
// 		using namespace ItemClicked;
// 
// 		if (eventData[P_BUTTON].GetInt() != MOUSEB_RIGHT)
// 			return;
// 
// 		UIElement* uiElement = dynamic_cast<UIElement*>(eventData[P_ITEM].GetPtr());
// 		BrowserFile* file = GetBrowserFileFromUIElement(uiElement);
// 
// 		if (file == NULL)
// 			return;
// 
// 		Vector<UIElement*> actions;
// 		if (file->resourceType == RESOURCE_TYPE_MATERIAL)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Edit", "HandleBrowserEditResource", file));
// 		}
// 		else if (file.resourceType == RESOURCE_TYPE_MODEL)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Instance Animated Model", "HandleBrowserInstantiateAnimatedModel", file));
// 			actions.Push(CreateBrowserFileActionMenu("Instance Static Model", "HandleBrowserInstantiateStaticModel", file));
// 		}
// 		else if (file.resourceType == RESOURCE_TYPE_PREFAB)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Instance Prefab", "HandleBrowserInstantiatePrefab", file));
// 			actions.Push(CreateBrowserFileActionMenu("Instance in Spawner", "HandleBrowserInstantiateInSpawnEditor", file));
// 		}
// 		else if (file.fileType == EXTENSION_TYPE_OBJ ||
// 			file.fileType == EXTENSION_TYPE_COLLADA ||
// 			file.fileType == EXTENSION_TYPE_FBX ||
// 			file.fileType == EXTENSION_TYPE_BLEND)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Import Model", "HandleBrowserImportModel", file));
// 			actions.Push(CreateBrowserFileActionMenu("Import Scene", "HandleBrowserImportScene", file));
// 		}
// 		else if (file.resourceType == RESOURCE_TYPE_UIELEMENT)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Open UI Layout", "HandleBrowserOpenUILayout", file));
// 		}
// 		else if (file.resourceType == RESOURCE_TYPE_SCENE)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Load Scene", "HandleBrowserLoadScene", file));
// 		}
// 		else if (file.resourceType == RESOURCE_TYPE_SCRIPTFILE)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Execute Script", "HandleBrowserRunScript", file));
// 		}
// 		else if (file.resourceType == RESOURCE_TYPE_PARTICLEEFFECT)
// 		{
// 			actions.Push(CreateBrowserFileActionMenu("Edit", "HandleBrowserEditResource", file));
// 		}
// 
// 		actions.Push(CreateBrowserFileActionMenu("Open", "HandleBrowserOpenResource", file));
// 
// 		ActivateContextMenu(actions);
	}

	BrowserDir* ResourceBrowser::GetBrowserDir(String path)
	{
		HashMap<String, BrowserDir*>::Iterator it = browserDirs.Find(path);
		if (it == browserDirs.End())
			return NULL;
		return it->second_;
	}

	BrowserDir* ResourceBrowser::InitBrowserDir(String path)
	{
		BrowserDir* browserDir = NULL;

		HashMap<String, BrowserDir*>::Iterator it = browserDirs.Find(path);
		if (it != browserDirs.End())
			return it->second_;

		Vector<String> parts = path.Split('/');
		Vector<String> finishedParts;
		if (parts.Size() > 0)
		{
			BrowserDir* parent = rootDir;
			for (unsigned int i = 0; i < parts.Size(); ++i)
			{
				finishedParts.Push(parts[i]);
				String currentPath = Join(finishedParts, "/");
				browserDir = GetBrowserDir(currentPath);
				if (browserDir == NULL)
				{
					browserDir = new BrowserDir(currentPath, cache_, this);
					browserDirs[currentPath] = browserDir;
					parent->children.Push(browserDir);
				}
				parent = browserDir;
	
			}
			return browserDir;
		}
		return NULL;
	}

	BrowserFile* ResourceBrowser::GetBrowserFileFromId(unsigned id)
	{
		if (id == 0)
			return NULL;

		BrowserFile* file;
		for (unsigned i = 0; i < browserFiles.Size(); ++i)
		{
			file = browserFiles[i];
			if (file->id == id) return file;
		}
		return NULL;
	}

	BrowserFile* ResourceBrowser::GetBrowserFileFromUIElement(UIElement* element)
	{
		if (element == NULL || !element->GetVars().Contains(TEXT_VAR_FILE_ID))
			return NULL;
		return GetBrowserFileFromId(element->GetVar(TEXT_VAR_FILE_ID).GetUInt());
	}

	BrowserFile* ResourceBrowser::GetBrowserFileFromPath(String path)
	{
		for (unsigned i = 0; i < browserFiles.Size(); ++i)
		{
			BrowserFile* file = browserFiles[i];
			if (path == file->GetFullPath())
				return file;
		}
		return NULL;
	}

	void ResourceBrowser::CreateResourceBrowserUI()
	{
		browserWindow = ui_->LoadLayout(cache_->GetResource<XMLFile>("UI/IDEResourceBrowser.xml"));
		browserDirList = (ListView*)browserWindow->GetChild("DirectoryList", true);
		browserFileList = (ListView*)browserWindow->GetChild("FileList", true);
		browserSearch = (LineEdit*)browserWindow->GetChild("Search", true);
		browserStatusMessage = (Text*)browserWindow->GetChild("StatusMessage", true);
		browserResultsMessage = (Text*)browserWindow->GetChild("ResultsMessage", true);
		// browserWindow.visible = false;
		//browserWindow.opacity = uiMaxOpacity;

		browserFilterWindow = DynamicCast<Window>(ui_->LoadLayout(cache_->GetResource<XMLFile>("UI/EditorResourceFilterWindow.xml")));
		CreateResourceFilterUI();
		HideResourceFilterWindow();

		int height = Min(ui_->GetRoot()->GetHeight() / 4, 300);
		//browserWindow->SetSize(900, height);
		//browserWindow->SetPosition(35, ui_->GetRoot()->GetHeight() - height - 25);

		/// \todo
		//CloseContextMenu();
	//	ui_->GetRoot()->AddChild(browserWindow);
		ui_->GetRoot()->AddChild(browserFilterWindow);

		EditorView* editorView = GetSubsystem<EditorView>();
		if (editorView)
			editorView->GetLeftFrame()->AddTab("Resources", browserWindow);

		SubscribeToEvent(browserWindow->GetChild("CloseButton", true), E_RELEASED, URHO3D_HANDLER(ResourceBrowser, HideResourceBrowserWindow));
		SubscribeToEvent(browserWindow->GetChild("RescanButton", true), E_RELEASED, URHO3D_HANDLER(ResourceBrowser, HandleRescanResourceBrowserClick));
		SubscribeToEvent(browserWindow->GetChild("FilterButton", true), E_RELEASED, URHO3D_HANDLER(ResourceBrowser, ToggleResourceFilterWindow));
		SubscribeToEvent(browserDirList, E_SELECTIONCHANGED, URHO3D_HANDLER(ResourceBrowser, HandleResourceBrowserDirListSelectionChange));
		SubscribeToEvent(browserSearch, E_TEXTCHANGED, URHO3D_HANDLER(ResourceBrowser, HandleResourceBrowserSearchTextChange));
		SubscribeToEvent(browserFileList, E_ITEMCLICKED, URHO3D_HANDLER(ResourceBrowser, HandleBrowserFileClick));
		SubscribeToEvent(browserFileList, E_SELECTIONCHANGED, URHO3D_HANDLER(ResourceBrowser, HandleResourceBrowserFileListSelectionChange));
		SubscribeToEvent(cache_, E_FILECHANGED, URHO3D_HANDLER(ResourceBrowser, HandleFileChanged));
	}

	void ResourceBrowser::InitResourceBrowserPreview()
	{
		resourcePreviewScene = new  Scene(context_);
		resourcePreviewScene->SetName("PreviewScene");

		resourcePreviewScene->CreateComponent<Octree>();
		PhysicsWorld* physicsWorld = resourcePreviewScene->CreateComponent<PhysicsWorld>();
		physicsWorld->SetEnabled(false);
		physicsWorld->SetGravity(Vector3(0.0f, 0.0f, 0.0f));

		Node* zoneNode = resourcePreviewScene->CreateChild("Zone");
		Zone* zone = zoneNode->CreateComponent<Zone>();
		zone->SetBoundingBox(BoundingBox(-1000, 1000));
		zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
		zone->SetFogColor(Color(0.0f, 0.0f, 0.0f));
		zone->SetFogStart(10.0f);
		zone->SetFogEnd(100.0f);

		resourcePreviewCameraNode = resourcePreviewScene->CreateChild("PreviewCamera");
		resourcePreviewCameraNode->SetPosition(Vector3(0.0f, 0.0f, -1.5f));
		Camera* camera = resourcePreviewCameraNode->CreateComponent<Camera>();
		camera->SetNearClip(0.1f);
		camera->SetFarClip(100.0f);

		resourcePreviewLightNode = resourcePreviewScene->CreateChild("PreviewLight");
		resourcePreviewLightNode->SetDirection(Vector3(0.5f, -0.5f, 0.5f));
		resourcePreviewLight = resourcePreviewLightNode->CreateComponent<Light>();
		resourcePreviewLight->SetLightType(LIGHT_DIRECTIONAL);
		resourcePreviewLight->SetSpecularIntensity(0.5f);

		resourceBrowserPreview = (View3D*)browserWindow->GetChild("ResourceBrowserPreview", true);
		resourceBrowserPreview->SetFixedHeight(200);
		resourceBrowserPreview->SetFixedWidth(266);
		resourceBrowserPreview->SetView(resourcePreviewScene, camera);
		resourceBrowserPreview->SetAutoUpdate(false);

		resourcePreviewNode = resourcePreviewScene->CreateChild("PreviewNodeContainer");

		SubscribeToEvent(resourceBrowserPreview, E_DRAGMOVE, URHO3D_HANDLER(ResourceBrowser, RotateResourceBrowserPreview));

		RefreshBrowserPreview();
	}

	void ResourceBrowser::InitializeBrowserFileListRow(Text* fileText, BrowserFile* file)
	{
		fileText->RemoveAllChildren();
		VariantMap params = VariantMap();
		fileText->SetVar(TEXT_VAR_FILE_ID, file->id);
		fileText->SetVar(TEXT_VAR_RESOURCE_TYPE, file->resourceType);
		if (file->resourceType > 0)
			fileText->SetDragDropMode(DD_SOURCE);

		{
			Text* text = new Text(context_);
			fileText->AddChild(text);
			text->SetStyle("FileSelectorListText");
			text->SetText(file->fullname);
			text->SetName(String(file->resourceKey));
		}

	{
		Text* text = new Text(context_);
		fileText->AddChild(text);
		text->SetStyle("FileSelectorListText");
		text->SetText(file->ResourceTypeName());
	}

	if (file->resourceType == RESOURCE_TYPE_MATERIAL ||
		file->resourceType == RESOURCE_TYPE_MODEL ||
		file->resourceType == RESOURCE_TYPE_PARTICLEEFFECT ||
		file->resourceType == RESOURCE_TYPE_PREFAB
		)
	{
		SubscribeToEvent(fileText, E_DRAGBEGIN, URHO3D_HANDLER(ResourceBrowser, HandleBrowserFileDragBegin));
		SubscribeToEvent(fileText, E_DRAGEND, URHO3D_HANDLER(ResourceBrowser, HandleBrowserFileDragEnd));
	}
	}

	void ResourceBrowser::RebuildResourceDatabase()
	{
		if (browserWindow.Null())
			return;

		String newResourceDirsCache = Join(cache_->GetResourceDirs(), ";");
		ScanResourceDirectories();
		if (newResourceDirsCache != resourceDirsCache)
		{
			resourceDirsCache = newResourceDirsCache;
			PopulateResourceDirFilters();
		}
		PopulateBrowserDirectories();
		PopulateResourceBrowserFilesByDirectory(rootDir);
	}

	void ResourceBrowser::RefreshBrowserResults()
	{
		if (browserSearch->GetText().Empty())
		{
			browserDirList->SetVisible(true);
			PopulateResourceBrowserFilesByDirectory(selectedBrowserDirectory);
		}
		else
		{
			browserDirList->SetVisible(false);
			PopulateResourceBrowserBySearch();
		}
	}

	void ResourceBrowser::CreateResourceFilterUI()
	{
		UIElement* options = browserFilterWindow->GetChild("TypeOptions", true);
		CheckBox* toggleAllTypes = (CheckBox*)browserFilterWindow->GetChild("ToggleAllTypes", true);
		CheckBox* toggleAllResourceDirs = (CheckBox*)browserFilterWindow->GetChild("ToggleAllResourceDirs", true);

		SubscribeToEvent(toggleAllTypes, E_TOGGLED, URHO3D_HANDLER(ResourceBrowser, HandleResourceTypeFilterToggleAllTypesToggled));
		SubscribeToEvent(toggleAllResourceDirs, E_TOGGLED, URHO3D_HANDLER(ResourceBrowser, HandleResourceDirFilterToggleAllTypesToggled));
		SubscribeToEvent(browserFilterWindow->GetChild("CloseButton", true), E_RELEASED, URHO3D_HANDLER(ResourceBrowser, HandleHideResourceFilterWindow));

		int columns = 2;
		UIElement* col1 = browserFilterWindow->GetChild("TypeFilterColumn1", true);
		UIElement* col2 = browserFilterWindow->GetChild("TypeFilterColumn2", true);

		// use array to get sort of items
		Vector<ResourceType> sorted;// akj
		for (int i = RESOURCE_TYPE_SCENE; i < NUM_RESOURCE_TYPES; ++i)
			sorted.Push(ResourceType(i, Res::ResourceTypeName(i)));
		/// \todo
		// 2 unknown types are reserved for the top, the rest are alphabetized
		//sorted.Sort();
//		Sort(sorted.Begin(), sorted.End(), ResourceTypeopCmp);
		sorted.Insert(0, ResourceType(RESOURCE_TYPE_UNKNOWN, Res::ResourceTypeName(RESOURCE_TYPE_UNKNOWN)));
		sorted.Insert(0, ResourceType(RESOURCE_TYPE_UNUSABLE, Res::ResourceTypeName(RESOURCE_TYPE_UNUSABLE)));
		int halfColumns = (int)ceil(float(sorted.Size()) / float(columns));

		for (unsigned int i = 0; i < sorted.Size(); ++i)
		{
			ResourceType& type = sorted[i];
			UIElement* resourceTypeHolder = new UIElement(context_);
			if (i < (unsigned)halfColumns)
				col1->AddChild(resourceTypeHolder);
			else
				col2->AddChild(resourceTypeHolder);

			resourceTypeHolder->SetLayoutMode(LM_HORIZONTAL);
			resourceTypeHolder->SetLayoutSpacing(4);
			Text* label = new Text(context_);
			label->SetStyle("EditorAttributeText");
			label->SetText(type.name);
			CheckBox* checkbox = new CheckBox(context_);
			checkbox->SetName(String(type.id));
			checkbox->SetStyleAuto();
			checkbox->SetVar(TEXT_VAR_RESOURCE_TYPE, i);
			checkbox->SetChecked(true);
			SubscribeToEvent(checkbox, E_TOGGLED, URHO3D_HANDLER(ResourceBrowser, HandleResourceTypeFilterToggled));

			resourceTypeHolder->AddChild(checkbox);
			resourceTypeHolder->AddChild(label);
		}
	}

	void ResourceBrowser::CreateDirList(BrowserDir* dir, UIElement* parentUI /*= null*/)
	{
		Text* dirText = new Text(context_);
		browserDirList->InsertItem(browserDirList->GetNumItems(), dirText, parentUI);
		dirText->SetStyle("FileSelectorListText");
		dirText->SetText(dir->resourceKey.Empty() ? "Root" : dir->name);
		dirText->SetName(dir->resourceKey);
		dirText->SetVar(TEXT_VAR_DIR_ID, dir->resourceKey);

		// Sort directories alphetically
		browserSearchSortMode = BROWSER_SORT_MODE_ALPHA;
		/// \todo
		//dir->children.Sort();
		Sort(dir->children.Begin(), dir->children.End(), BrowserDiropCmp);

		for (unsigned i = 0; i < dir->children.Size(); ++i)
			CreateDirList(dir->children[i], dirText);
	}

	void ResourceBrowser::CreateFileList(BrowserFile* file)
	{
		Text* fileText = new Text(context_);
		fileText->SetStyle("FileSelectorListText");
		fileText->SetLayoutMode(LM_HORIZONTAL);
		browserFileList->InsertItem(browserFileList->GetNumItems(), fileText);
		file->browserFileListRow = fileText;
		InitializeBrowserFileListRow(fileText, file);
	}

	void ResourceBrowser::CreateResourcePreview(String path, Node* previewNode)
	{
		resourceBrowserPreview->SetAutoUpdate(false);
		int resourceType = Res::GetResourceType(context_,path);
		if (resourceType > 0)
		{
			File file(context_);
			file.Open(path);

			if (resourceType == RESOURCE_TYPE_MODEL)
			{
				Model* model = new Model(context_);
				if (model->Load(file))
				{
					StaticModel* staticModel = previewNode->CreateComponent<StaticModel>();
					staticModel->SetModel(model);
					return;
				}
			}
			else if (resourceType == RESOURCE_TYPE_MATERIAL)
			{
				Material* material = new Material(context_);
				/// \todo i get an error if i dont cast it to Resource ... :/
				if (((Resource*)material)->Load(file))
				{
					StaticModel* staticModel = previewNode->CreateComponent<StaticModel>();
					staticModel->SetModel(cache_->GetResource<Model>("Models/Sphere.mdl"));
					staticModel->SetMaterial(material);
					return;
				}
			}
			else if (resourceType == RESOURCE_TYPE_IMAGE)
			{
				SharedPtr<Image> image(new Image(context_));
				if (image->Load(file))
				{
					StaticModel* staticModel = previewNode->CreateComponent<StaticModel>();
					staticModel->SetModel(cache_->GetResource<Model>("Models/Editor/ImagePlane.mdl"));
					Material* material = cache_->GetResource<Material>("Materials/Editor/TexturedUnlit.xml");
					SharedPtr<Texture2D> texture(new Texture2D(context_));
					texture->SetData(image, true);
					material->SetTexture(TU_DIFFUSE, texture);
					staticModel->SetMaterial(material);
					return;
				}
			}
			else if (resourceType == RESOURCE_TYPE_PREFAB)
			{
				bool loaded = false;
				if (GetExtension(path) == ".xml")
				{
					XMLFile xmlFile(context_);
					if (xmlFile.Load(file))
						loaded = previewNode->LoadXML(xmlFile.GetRoot(), true);
				}
				else
				{
					loaded = previewNode->Load(file, true);
				}

				PODVector<StaticModel*> statDest;
				previewNode->GetComponents<StaticModel>(statDest, true);
				PODVector<AnimatedModel*> animDest;
				previewNode->GetComponents<AnimatedModel>(animDest, true);

				if (loaded && (statDest.Size() > 0 || animDest.Size() > 0))
					return;

				previewNode->RemoveAllChildren();
				previewNode->RemoveAllComponents();
			}
			else if (resourceType == RESOURCE_TYPE_PARTICLEEFFECT)
			{
				SharedPtr<ParticleEffect> particleEffect(new ParticleEffect(context_));
				/// \todo i get an error if i dont cast it to Resource ... :/
				if (((Resource*)particleEffect)->Load(file))
				{
					ParticleEmitter* particleEmitter = previewNode->CreateComponent<ParticleEmitter>();
					particleEmitter->SetEffect(particleEffect);
					particleEffect->SetActiveTime(0.0f);
					particleEmitter->Reset();
					resourceBrowserPreview->SetAutoUpdate(true);
					return;
				}
			}
		}

		StaticModel* staticModel = previewNode->CreateComponent<StaticModel>();
		staticModel->SetModel(cache_->GetResource<Model>("Models/Editor/ImagePlane.mdl"));
		Material* material = cache_->GetResource<Material>("Materials/Editor/TexturedUnlit.xml");
		SharedPtr<Texture2D> texture(new Texture2D(context_));
		SharedPtr<Image> noPreviewImage(cache_->GetResource<Image>("Textures/Editor/NoPreviewAvailable.png"));
		texture->SetData(noPreviewImage, false);
		material->SetTexture(TU_DIFFUSE, texture);
		staticModel->SetMaterial(material);

		return;
	}

	void ResourceBrowser::PopulateResourceDirFilters()
	{
		UIElement* resourceDirs = browserFilterWindow->GetChild("DirFilters", true);
		resourceDirs->RemoveAllChildren();
		activeResourceDirFilters.Clear();
		for (unsigned i = 0; i < cache_->GetResourceDirs().Size(); ++i)
		{
			UIElement* resourceDirHolder = new UIElement(context_);
			resourceDirs->AddChild(resourceDirHolder);
			resourceDirHolder->SetLayoutMode(LM_HORIZONTAL);
			resourceDirHolder->SetLayoutSpacing(4);
			resourceDirHolder->SetFixedHeight(16);

			Text* label = new Text(context_);
			label->SetStyle("EditorAttributeText");

			Vector<String> splits = cache_->GetResourceDirs()[i].Split('/');
			String text = splits[splits.Size() - 1];
			text = text + String("/");
			label->SetText(text);

			CheckBox* checkbox = new CheckBox(context_);
			checkbox->SetName(String(i));
			checkbox->SetStyleAuto();
			checkbox->SetVar(TEXT_VAR_RESOURCE_DIR_ID, i);
			checkbox->SetChecked(true);
			SubscribeToEvent(checkbox, E_TOGGLED, URHO3D_HANDLER(ResourceBrowser, HandleResourceDirFilterToggled));

			resourceDirHolder->AddChild(checkbox);
			resourceDirHolder->AddChild(label);
		}
	}

	void ResourceBrowser::PopulateBrowserDirectories()
	{
		browserDirList->RemoveAllItems();
		CreateDirList(rootDir);
		browserDirList->SetSelection(0);
	}

	void ResourceBrowser::PopulateResourceBrowserFilesByDirectory(BrowserDir* dir)
	{
		selectedBrowserDirectory = dir;
		browserFileList->RemoveAllItems();
		if (dir == NULL)
			return;

		Vector<BrowserFile*> files;
		for (unsigned x = 0; x < dir->files.Size(); x++)
		{
			BrowserFile* file = dir->files[x];

			if (activeResourceTypeFilters.Find(file->resourceType) == activeResourceTypeFilters.End())
				files.Push(file);
		}

		// Sort alphetically
		browserSearchSortMode = BROWSER_SORT_MODE_ALPHA;
		/// \todo
		Sort(files.Begin(), files.End(), BrowserFileopCmp);
	
		PopulateResourceBrowserResults(files);
		browserResultsMessage->SetText("Showing " + String(files.Size()) + " files");
	}

	void ResourceBrowser::PopulateResourceBrowserResults(Vector<BrowserFile*>& files)
	{
		browserFileList->RemoveAllItems();
		for (unsigned i = 0; i < files.Size(); ++i)
			CreateFileList(files[i]);
	}

	void ResourceBrowser::PopulateResourceBrowserBySearch()
	{
		String query = browserSearch->GetText();

		Vector<int> scores;
		Vector<BrowserFile*> scored;
		Vector<BrowserFile*> filtered;
		{
			BrowserFile* file = NULL;
			for (unsigned x = 0; x < browserFiles.Size(); x++)
			{
				file = browserFiles[x];
				file->sortScore = -1;

				if (activeResourceTypeFilters.Find(file->resourceType) != activeResourceTypeFilters.End())
					continue;

				if (activeResourceDirFilters.Find(file->resourceSourceIndex) != activeResourceDirFilters.End())
					continue;

				int find = file->fullname.Find(query, 0, false);
				if (find > -1)
				{
					int fudge = query.Length() - file->fullname.Length();
					int score = find * int(Abs(fudge * 2)) + int(Abs(fudge));
					file->sortScore = score;
					scored.Push(file);
					scores.Push(score);
				}
			}
		}

		// cut this down for a faster sort
		if (scored.Size() > BROWSER_SEARCH_LIMIT)
		{
			/// \todo
			//scores.Sort();
			Sort(scores.Begin(), scores.End());
			int scoreThreshold = scores[BROWSER_SEARCH_LIMIT];
			BrowserFile* file;
			for (unsigned x = 0; x < scored.Size(); x++)
			{
				file = scored[x];
				if (file->sortScore <= scoreThreshold)
					filtered.Push(file);
			}
		}
		else
			filtered = scored;

		browserSearchSortMode = BROWSER_SORT_MODE_ALPHA;
		/// \todo
		//filtered.Sort();
		if (browserSearchSortMode == 1)	
			Sort(filtered.Begin(), filtered.End(), BrowserFileopCmp);
		else
			Sort(filtered.Begin(), filtered.End(), BrowserFileopCmpScore);

		PopulateResourceBrowserResults(filtered);
		browserResultsMessage->SetText("Showing top " + String(filtered.Size()) + " of " + String(scored.Size()) + " results");
	}
}