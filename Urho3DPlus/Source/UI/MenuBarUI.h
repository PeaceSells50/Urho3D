/*!
 * \file MenuBarUI.h
 *
 * \author vitali
 * \date Februar 2015
 *
 *
 */

#pragma once

#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/BorderImage.h>

namespace Urho3D
{

	/// Menu selected.
	URHO3D_EVENT(E_MENUBAR_ACTION, MenuBarAction)
	{
		URHO3D_PARAM(P_ACTION, Action);              // stringhash 
		URHO3D_PARAM(P_UINAME, UIName);              // string
	}

	class BorderImage;
	class XMLFile;
	class Menu;
	class Window;
	class Text;

	/// \todo use dirty masks
	class MenuBarUI : public BorderImage
	{
		URHO3D_OBJECT(MenuBarUI, BorderImage);
	public:
		MenuBarUI(Context* context);
		virtual ~MenuBarUI();
		static void RegisterObject(Context* context);
		static MenuBarUI* Create(UIElement* context, const String& idname, int width = 0, int height = 21, XMLFile* defaultstyle = NULL);

		Menu* CreateMenu(const String& title);
		Menu* CreateMenuItem(const String& menuTitle, const String& title,const StringHash& action = StringHash::ZERO, int accelKey = 0, int accelQual = 0, bool addToQuickMenu = true, String quickMenuText = "");
		Window* CreatePopupMenu(Menu* menu);
		Menu* CreatePopupMenuItem(Window* window, const String& title, const StringHash& action = StringHash::ZERO, int accelKey = 0, int accelQual = 0, bool addToQuickMenu = true, String quickMenuText = "");
	protected:
		void FinalizedPopupMenu(Window* popup);
		Text* CreateAccelKeyText(int accelKey, int accelQual);

		void HandleMenuSelected(StringHash eventType, VariantMap& eventData);
	private:
	};
}