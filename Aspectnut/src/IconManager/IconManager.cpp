#include "IconManager.h"
#include "Aspect/Asset/ConfigManager/ConfigManager.h"
#include "Aspect/Asset/AssetManager.h"

namespace Aspect
{
	IconManager::IconManager()
	{
		AS_CORE_INFO("Load Icon...");

		Add("EntityIcon", Texture2D::Create(AssetManager::GetFullStringPath("Resources/Icons/Entity.png")));
		Add("NullTextrue", Texture2D::Create(AssetManager::GetFullStringPath("Assets/Textures/DefaultTexture.png")));
		Add("SettingIcon", Texture2D::Create(std::string(("Resources/Icons/Setting.png"))));
		Add("DirectoryIcon", Texture2D::Create(std::string(("Resources/Icons/ContentBrowser/DirectoryIcon.png"))));
		Add("FileIcon", Texture2D::Create(std::string(("Resources/Icons/ContentBrowser/FileIcon.png"))));
		Add("PlayIcon", Texture2D::Create(std::string(("Resources/Icons/PlayButton.png"))));
		Add("StopIcon", Texture2D::Create(std::string(("Resources/Icons/StopButton.png"))));
		Add("TransIcon", Texture2D::Create(std::string(("Resources/Icons/Trans.png"))));
		Add("RotateIcon", Texture2D::Create(std::string(("Resources/Icons/Rotate.png"))));
		Add("ScaleIcon", Texture2D::Create(std::string(("Resources/Icons/Scale.png"))));
		Add("MusicIcon", Texture2D::Create(std::string(("Resources/Icons/Music.png"))));
	}
}
