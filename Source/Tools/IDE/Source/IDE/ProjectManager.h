/*!
 * \file ProjectManager.h
 *
 * \author vitali
 * \date Februar 2015
 *
 *
 */

#pragma once

#include <Urho3d\Core\Object.h>
#include <Urho3d\Scene\Serializable.h>

class Context;

namespace Urho3D
{
	class UIElement;
	class DirSelector;
	class UI;
	class Graphics;
	class ResourceCache;
	class IDESettings;
	class TemplateManager;
	class Text;
	class AttributeContainer;

	const StringHash PROJECTINDEX_VAR("ProjectIndex");

	/// ProjectManager Open Project.
	URHO3D_EVENT(E_OPENPROJECT, OpenProject)
	{
	}

	class ProjectSettings : public Serializable
	{
		URHO3D_OBJECT(ProjectSettings, Serializable);
	public:
		ProjectSettings(Context* context);
		virtual ~ProjectSettings();
		static void RegisterObject(Context* context);
		virtual bool	SaveDefaultAttributes() const { return true; }
		void CopyAttr(ProjectSettings* proj);

		/// attributes
		String name_;
		String icon_;
		String resFolders_;
		String mainScript_;
		String mainScene_;

		/// intern
		String path_;
	};

	class ProjectManager : public Object
	{
		URHO3D_OBJECT(ProjectManager, Object);
	public:
		ProjectManager(Context* context);
		virtual ~ProjectManager();
		static void RegisterObject(Context* context);

		UIElement*	CreateWelcomeScreen();
		void		ShowWelcomeScreen(bool value);
	
		void SetProjectRootFolder(const String& path);
		
		bool ChooseRootDir();
		bool OpenProject(const String& dir);
		bool OpenProject(const ProjectSettings* proj);

		void UpdateProjects(const String& dir);

		const String&		GetRootDir();
		ProjectSettings*	GetProject();

		bool SelectProject(const String& dir, const String& projectName);

	protected:
		void HandleNewProject(StringHash eventType, VariantMap& eventData);
		void HandleNewProjectAck(StringHash eventType, VariantMap& eventData);
		void HandleOpenProject(StringHash eventType, VariantMap& eventData);
		void HandleChangeRootDir(StringHash eventType, VariantMap& eventData);
		void HandleRootSelected(StringHash eventType, VariantMap& eventData);
		void HandleRescanProjects(StringHash eventType, VariantMap& eventData);
		
		void HandleOnTemplateClick(StringHash eventType, VariantMap& eventData);

		void CreateDirSelector(const String& title, const String& ok, const String& cancel, const String& initialPath);
		void CloseDirSelector(String& path);
		void CloseDirSelector();

		void HandleProjectListClick(StringHash eventType, VariantMap& eventData);

		void NewProject();

		SharedPtr<UIElement> welcomeUI_;
		DirSelector* dirSelector_;
	
		SharedPtr<ProjectSettings> newProject_;
		SharedPtr<ProjectSettings> selectedProject_;

		Vector<SharedPtr<ProjectSettings>> projects_;

		SharedPtr<TemplateManager> templateManager_;
		SharedPtr<Text> templateSlectedText_;
		ProjectSettings* selectedtemplate_;
		SharedPtr<AttributeContainer> newProjectAttrCont_;

		///cached subsystems
		ResourceCache* cache_;
		UI* ui_;
		Graphics* graphics_;
		IDESettings* settings_;
	};
}
