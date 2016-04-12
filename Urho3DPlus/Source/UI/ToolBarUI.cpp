



#include <Urho3D/Urho3D.h>
#include <Urho3D/Core\Context.h>
#include <Urho3D/UI\BorderImage.h>
#include <Urho3D/UI\UI.h>
#include <Urho3D/UI\Menu.h>
#include <Urho3D/Math\Rect.h>
#include <Urho3D/UI\Text.h>
#include <Urho3D/UI\Window.h>
#include <Urho3D/Input\InputEvents.h>
#include <Urho3D/UI\UIEvents.h>
#include "ToolBarUI.h"
#include <Urho3D/UI\CheckBox.h>
#include <Urho3D/UI\ToolTip.h>
#include <Urho3D/UI\ScrollBar.h>

namespace Urho3D
{


	ToolBarUI* ToolBarUI::Create(UIElement* parent, const String& idname, XMLFile* iconStyle, const String& baseStyle, int width/*=0*/, int height /*= 21*/, XMLFile* defaultstyle /*= NULL*/)
	{
		ToolBarUI* toolbar = parent->CreateChild<ToolBarUI>(idname);
		//menubar->SetStyle("Window",styleFile);

		if (defaultstyle)
			toolbar->SetDefaultStyle(defaultstyle);
		toolbar->SetStyleAuto();
		if (width > 0)
			toolbar->SetFixedWidth(width);
		else
			toolbar->SetFixedWidth(parent->GetMinWidth());

		toolbar->SetFixedHeight(height);
		toolbar->iconStyle_ = iconStyle;
		toolbar->baseStyle_ = baseStyle;

		return toolbar;
	}

	void ToolBarUI::RegisterObject(Context* context)
	{
		context->RegisterFactory<ToolBarUI>();
		URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
		URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);

	}

	ToolBarUI::~ToolBarUI()
	{

	}

	ToolBarUI::ToolBarUI(Context* context) : BorderImage(context)
	{
		bringToFront_ = true;
		clipChildren_ = true;
		SetEnabled(true);
		SetLayout(LM_HORIZONTAL, 4, IntRect(8, 4, 4, 8));
		SetAlignment(HA_LEFT, VA_TOP);
// 		horizontalScrollBar_ = CreateChild<ScrollBar>("TB_HorizontalScrollBar");
// 		horizontalScrollBar_->SetInternal(true);
// 		horizontalScrollBar_->SetAlignment(HA_LEFT, VA_BOTTOM);
// 		horizontalScrollBar_->SetOrientation(O_HORIZONTAL);

	}

	UIElement* ToolBarUI::CreateGroup(const String& name, LayoutMode layoutmode)
	{
		UIElement* group = GetChild(name);
		if (group)
			return group;

		group = new UIElement(context_);
		group->SetName(name);
		group->SetDefaultStyle(GetDefaultStyle());
		group->SetLayoutMode(layoutmode);
		group->SetAlignment(HA_LEFT,VA_CENTER);
		AddChild(group);
		return group;
	}

	CheckBox* ToolBarUI::CreateToolBarToggle(const String& groupname, const String& title)
	{
		UIElement* group = GetChild(groupname);
		if (group)
		{
			CheckBox* toggle = new CheckBox(context_);
			toggle->SetName(title);
			toggle->SetDefaultStyle(GetDefaultStyle());
			toggle->SetStyle(baseStyle_);
			toggle->SetOpacity(0.7f);

			CreateToolBarIcon(toggle);
			CreateToolTip(toggle, title, IntVector2(toggle->GetWidth() + 10, toggle->GetHeight() - 10));

			group->AddChild(toggle);
			FinalizeGroupHorizontal(group, baseStyle_);
			
			return toggle;
		}
		return NULL;
	}

	CheckBox* ToolBarUI::CreateToolBarToggle(const String& title)
	{
		CheckBox* toggle = new CheckBox(context_);
		toggle->SetName(title);
		toggle->SetDefaultStyle(GetDefaultStyle());
		toggle->SetStyle(baseStyle_);
		toggle->SetOpacity(0.7f);
	
		CreateToolBarIcon(toggle);
		CreateToolTip(toggle, title, IntVector2(toggle->GetWidth() + 10, toggle->GetHeight() - 10));
		AddChild(toggle);

		return toggle;
	}

	UIElement* ToolBarUI::CreateToolBarIcon(UIElement* element)
	{
		BorderImage* icon = new BorderImage(context_);
		icon->SetName("Icon");
		icon->SetDefaultStyle(iconStyle_);
		icon->SetStyle(element->GetName());
		icon->SetFixedSize(GetHeight() - 11, GetHeight() - 11);
		icon->SetAlignment(HA_CENTER, VA_CENTER);
		element->AddChild(icon);
		return icon;
	}

	UIElement* ToolBarUI::CreateToolTip(UIElement* parent, const String& title, const IntVector2& offset)
	{
		ToolTip* toolTip = parent->CreateChild<ToolTip>("ToolTip");
		toolTip->SetPosition(offset);

		BorderImage* textHolder = toolTip->CreateChild<BorderImage>("BorderImage");
		textHolder->SetStyle("ToolTipBorderImage");

		Text* toolTipText = textHolder->CreateChild<Text>("Text");
		toolTipText->SetStyle("ToolTipText");
		toolTipText->SetText(title);

		return toolTip;
	}

	void ToolBarUI::FinalizeGroupHorizontal(UIElement* group, const String& baseStyle)
	{
		int width = 0;
		for (unsigned int i = 0; i < group->GetNumChildren(); ++i)
		{
			UIElement* child = group->GetChild(i);
			width += child->GetMinWidth();
			if (i == 0 && i < group->GetNumChildren() - 1)
				child->SetStyle(baseStyle + "GroupLeft");
			else if (i < group->GetNumChildren() - 1)
				child->SetStyle(baseStyle + "GroupMiddle");
			else
				child->SetStyle(baseStyle + "GroupRight");
			child->SetFixedSize(GetHeight() - 6, GetHeight() - 6);
		}
		/// \todo SetMaxSize(group->GetSize()) does not work !? 
		//group->SetMaxSize(group->GetSize());
		group->SetFixedWidth(width);
	}

	UIElement* ToolBarUI::CreateToolBarSpacer(int width)
	{
		UIElement* spacer = new UIElement(context_);
		spacer->SetFixedWidth(width);
		AddChild(spacer);
		return spacer;
	}







}
