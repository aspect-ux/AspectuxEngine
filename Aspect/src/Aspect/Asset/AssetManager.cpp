#include "aspch.h"
#include "Aspect/Asset/AssetManager.h"
#include "Aspect/Asset/ConfigManager/ConfigManager.h"

namespace Aspect
{
	std::filesystem::path AssetManager::GetFullPath(const std::string& RelativePath)
	{
		return ConfigManager::GetInstance().GetRootFolder() / RelativePath;
	}
}
