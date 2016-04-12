//
// Copyright (c) 2008-2016 the Urho3D project.
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

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>
using namespace Urho3D;
class MyApp : public Application
{
public:
    MyApp(Context* context) :
        Application(context)
    {
    }
    virtual void Setup()
    {
        // Called before engine initialization. engineParameters_ member variable can be modified here
    }
    virtual void Start()
    {
        // Called after engine initialization. Setup application & subscribe to events here
        SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));
    }
    virtual void Stop()
    {
        // Perform optional cleanup after main loop has terminated
    }
    void HandleKeyDown(StringHash eventType, VariantMap& eventData)
    {
        using namespace KeyDown;
        // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESC)
            engine_->Exit();
    }
};
URHO3D_DEFINE_APPLICATION_MAIN(MyApp)