#pragma once

#include "Aspect/Core/Base/PublicSingleton.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace Aspect
{
	class ConfigManager final : public PublicSingleton<ConfigManager>
	{
	public:
		ConfigManager();
		ConfigManager(const ConfigManager&) = delete;
		ConfigManager& operator=(const ConfigManager&) = delete;

	public:
		void Clear();

		//TODO:move
		[[nodiscard]] static std::filesystem::path GetFullPath(const std::string& relativePath);

		[[nodiscard]] const std::filesystem::path& GetRootFolder() const;
		[[nodiscard]] const std::filesystem::path& GetAssetsFolder() const;
		[[nodiscard]] const std::filesystem::path& GetShadersFolder() const;
		[[nodiscard]] const std::filesystem::path& GetResourcesFolder() const;

		static glm::vec2 m_ViewportSize;
		static int selectedEntity;
	private:
		std::filesystem::path m_RootFolder;
		std::filesystem::path m_AssetsFolder;
		std::filesystem::path m_ShadersFolder;
		std::filesystem::path m_ResourcesFolder;
	};
}
