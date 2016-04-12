#include "EditorView.h"
#include <Urho3d/Resource/ResourceCache.h>
#include <Urho3d/UI/UI.h>
#include <Urho3d/Graphics/Graphics.h>
#include <Urho3d/Input/InputEvents.h>
#include <Urho3d/UI/Window.h>
#include <Urho3d/UI/UIElement.h>
#include <Urho3d/UI/UIEvents.h>
#include <Urho3d/Scene/Scene.h>
#include <Urho3d/UI/View3D.h>
#include <Urho3d/Scene/Node.h>
#include <Urho3d/Graphics/Camera.h>
#include <Urho3d/Graphics/Zone.h>
#include <Urho3d/Math/BoundingBox.h>
#include <Urho3d/Math/Color.h>
#include <Urho3d/Math/Quaternion.h>
#include <Urho3d/Graphics/StaticModel.h>
#include <Urho3d/Graphics/Model.h>
#include <Urho3d/Graphics/Material.h>
#include <Urho3d/Graphics/Light.h>
#include <Urho3d/Graphics/Octree.h>
#include <Urho3d/UI/UIElement.h>
#include <Urho3d/Resource/Resource.h>
#include <Urho3d/Container/Str.h>
#include <Urho3d/UI/FileSelector.h>
#include <Urho3d/Resource/XMLFile.h>
#include <Urho3d/Core/CoreEvents.h>

#include "UI/HierarchyWindow.h"
#include "UI/AttributeInspector.h"
#include "UI/MenuBarUI.h"
#include "UI/ToolBarUI.h"
#include "UI/MiniToolBarUI.h"
#include "UI/UIUtils.h"
#include "TabWindow.h"
#include "EditorData.h"
#include "Editor/Editor.h"

namespace Urho3D
{
	EditorView::EditorView(Context* context, EditorData* data) : Object(context)
	{
		if (!data)
			return;

		cache_ = GetSubsystem<ResourceCache>();
		graphics_ = GetSubsystem<Graphics>();
		ui_ = GetSubsystem<UI>();

		XMLFile* styleFile = NULL;
		XMLFile* iconstyle = NULL;
		if (data)
		{
			styleFile = data->GetDefaultStyle();
			iconstyle = data->GetIconStyle();
		}
		else
		{
			styleFile = cache_->GetResource<XMLFile>("UI/IDEStyle.xml");
			iconstyle = cache_->GetResource<XMLFile>("UI/IDEIcons.xml");
		}

		uiRoot_ = data->GetEdiorRootUI();

		menubar_ = MenuBarUI::Create(uiRoot_, "EditorMenuBar");

		editorData_ = GetSubsystem<EditorData>();
		if (editorData_)
			editor_ = editorData_->GetEditor();

		int toolwidth = ui_->GetRoot()->GetWidth();
		toolbar_ = ToolBarUI::Create(uiRoot_, "EditorToolBar", iconstyle, "ToolBarToggle", toolwidth);

		int minitoolheight = ui_->GetRoot()->GetHeight() - toolbar_->GetMinHeight() - menubar_->GetMinHeight();

		UIElement* mainFrameUI_ = uiRoot_->CreateChild<UIElement>("MainFrame");
		mainFrameUI_->SetDefaultStyle(styleFile);
		mainFrameUI_->SetFixedSize(ui_->GetRoot()->GetWidth(), minitoolheight);

		miniToolbar_ = MiniToolBarUI::Create(mainFrameUI_, "EditorMiniToolBar", iconstyle, 28, minitoolheight, styleFile);
	
		leftFrame_ = mainFrameUI_->CreateChild<TabWindow>("LeftFrame");

		leftFrame_->SetStyle("TabWindow");
		leftFrame_->SetWidth(200);
		leftFrame_->SetFixedHeight(minitoolheight);
		leftFrame_->SetResizable(true);
		leftFrame_->SetResizeBorder(IntRect(6, 6, 6, 6));
		leftFrame_->SetLayoutBorder(IntRect(4, 4, 4, 4));
		leftFrame_->SetPosition(miniToolbar_->GetPosition() + IntVector2(miniToolbar_->GetWidth(), 0));

		middleFrame_ = mainFrameUI_->CreateChild<TabWindow>("MiddleFrame");
		middleFrame_->SetPosition(leftFrame_->GetPosition() + IntVector2(leftFrame_->GetWidth(), 0));
		middleFrame_->SetStyle("TabWindow");
		middleFrame_->SetHeight(minitoolheight);
		middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - 400);
		middleFrame_->SetLayoutBorder(IntRect(4, 4, 4, 4));

		rightFrame_ = mainFrameUI_->CreateChild<TabWindow>("LeftFrame");
		rightFrame_->SetPosition(middleFrame_->GetPosition() + IntVector2(middleFrame_->GetWidth(), 0));
		rightFrame_->SetStyle("TabWindow");
		rightFrame_->SetLayout(LM_VERTICAL);
		rightFrame_->SetWidth(200);
		rightFrame_->SetResizable(true);
		rightFrame_->SetFixedHeight(minitoolheight);
		rightFrame_->SetResizeBorder(IntRect(6, 6, 6, 6));
		rightFrame_->SetLayoutBorder(IntRect(4, 4, 4, 4));
		SubscribeToEvent(rightFrame_, E_RESIZED, URHO3D_HANDLER(EditorView, HandlePost));
		SubscribeToEvent(leftFrame_, E_RESIZED, URHO3D_HANDLER(EditorView, HandlePost));

		SubscribeToEvent(E_UIMOUSECLICK, URHO3D_HANDLER(EditorView, ViewMouseClick));
		SubscribeToEvent(E_UIMOUSECLICKEND, URHO3D_HANDLER(EditorView, ViewMouseClickEnd));

		minLeftFrameWidth_ = 100;
		minMiddleFrameWidth_ = 200; /// \todo use minMiddleFrameWidth_
		minRightFrameWidth_ = 100;
	}

	EditorView::~EditorView()
	{
	}

	void EditorView::RegisterObject(Context* context)
	{
		context->RegisterFactory<EditorView>();
	}

	void EditorView::HandlePost(StringHash eventType, VariantMap& eventData)
	{
		using namespace Resized;

		UIElement* element = (UIElement*)eventData[P_ELEMENT].GetPtr();
		int width = eventData[P_WIDTH].GetInt();
		int height = eventData[P_HEIGHT].GetInt();

		if (element == leftFrame_)
		{
			if (width < minLeftFrameWidth_)
			{
				leftFrame_->SetWidth(minLeftFrameWidth_);
			}
			middleFrame_->SetPosition(leftFrame_->GetPosition() + IntVector2(leftFrame_->GetWidth(), 0));
			if (rightFrame_->IsVisible())
			{
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
			}
			else
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - miniToolbar_->GetWidth());
		}
		else if (element == rightFrame_)
		{
			if (width < minRightFrameWidth_)
			{
				rightFrame_->SetWidth(minRightFrameWidth_);
			}
			if (leftFrame_->IsVisible())
			{
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
			}
			else
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());

			rightFrame_->SetPosition(middleFrame_->GetPosition() + IntVector2(middleFrame_->GetWidth(), 0));
		}
	}

	void EditorView::UpdateLayout()
	{
		int minitoolheight = ui_->GetRoot()->GetHeight() - menubar_->GetMinHeight();
		if (toolbar_->IsVisible())
			minitoolheight -= toolbar_->GetMinHeight();
		miniToolbar_->SetFixedHeight(minitoolheight);
		middleFrame_->SetFixedHeight(minitoolheight);
		if (leftFrame_->IsVisible())
		{
			if (miniToolbar_->IsVisible())
				leftFrame_->SetPosition(miniToolbar_->GetPosition() + IntVector2(miniToolbar_->GetWidth(), 0));
			else
				leftFrame_->SetPosition(IntVector2::ZERO);

			middleFrame_->SetPosition(leftFrame_->GetPosition() + IntVector2(leftFrame_->GetWidth(), 0));
			leftFrame_->SetFixedHeight(minitoolheight);
			if (rightFrame_->IsVisible())
			{
				rightFrame_->SetFixedHeight(minitoolheight);
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
				rightFrame_->SetPosition(middleFrame_->GetPosition() + IntVector2(middleFrame_->GetWidth(), 0));
			}
			else
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - leftFrame_->GetWidth() - miniToolbar_->GetWidth());
		}
		else
		{
			if (miniToolbar_->IsVisible())
				middleFrame_->SetPosition(miniToolbar_->GetPosition() + IntVector2(miniToolbar_->GetWidth(), 0));
			else
				middleFrame_->SetPosition(IntVector2::ZERO);

			if (rightFrame_->IsVisible())
			{
				rightFrame_->SetFixedHeight(minitoolheight);
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - rightFrame_->GetWidth() - miniToolbar_->GetWidth());
				rightFrame_->SetPosition(middleFrame_->GetPosition() + IntVector2(middleFrame_->GetWidth(), 0));
			}
			else
				middleFrame_->SetWidth(ui_->GetRoot()->GetWidth() - miniToolbar_->GetWidth());
		}
	}

	void EditorView::SetLeftFrameVisible(bool enable)
	{
		leftFrame_->SetVisible(enable);
		UpdateLayout();
	}

	bool EditorView::IsLeftFrameVisible() const
	{
		return leftFrame_->IsVisible();
	}

	void EditorView::SetRightFrameVisible(bool enable)
	{
		rightFrame_->SetVisible(enable);
		UpdateLayout();
	}

	bool EditorView::IsRightFrameVisible() const
	{
		return rightFrame_->IsVisible();
	}

	TabWindow* EditorView::GetLeftFrame()
	{
		return leftFrame_;
	}

	TabWindow* EditorView::GetRightFrame()
	{
		return rightFrame_;
	}

	TabWindow* EditorView::GetMiddleFrame()
	{
		return middleFrame_;
	}

	ToolBarUI* EditorView::GetToolBar()
	{
		return toolbar_;
	}

	MiniToolBarUI* EditorView::GetMiniToolBar()
	{
		return miniToolbar_;
	}

	MenuBarUI* EditorView::GetGetMenuBar()
	{
		return menubar_;
	}

	void EditorView::ViewMouseClick(StringHash eventType, VariantMap& eventData)
	{
		//ViewRaycast(true);
	}

	void EditorView::ViewMouseClickEnd(StringHash eventType, VariantMap& eventData)
	{
		// checks to close open popup windows
		IntVector2 pos = ui_->GetCursorPosition();
		if (editor_->GetContextMenu() && editor_->GetContextMenu()->IsEnabled())
		{
			if (editor_->IsContextMenuActionWaitFrame())
				editor_->SetContextMenuActionWaitFrame(false);
			else
			{
				if (!editor_->GetContextMenu()->IsInside(pos, true))
					editor_->CloseContextMenu();
			}
		}
//		if (quickMenu !is null && quickMenu.enabled)
//		{
//			bool enabled = quickMenu.IsInside(pos, true);
//			quickMenu.enabled = enabled;
//			quickMenu.visible = enabled;
//		}
	}
}