
#include <Urho3D/Urho3D.h>
#include "EPScene2D.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Window.h>
#include "EditorData.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/IO/FileSystem.h>
#include "EditorView.h"
#include "EditorSelection.h"
#include "Editor.h"
#include <Urho3D/Urho2D/Drawable2D.h>

namespace Urho3D
{
	extern const char* URHO2D_CATEGORY;


	EPScene2D::EPScene2D(Context* context) : EditorPlugin(context)
	{

	}

	EPScene2D::~EPScene2D()
	{

	}

	bool EPScene2D::HasMainScreen()
	{
		return true;
	}

	Urho3D::String EPScene2D::GetName() const
	{
		return String("2DView");
	}

	void EPScene2D::Edit(Object *object)
	{

	}

	bool EPScene2D::Handles(Object *object) const
	{
		/// can handle 2d Drawables 
		Drawable2D* drawable = dynamic_cast<Drawable2D*>(object);
		if (drawable)
			return true;
		/// also check if the component is in the Urho2d category
		const HashMap<String, Vector<StringHash> >& categories = context_->GetObjectCategories();
		HashMap<String, Vector<StringHash> >::ConstIterator it = categories.Find(URHO2D_CATEGORY);
		if (it != categories.End())
		{
			Vector<StringHash>::ConstIterator it2 = it->second_.Find(object->GetType());
			if (it2 != it->second_.End())
			{
				return true;
			}
		}
			
		return false;
	}

	UIElement* EPScene2D::GetMainScreen()
	{
		if (!window_)
		{
			EditorData* editorData_ = GetSubsystem<EditorData>();
			window_ = new Window(context_);
			window_->SetFixedSize(200, 200);
			window_->SetStyleAuto(editorData_->GetDefaultStyle());
			Text* hello = window_->CreateChild<Text>();
			hello->SetText("hello 2d view ... ");
			hello->SetStyleAuto(editorData_->GetDefaultStyle());
		}
		return window_;
	}

	void EPScene2D::SetVisible(bool visible)
	{
		visible_ = visible;
	}

	EPScene2DEditor::EPScene2DEditor(Context* context) : Object(context)
	{
		ui_ = GetSubsystem<UI>();
		input_ = GetSubsystem<Input>();
		cache_ = GetSubsystem<ResourceCache>();
		renderer = GetSubsystem<Renderer>();
		fileSystem_ = GetSubsystem<FileSystem>();

		editorView_ = GetSubsystem<EditorView>();
		editorData_ = GetSubsystem<EditorData>();
		editorSelection_ = GetSubsystem<EditorSelection>();
		if (editorData_)
			editor_ = editorData_->GetEditor();
	}

	EPScene2DEditor::~EPScene2DEditor()
	{

	}

	void EPScene2DEditor::Edit(Object *object)
	{
		/// can handle 2d Drawables 
		Drawable2D* drawable = dynamic_cast<Drawable2D*>(object);
		if (drawable)
		{

		}
		
		/// also check if the component is in the Urho2d category
		const HashMap<String, Vector<StringHash> >& categories = context_->GetObjectCategories();
		HashMap<String, Vector<StringHash> >::ConstIterator it = categories.Find(URHO2D_CATEGORY);
		if (it != categories.End())
		{
			Vector<StringHash>::ConstIterator it2 = it->second_.Find(object->GetType());
			if (it2 != it->second_.End())
			{
				
			}
		}
	}

	void EPScene2DEditor::Show()
	{

	}

	void EPScene2DEditor::Hide()
	{

	}

}