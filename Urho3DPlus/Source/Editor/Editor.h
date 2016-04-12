#pragma once

#include <Urho3D/Core\Object.h>
#include <Urho3D/Container\Vector.h>
#include <Urho3D/Core\Variant.h>

namespace Urho3D
{
	class UIElement;
	class ResourceCache;
	class UI;
	class Graphics;
	class Scene;
	class View3D;
	class Window;
	class Node;
	class Component;
	class Resource;
	class XMLFile;
	class FileSelector;
	class FileSystem;

	class MenuBarUI;
	class MiniToolBarUI;
	class ToolBarUI;
	class HierarchyWindow;
	class EditorSelection;
	class AttributeInspector;
	class EditorData;
	class EditorView;
	class EditorPlugin;
	class ProjectSettings;
	class ResourceBrowser;
	class Window;

	const StringHash VAR_CONTEXT_MENU_HANDLER("ContextMenuHandler");

	class Editor : public Object
	{
		URHO3D_OBJECT(Editor, Object);
	public:
		Editor(Context* context);
		virtual ~Editor();
		static void RegisterObject(Context* context);
		/// create the Editor but dont load the plugins 
		bool Create(Scene* scene, UIElement* sceneUI);
		/// load plugins 
		void LoadPlugins();
		/// open the project
		void OpenProject(ProjectSettings * project);
		/// load scene 
		bool LoadScene(const String& fileName);


		/// adds the plugin to the editor data, if plugin has the main screen then add it to the middle frame tabs.
		void AddEditorPlugin(EditorPlugin* plugin);
		/// remove the plugin
		void RemoveEditorPlugin(EditorPlugin* plugin);
	
		/// Getters
		Scene*		GetScene();
		UIElement*	GetSceneUI() { return sceneRootUI_; }
		Component*	GetListComponent(UIElement*  item);
		Node*		GetListNode(UIElement*  item);
		UIElement*	GetListUIElement(UIElement*  item);
		UIElement*	GetUIElementByID(const Variant& id);
		HierarchyWindow*	GetHierarchyWindow() { return hierarchyWindow_; }
		AttributeInspector* GetAttributeWindow() { return attributeWindow_; }
		EditorSelection*	GetEditorSelection() { return editorSelection_; }
		EditorData*			GetEditorData() { return editorData_; }
		EditorView*			GetEditorView() { return editorView_; }

		/// Setters
		void SetScene(Scene* scene);
		void SetSceneUI(UIElement* sceneUI);

		// UI Stuff
		/// create/add a Menu Item to the Menu Bar
		bool AddToMenuBar(const String& menuTitle, const String& itemTitle, const StringHash& action, int accelKey, int accelQual, bool addToQuickMenu, String quickMenuText);
		/// create/add a Toggle to the Tool Bar
		bool AddToToolBar(const String& groupname, const String& title);
		/// create/add a Spacer to the Tool Bar
		void AddSpacerToToolBar(int width);

		void CreateFileSelector(const String& title, const String& ok, const String& cancel,
			const String& initialPath, Vector<String>& filters, unsigned int initialFilter);
		void			CloseFileSelector(unsigned int& filterIndex, String& path);
		void			CloseFileSelector();
		FileSelector*	GetUIFileSelector();

		ResourceBrowser* GetResourceBrowser() { return resourceBrowser_.Get(); }

		void CreateContextMenu();
		void OpenContextMenu(bool fromClick = true);
		void CloseContextMenu();
		void ActivateContextMenu(Vector<UIElement*> actions);
		UIElement* CreateContextMenuItem(String text, EventHandler* handler, String menuName = "", bool autoLocalize = true);
		void ContextMenuEventWrapper(StringHash eventType, VariantMap& eventData);
		SharedPtr<UIElement> LoadEditorUI(const String& fileName);
		SharedPtr<XMLFile> GetEditorUIXMLFile(const String& fileName);

		Window* GetContextMenu() { return contextMenu_; }
		bool IsContextMenuActionWaitFrame() { return contextMenuActionWaitFrame_; }
		void SetContextMenuActionWaitFrame(bool flag) { contextMenuActionWaitFrame_=flag; }

	protected: 
		void HandleUpdate(StringHash eventType, VariantMap& eventData);
		/// Handle Menu Bar Events
		void HandleMenuBarAction(StringHash eventType, VariantMap& eventData);
		/// Handle Events
		void HandleMainEditorTabChanged(StringHash eventType, VariantMap& eventData);
		/// handle Hierarchy Events
		void HandleHierarchyListSelectionChange(StringHash eventType, VariantMap& eventData);
		void HandleHierarchyListDoubleClick(StringHash eventType, VariantMap& eventData);
		
		void AddResourcePath(String newPath, bool usePreferredDir = true);

		Window* contextMenu_;
		bool contextMenuActionWaitFrame_;

		///cached subsystems
		ResourceCache*	cache_;
		UI*				ui_;
		Graphics*		graphics_;
		FileSystem*		fileSystem_;

		/// is the editor visible, used for the in game editor
		bool visible_;

		/// currently edited
		SharedPtr<Scene>		scene_;
		SharedPtr<UIElement>	sceneRootUI_;

		/// editor plugin handling
		Vector<EditorPlugin*>	mainEditorPlugins_;
		EditorPlugin*			editorPluginMain_;
		EditorPlugin*			editorPluginOver_;

		/// ui stuff
		SharedPtr<UIElement>	rootUI_;
		SharedPtr<EditorView>	editorView_;
		SharedPtr<FileSelector> uiFileSelector_;

		/// split editor functionality in different classes
		SharedPtr<EditorSelection>	editorSelection_;
		SharedPtr<EditorData>		editorData_;

		/// default IDE Editors
		SharedPtr<HierarchyWindow>		hierarchyWindow_;
		SharedPtr<AttributeInspector>	attributeWindow_;
		SharedPtr<ResourceBrowser>		resourceBrowser_;


	};
}