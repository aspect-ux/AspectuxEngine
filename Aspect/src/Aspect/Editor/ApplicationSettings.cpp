#include "aspch.h"
#include "ApplicationSettings.h"
#include "Aspect/Utilities/FileSystem.h"

#include <yaml-cpp/yaml.h>

#include <sstream>
#include <filesystem>

namespace Aspect {

	static std::filesystem::path s_EditorSettingsPath;
	
	ApplicationSettings& ApplicationSettings::Get()
	{
		static ApplicationSettings s_Settings;
		return s_Settings;
	}

	void ApplicationSettingsSerializer::Init()
	{
		s_EditorSettingsPath = std::filesystem::absolute("Config");

		if (!FileSystem::Exists(s_EditorSettingsPath))
			FileSystem::CreateDirectory(s_EditorSettingsPath);
		s_EditorSettingsPath /= "ApplicationSettings.yaml";

		LoadSettings();
	}

#define AS_ENTER_GROUP(name) currentGroup = rootNode[name];
#define AS_READ_VALUE(name, type, var, defaultValue) var = currentGroup[name].as<type>(defaultValue)

	void ApplicationSettingsSerializer::LoadSettings()
	{
		// Generate default settings file if one doesn't exist
		if (!FileSystem::Exists(s_EditorSettingsPath))
		{
			SaveSettings();
			return;
		}

		std::ifstream stream(s_EditorSettingsPath);
		AS_CORE_VERIFY(stream);
		std::stringstream ss;
		ss << stream.rdbuf();

		YAML::Node data = YAML::Load(ss.str());
		if (!data["ApplicationSettings"])
			return;

		YAML::Node rootNode = data["ApplicationSettings"];
		YAML::Node currentGroup = rootNode;

		auto& settings = ApplicationSettings::Get();

		AS_ENTER_GROUP("Aspectnut");
		{
			AS_READ_VALUE("AdvancedMode", bool, settings.AdvancedMode, false);
		}

		AS_ENTER_GROUP("Scripting");
		{
			AS_READ_VALUE("ShowHiddenFields", bool, settings.ShowHiddenFields, false);
			AS_READ_VALUE("DebuggerListenPort", int, settings.ScriptDebuggerListenPort, 2550);
		}

		AS_ENTER_GROUP("ContentBrowser");
		{
			AS_READ_VALUE("ShowAssetTypes", bool, settings.ContentBrowserShowAssetTypes, true);
			AS_READ_VALUE("ThumbnailSize", int, settings.ContentBrowserThumbnailSize, 128);
		}

		stream.close();
	}

#define AS_BEGIN_GROUP(name)\
		out << YAML::Key << name << YAML::Value << YAML::BeginMap;
#define AS_END_GROUP() out << YAML::EndMap;

#define AS_SERIALIZE_VALUE(name, value) out << YAML::Key << name << YAML::Value << value;

	void ApplicationSettingsSerializer::SaveSettings()
	{
		const auto& settings = ApplicationSettings::Get();

		YAML::Emitter out;
		out << YAML::BeginMap;
		AS_BEGIN_GROUP("ApplicationSettings");
		{
			AS_BEGIN_GROUP("Aspectnut");
			{
				AS_SERIALIZE_VALUE("AdvancedMode", settings.AdvancedMode);
			}
			AS_END_GROUP(); 
			
			AS_BEGIN_GROUP("Scripting");
			{
				AS_SERIALIZE_VALUE("ShowHiddenFields", settings.ShowHiddenFields);
				AS_SERIALIZE_VALUE("DebuggerListenPort", settings.ScriptDebuggerListenPort);
			}
			AS_END_GROUP();

			AS_BEGIN_GROUP("ContentBrowser");
			{
				AS_SERIALIZE_VALUE("ShowAssetTypes", settings.ContentBrowserShowAssetTypes);
				AS_SERIALIZE_VALUE("ThumbnailSize", settings.ContentBrowserThumbnailSize);
			}
			AS_END_GROUP();
		}
		AS_END_GROUP();
		out << YAML::EndMap;

		std::ofstream fout(s_EditorSettingsPath);
		fout << out.c_str();
		fout.close();
	}


}
