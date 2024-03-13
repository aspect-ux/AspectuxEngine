#include "aspch.h"
#include "ContentBrowserPanel.h"

#include "Aspect/Project/Project.h"


// TEMP
#include "../IconManager/IconManager.h"
#include "Aspect/Asset/ConfigManager/ConfigManager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

namespace Aspect {

	namespace Utils
	{
		static bool HaveDirectoryMember(std::filesystem::path currentPath)
		{
			for (auto& directoryEntry : std::filesystem::directory_iterator(currentPath))
			{
				if (directoryEntry.is_directory())
					return true;
			}
			return false;
		}

		static bool IsImageFormat(std::string filePath)
		{
			std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
			if (extension == "png" || extension == "jpg" || extension == "bmp" || extension == "hdr" || extension == "tga")
			{
				return true;
			}
			return false;
		}

		static bool IsMusicFormat(std::string filePath)
		{
			std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
			if (extension == "mp3" || extension == "wma" || extension == "wav")
			{
				return true;
			}
			return false;
		}
	}

	// TODO: Move ConfigManager 
	ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		// initialize selected directory
		m_SelectedDirectory = std::optional<std::filesystem::path>(m_BaseDirectory);

		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGui::Columns(2);

		static bool init = true;
		if (init)
		{
			ImGui::SetColumnWidth(0, 5.0f);
			init = false;
		}

		if (ImGui::BeginChild("CONTENT_BROWSER_TREE"))
		{
			DrawTree();
		}
		ImGui::EndChild();

		ImGui::NextColumn();

		if (ImGui::BeginChild("CONTENT_BROWSER_CONTENT"))
		{
			DrawContent();
		}
		ImGui::EndChild();
		
		ImGui::Columns(1);

		ImGui::End();
	}

	void ContentBrowserPanel::DrawContent()
	{
		if (!m_SelectedDirectory)
		{
			return;
		}

		m_CurrentDirectory = *m_SelectedDirectory;

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();

				
				auto tempAbsoluteDirectory = Project::GetAssetDirectory() / m_CurrentDirectory;
				AS_CORE_INFO("Directory selected" + tempAbsoluteDirectory.string());

				m_SelectedDirectory = { m_CurrentDirectory };// reset selected directory(relative)
			}
		}

		static float padding = 8.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();

					m_SelectedDirectory = { m_CurrentDirectory };
				}

			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

	}

	void ContentBrowserPanel::DrawTree()
	{
		// ConfigManager::GetInstance().GetAssetsFolder()
		DrawTreeRecursive(Project::GetAssetDirectory());
	}

	void ContentBrowserPanel::DrawTreeRecursive(const std::filesystem::path& currentPath)
	{
		const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

		ImGuiTreeNodeFlags nodeFlags = baseFlags;

		if (m_SelectedDirectory && *m_SelectedDirectory == currentPath)
		{
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool bNeedOpen = true;
		if (!Utils::HaveDirectoryMember(currentPath))
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			bNeedOpen = false;
		}

		std::string label = "##" + currentPath.filename().string();
		bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

		if (ImGui::IsItemClicked())
		{
			m_SelectedDirectory = currentPath;
		}

		ImGui::SameLine();
		ImGui::Image((ImTextureID)IconManager::GetInstance().GetDirectoryIcon()->GetRendererID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
		ImGui::SameLine();
		ImGui::Text(currentPath.filename().string().c_str());

		if (nodeOpen && bNeedOpen)
		{
			for (auto& p : std::filesystem::directory_iterator(currentPath))
			{
				const auto& path = p.path();
				if (!std::filesystem::is_directory(path))
				{
					continue;
				}

				DrawTreeRecursive(path);
			}
			ImGui::TreePop();
		}
	}

}
