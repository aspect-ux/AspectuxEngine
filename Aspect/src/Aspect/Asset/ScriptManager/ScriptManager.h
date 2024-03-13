#pragma once

#include "Aspect/Core/Base/PublicSingleton.h"
#include "Aspect/Core/Ref.h"
#include "Aspect/Script/ScriptAsset.h"

#include <filesystem>
#include <glm/glm.hpp>


namespace Aspect
{
	class ScriptManager final : public PublicSingleton<ScriptManager>
	{
	public:
		ScriptManager();
		ScriptManager(const ScriptManager&) = delete;
		ScriptManager& operator=(const ScriptManager&) = delete;

	public:
		static AspectRef<ScriptAsset> CreateScriptAsset(std::filesystem::path scriptPath){}

		static glm::vec2 m_ViewportSize;
		static int selectedEntity;
	private:
		std::filesystem::path m_RootFolder;
		std::filesystem::path m_AssetsFolder;
		std::filesystem::path m_ShadersFolder;
		std::filesystem::path m_ResourcesFolder;
	};
}
