#include "aspch.h"

#include "Aspect/Asset/ConfigManager/ConfigManager.h"
#include "Aspect/Project/Project.h"

namespace Aspect
{
	glm::vec2 ConfigManager::m_ViewportSize = { 1280.0f, 720.0f };
	int ConfigManager::selectedEntity = -1;

	ConfigManager::ConfigManager()
	{
		//TODO:Repair
		//temp: root folder is of no use temporarily
		m_RootFolder;//{ AS_XSTRINGIFY_MACRO(ENGINE_ROOT_DIR) };
		m_AssetsFolder = m_RootFolder / "Assets";
		m_ShadersFolder = m_RootFolder / "Shaders";
		m_ResourcesFolder = m_RootFolder / "Resources";
	}

	void ConfigManager::Clear()
	{
		m_RootFolder.clear();
		m_AssetsFolder.clear();
		m_ShadersFolder.clear();
		m_ResourcesFolder.clear();
	}

	std::filesystem::path ConfigManager::GetFullPath(const std::string& relativePath)
	{
		return ConfigManager::GetInstance().GetRootFolder() / relativePath;
	}

	const std::filesystem::path& ConfigManager::GetRootFolder() const
	{
		AS_CORE_ASSERT(std::filesystem::exists(m_RootFolder));
		return m_RootFolder;
	}

	const std::filesystem::path& ConfigManager::GetAssetsFolder() const
	{
		AS_CORE_ASSERT(std::filesystem::exists(m_AssetsFolder));
		return m_AssetsFolder;
	}

	const std::filesystem::path& ConfigManager::GetShadersFolder() const
	{
		AS_CORE_ASSERT(std::filesystem::exists(m_ShadersFolder));
		return m_ShadersFolder;
	}

	const std::filesystem::path& ConfigManager::GetResourcesFolder() const
	{
		AS_CORE_ASSERT(std::filesystem::exists(m_ResourcesFolder));
		return m_ResourcesFolder;
	}
}
