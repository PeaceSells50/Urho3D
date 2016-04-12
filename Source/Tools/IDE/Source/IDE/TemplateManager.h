#pragma once

#include <Urho3d/Core/Object.h>

namespace Urho3D
{
	class ProjectSettings;
	class Text;

	class UIElement;

	class TemplateManager : public Object
	{
		URHO3D_OBJECT(TemplateManager, Object);
	public:
		/// Construct.
		TemplateManager(Context* context);
		/// Destruct.
		virtual ~TemplateManager();
		/// Register object factory.
		static void RegisterObject(Context* context);

		bool LoadTemplates();

		ProjectSettings* GetSelectedTemplate();

		Vector<SharedPtr<ProjectSettings>>& GetTemplateProjects();

		UIElement* GetContainer();
		String GetTemplatesPath() { return templatesPath_; }
	protected:
		void HandleMouseClick(StringHash eventType, VariantMap& eventData);
		SharedPtr<Text> slectedText_;
		Vector<SharedPtr<ProjectSettings>> templateProjects_;
		ProjectSettings* selectedtemplate_;
		String templatesPath_;
	};
}

