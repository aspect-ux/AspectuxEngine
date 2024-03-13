#pragma once

#include <string>
#include <filesystem>

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Asset/AssetManager/EditorAssetManager.h"

namespace Aspect {

	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::string AssetRegistryPath;
		std::filesystem::path ScriptModulePath;

		bool AutomaticallyReloadAssembly = false;//todo

		bool EnableAutoSave = false;
		int AutoSaveIntervalSeconds = 300;

		// Not serialized
		std::string ProjectFileName;
		std::string ProjectDirectory;
	};

	class Project
	{
	public:
		static const std::string& GetProjectName()
		{
			AS_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetConfig().Name;
		}

		static const std::filesystem::path& GetProjectDirectory()
		{
			AS_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->m_ProjectDirectory;
		}

		//TODO: remove
		static const std::filesystem::path& GetRelativeAssetDirectory()
		{
			AS_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			AS_CORE_ASSERT(s_ActiveProject);
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		// TODO(Yan): move to asset manager when we have one
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			AS_CORE_ASSERT(s_ActiveProject);
			return GetAssetDirectory() / path;
		}

		//TODO:
	    static std::filesystem::path GetScriptModulePath()
		{
			AS_CORE_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / std::filesystem::path(s_ActiveProject->GetConfig().AssetDirectory) / s_ActiveProject->GetConfig().ScriptModulePath;
		}

		static std::filesystem::path GetScriptModuleFilePath()
		{
			AS_CORE_ASSERT(s_ActiveProject);
			return GetScriptModulePath() / fmt::format("{0}.dll", GetProjectName());
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);


		//==========================TODO:====================================
		static std::filesystem::path GetAssetRegistryPath()
		{
			AS_CORE_ASSERT(s_ActiveProject);
			//AS_CORE_WARN("ProjectDirectory", s_ActiveProject->GetConfig().ProjectDirectory);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetRegistryPath;
		}

		inline static AspectRef<AssetManagerBase> GetAssetManager() { return s_AssetManager; }
		inline static AspectRef<EditorAssetManager> GetEditorAssetManager() { return s_AssetManager.As<EditorAssetManager>(); }
		//inline static Ref<RuntimeAssetManager> GetRuntimeAssetManager() { return s_AssetManager.As<RuntimeAssetManager>(); }

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<Project> s_ActiveProject;

		inline static AspectRef<AssetManagerBase> s_AssetManager;
	};
}
