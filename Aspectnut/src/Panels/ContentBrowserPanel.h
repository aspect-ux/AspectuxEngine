#pragma once

#include "Aspect/Renderer/Texture.h"
#include "Aspect/Asset/AssetMetadata.h"

#include <optional>
#include <filesystem>


namespace Aspect {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

		void RegisterItemActivateCallbackForType(AssetType type, const std::function<void(const AssetMetadata&)>& callback)
		{
			m_ItemActivationCallbacks[type] = callback;
		}
	private:
		// 绘制文件面板
		void DrawContent();
		void DrawTree();
		void DrawTreeRecursive(const std::filesystem::path& currentPath);

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::optional<std::filesystem::path> m_SelectedDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;

		// TODO:Added
		//std::unordered_map<AssetHandle, Ref<DirectoryInfo>> m_Directories;

		std::unordered_map<AssetType, std::function<void(const AssetMetadata&)>> m_ItemActivationCallbacks;
		std::vector<std::function<void(const AssetMetadata&)>> m_NewAssetCreatedCallbacks;
		std::vector<std::function<void(const AssetMetadata&)>> m_AssetDeletedCallbacks;
	};

}
