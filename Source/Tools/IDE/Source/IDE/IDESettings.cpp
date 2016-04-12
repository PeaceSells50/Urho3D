#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Variant.h>
#include "IDESettings.h"
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>

namespace Urho3D
{
	IDESettings::IDESettings(Context* context) :Serializable(context)

	{
		ResetToDefault();
	}

	IDESettings::~IDESettings()
	{
	}

	void IDESettings::RegisterObject(Context* context)
	{
		context->RegisterFactory<IDESettings>();

		URHO3D_ATTRIBUTE("WindowTitle", String, windowTitle_, String("Urho3D IDE"), AM_FILE);
		URHO3D_ATTRIBUTE("WindowIcon", String, windowIcon_, String("Textures/UrhoIcon.png"), AM_FILE);
		URHO3D_ATTRIBUTE("WindowWidth", int, windowWidth_, 1024, AM_FILE);
		URHO3D_ATTRIBUTE("WindowHeight", int, windowHeight_, 768, AM_FILE);

		URHO3D_ATTRIBUTE("ResourcePaths", String, resourcePaths_, String("Data;CoreData;IDEData"), AM_FILE);
		URHO3D_ATTRIBUTE("ResourcePackages", String, resourcePackages_, String::EMPTY, AM_FILE);
		URHO3D_ATTRIBUTE("AutoloadPaths", String, autoloadPaths_, String("Extra"), AM_FILE);

		URHO3D_ATTRIBUTE("WindowResizable", bool, windowResizable_, false, AM_FILE);
		URHO3D_ATTRIBUTE("FullScreen", bool, fullScreen_, false, AM_FILE);

		URHO3D_ATTRIBUTE("Projects Root Dir", String, projectsRootDir_, String::EMPTY, AM_FILE);

		URHO3D_ATTRIBUTE("Project Name", String, projectName_, String::EMPTY, AM_FILE);
	}

	VariantMap IDESettings::ToVariantMap()
	{
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if (!attributes)
			return VariantMap();
		VariantMap variantMap;
		Variant value;

		for (unsigned i = 0; i < attributes->Size(); ++i)
		{
			const AttributeInfo& attr = attributes->At(i);
			if (!(attr.mode_ & AM_FILE))
				continue;

			OnGetAttribute(attr, value);
			variantMap[attr.name_] = value;
		}
		return variantMap;
	}

	Urho3D::String IDESettings::GetPreferencesFullPath()
	{
		FileSystem* fs = GetSubsystem<FileSystem>();
		String filepath = fs->GetAppPreferencesDir("urho3d", "ide");
		filepath += "IDESettings.xml";
		return filepath;
	}

	bool IDESettings::Save()
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();

		File saveFile(context_, GetPreferencesFullPath(), FILE_WRITE);
		XMLFile xmlFile(context_);
		XMLElement rootElem = xmlFile.CreateRoot("IDESettings");
		SaveXML(rootElem);
		return xmlFile.Save(saveFile);
	}

	bool IDESettings::Load()
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();

		if (fileSystem->FileExists(GetPreferencesFullPath()))
		{
			File loadFile(context_, GetPreferencesFullPath(), FILE_READ);
			XMLFile loadXML(context_);
			loadXML.BeginLoad(loadFile);
			LoadXML(loadXML.GetRoot("IDESettings"));
			return true;
		}
		else
			return false;
	}

	void IDESettings::LoadConfigFile()
	{
		if (!Load())
		{
			/// file does not exist so create default file
			Save();
		}
	}
}