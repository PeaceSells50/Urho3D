//
// Copyright (c) 2008-2015 the Sviga project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Urho3d\Engine\Application.h>

namespace Urho3D
{
	class Text;
	class UIElement;
	class IDESettings;
	class ProjectManager;
	class ResourceCache;
	class UI;
	class Console;
	class DebugHud;
	class FileSelector;
	class Graphics;
	class DirSelector;
	class Editor;
	class View3D;
}

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

const float TOUCH_SENSITIVITY = 2.0f;

/// Sample class, as framework for all samples.
///    - Initialization of the Urho3D engine (in Application class)
///    - Modify engine parameters for windowed mode and to show the class name as title
///    - Create Urho3D logo at screen
///    - Set custom window title and icon
///    - Create Console and Debug HUD, and use F1 and F2 key to toggle them
///    - Toggle rendering options from the keys 1-8
///    - Take screenshot with key 9
///    - Handle Esc key down to hide Console or exit application
///    - Init touch input on mobile platform using screen joysticks (patched for each individual sample)

class IDE : public Application
{
	// Enable type information.
	URHO3D_OBJECT(IDE, Application);

public:
	/// Construct.
	IDE(Urho3D::Context* context);

	/// Setup before engine initialization. This is a chance to eg. modify the engine parameters. Call ErrorExit() to terminate without initializing the engine. Called by Application.
	virtual void Setup();
	/// Setup after engine initialization and before running the main loop. Call ErrorExit() to terminate without running the main loop. Called by Application.
	virtual void Start();
	/// Cleanup after the main loop. Called by Application.
	virtual void Stop();

	/// shows the project managers welcome screen
	void ShowWelcomeScreen();

	void Quit();

protected:
	void HandleQuitMessageAck(StringHash eventType, VariantMap& eventData);
	/// Handle key down event to process key controls common to all samples.
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);

	void HandleMenuBarAction(StringHash eventType, VariantMap& eventData);

	void HandleOpenProject(StringHash eventType, VariantMap& eventData);

	SharedPtr<UIElement> rootUI_;
	SharedPtr<UIElement> welcomeUI_;
	SharedPtr<IDESettings> settings_;
	SharedPtr<Editor> editor_;

	///cached subsystems
	ProjectManager* prjMng_;
	ResourceCache* cache_;
	UI* ui_;
	Console* console_;
	DebugHud* debugHud_;
	Graphics* graphics_;

	/// Flag to indicate whether touch input has been enabled.
	bool touchEnabled_;

private:
	/// Screen joystick index for navigational controls (mobile platforms only).
	unsigned screenJoystickIndex_;
	/// Screen joystick index for settings (mobile platforms only).
	unsigned screenJoystickSettingsIndex_;
	/// Pause flag.
	bool paused_;
};
