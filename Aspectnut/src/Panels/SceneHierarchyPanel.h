#pragma once

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Scene/Scene.h"
#include "Aspect/Scene/Entity.h"
#include "Aspect/Editor/SelectionManager.h"

namespace Aspect {

// NOTE(Peter): Stolen from imgui.h since IM_COL32 isn't available in this header
#define COLOR32(R,G,B,A)    (((ImU32)(A)<<24) | ((ImU32)(B)<<16) | ((ImU32)(G)<<8) | ((ImU32)(R)<<0))

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const AspectRef<Scene>& scene);
		SceneHierarchyPanel(const AspectRef<Scene>& scene, SelectionContext selectionContext, bool isWindow = true);

		void SetContext(const AspectRef<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);

		void SetEntityDeletedCallback(const std::function<void(Entity)>& func) { m_EntityDeletedCallback = func; }
		void OnEvent(Event& event);
	public:
		template<typename TPrimitive, typename TComponent, typename GetOtherFunc>
		bool IsInconsistentPrimitive(GetOtherFunc func)
		{
			//TODO:
			const auto& entities = SelectionManager::GetSelections(m_TestContext);

			if (entities.size() < 2)
				return false;

			Entity firstEntity = m_Context->GetEntityWithUUID(entities[0]);
			const TPrimitive& first = func(firstEntity.GetComponent<TComponent>());

			for (size_t i = 1; i < entities.size(); i++)
			{
				Entity entity = m_Context->GetEntityWithUUID(entities[i]);

				if (!entity.HasComponent<TComponent>())
					continue;

				const auto& otherValue = func(entity.GetComponent<TComponent>());
				if (otherValue != first)
					return true;
			}

			return false;
		}

		static SelectionContext GetActiveSelectionContext() { return s_ActiveSelectionContext; }
		AspectRef<Scene> GetSceneContext() const { return m_Context; }
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
		bool TagSearchRecursive(Entity entity, std::string_view searchFilter, uint32_t maxSearchDepth, uint32_t currentDepth = 1);
		void DrawEntityCreateMenu(Entity parent = {});
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);//v1
		void DrawComponents(const std::vector<UUID>& entities);//v2

		template<typename TComponent, typename... TIncompatibleComponents>
		void DrawSimpleAddComponentButton(SceneHierarchyPanel* _this, const std::string& name, Ref<Texture2D> icon = nullptr)
		{
			bool canAddComponent = false;
			
			for (const auto& entityID : SelectionManager::GetSelections(SceneHierarchyPanel::GetActiveSelectionContext()))
			{
				Entity entity = _this->GetSceneContext()->GetEntityWithUUID(entityID);
				if (!entity.HasComponent<TComponent>())
				{
					canAddComponent = true;
					break;
				}
			}

			if (!canAddComponent)
				return;

			if (icon == nullptr)
				icon = EditorResources::AssetIcon;

			const float rowHeight = 25.0f;
			auto* window = ImGui::GetCurrentWindow();
			window->DC.CurrLineSize.y = rowHeight;
			ImGui::TableNextRow(0, rowHeight);
			ImGui::TableSetColumnIndex(0);

			window->DC.CurrLineTextBaseOffset = 3.0f;

			const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
			const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x - 20,
										rowAreaMin.y + rowHeight };

			ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
			bool isRowHovered, held;
			bool isRowClicked = ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()), &isRowHovered, &held, ImGuiButtonFlags_AllowItemOverlap);
			ImGui::SetItemAllowOverlap();
			ImGui::PopClipRect();

			auto fillRowWithColour = [](const ImColor& colour)
				{
					for (int column = 0; column < ImGui::TableGetColumnCount(); column++)
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
				};

			if (isRowHovered)
				fillRowWithColour(Colours::Theme::background);

			UI::ShiftCursor(1.5f, 1.5f);
			//UI::Image(icon, { rowHeight - 3.0f, rowHeight - 3.0f });
			UI::ShiftCursor(-1.5f, -1.5f);
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-1);
			ImGui::TextUnformatted(name.c_str());
			
			if (isRowClicked)
			{
				for (const auto& entityID : SelectionManager::GetSelections(SceneHierarchyPanel::GetActiveSelectionContext()))
				{
					Entity entity = _this->GetSceneContext()->GetEntityWithUUID(entityID);

					if (sizeof...(TIncompatibleComponents) > 0 && entity.HasComponent<TIncompatibleComponents...>())
						continue;

					if (!entity.HasComponent<TComponent>())
						entity.AddComponent<TComponent>();
				}

				ImGui::CloseCurrentPopup();
			}
		}

		template<typename TComponent, typename... TIncompatibleComponents, typename OnAddedFunction>
		void DrawAddComponentButton(SceneHierarchyPanel* _this, const std::string& name, OnAddedFunction onComponentAdded, Ref<Texture2D> icon = nullptr)
		{
			bool canAddComponent = false;

			for (const auto& entityID : SelectionManager::GetSelections(SceneHierarchyPanel::GetActiveSelectionContext()))
			{
				Entity entity = _this->GetSceneContext()->GetEntityWithUUID(entityID);
				if (!entity.HasComponent<TComponent>())
				{
					canAddComponent = true;
					break;
				}
			}

			if (!canAddComponent)
				return;

			if (icon == nullptr)
				icon = EditorResources::AssetIcon;

			const float rowHeight = 25.0f;
			auto* window = ImGui::GetCurrentWindow();
			window->DC.CurrLineSize.y = rowHeight;
			ImGui::TableNextRow(0, rowHeight);
			ImGui::TableSetColumnIndex(0);

			window->DC.CurrLineTextBaseOffset = 3.0f;

			const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
			const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x - 20,
										rowAreaMin.y + rowHeight };

			ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
			bool isRowHovered, held;
			bool isRowClicked = ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()), &isRowHovered, &held, ImGuiButtonFlags_AllowItemOverlap);
			ImGui::SetItemAllowOverlap();
			ImGui::PopClipRect();

			auto fillRowWithColour = [](const ImColor& colour)
				{
					for (int column = 0; column < ImGui::TableGetColumnCount(); column++)
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
				};

			if (isRowHovered)
				fillRowWithColour(Colours::Theme::background);

			UI::ShiftCursor(1.5f, 1.5f);
			UI::Image(icon, { rowHeight - 3.0f, rowHeight - 3.0f });
			UI::ShiftCursor(-1.5f, -1.5f);
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-1);
			ImGui::TextUnformatted(name.c_str());

			if (isRowClicked)
			{
				for (const auto& entityID : SelectionManager::GetSelections(SceneHierarchyPanel::GetActiveSelectionContext()))
				{
					Entity entity = _this->GetSceneContext()->GetEntityWithUUID(entityID);

					if (sizeof...(TIncompatibleComponents) > 0 && entity.HasComponent<TIncompatibleComponents...>())
						continue;

					if (!entity.HasComponent<TComponent>())
					{
						auto& component = entity.AddComponent<TComponent>();
						onComponentAdded(entity, component);
					}
				}

				ImGui::CloseCurrentPopup();
			}
		}

		//TODO: v1
		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (entity.HasComponent<T>())
			{
				auto& component = entity.GetComponent<T>();
				ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
				ImGui::PopStyleVar(
				);
				ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
				if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}

				bool removeComponent = false;
				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove component"))
						removeComponent = true;

					ImGui::EndPopup();
				}

				if (open)
				{
					uiFunction(component);
					ImGui::TreePop();
				}

				if (removeComponent)
					entity.RemoveComponent<T>();
			}
		}
		
		// TODO: v2
		template<typename TComponent, typename UIFunction>
		void DrawComponent(const std::string& name, UIFunction uiFunction, Ref<Texture2D> icon = nullptr)
		{
			bool shouldDraw = true;

			auto& entities = SelectionManager::GetSelections(s_ActiveSelectionContext);
			AS_CORE_ERROR("SIZE {0}", entities.size());
			for (const auto& entityID : entities)
			{
				Entity entity = m_Context->GetEntityWithUUID(entityID);
				if (!entity.HasComponent<TComponent>())
				{
					shouldDraw = false;
					break;
				}
			}

			if (!shouldDraw || entities.size() == 0)
				return;
			
			if (icon == nullptr)
				icon = EditorResources::AssetIcon;

			// NOTE(Peter):
			//	This fixes an issue where the first "+" button would display the "Remove" buttons for ALL components on an Entity.
			//	This is due to ImGui::TreeNodeEx only pushing the id for it's children if it's actually open
			ImGui::PushID((void*)typeid(TComponent).hash_code());
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			bool open = UI::TreeNodeWithIcon(name, icon, { 14.0f, 14.0f });
			bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
			float lineHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;
			float itemPadding = 4.0f;

			bool resetValues = false;
			bool removeComponent = false;

			ImGui::SameLine(contentRegionAvailable.x - lineHeight - 5.0f);
			UI::ShiftCursorY(lineHeight / 4.0f);
			if (ImGui::InvisibleButton("##options", ImVec2{ lineHeight, lineHeight }) || right_clicked)
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			
			UI::DrawButtonImage(EditorResources::GearIcon, COLOR32(160, 160, 160, 200),
				COLOR32(160, 160, 160, 255),
				COLOR32(160, 160, 160, 150),
				UI::RectExpanded(UI::GetItemRect(), 0.0f, 0.0f));

			if (UI::BeginPopup("ComponentSettings"))
			{
				UI::ShiftCursorX(itemPadding);

				Entity entity = m_Context->GetEntityWithUUID(entities[0]);
				auto& component = entity.GetComponent<TComponent>();
				
				if (ImGui::MenuItem("Copy"))
					Scene::CopyComponentFromScene<TComponent>(m_ComponentCopyEntity, m_ComponentCopyScene, entity, m_Context);
				
				UI::ShiftCursorX(itemPadding);

				if (ImGui::MenuItem("Paste"))
				{
					for (auto entityID : SelectionManager::GetSelections(s_ActiveSelectionContext))
					{
						Entity selectedEntity = m_Context->GetEntityWithUUID(entityID);
						Scene::CopyComponentFromScene<TComponent>(selectedEntity, m_Context, m_ComponentCopyEntity, m_ComponentCopyScene);
					}
				}

				UI::ShiftCursorX(itemPadding);
				if (ImGui::MenuItem("Reset"))
					resetValues = true;

				UI::ShiftCursorX(itemPadding);
				if constexpr (!std::is_same<TComponent, TransformComponent>::value)
				{
					if (ImGui::MenuItem("Remove component"))
						removeComponent = true;
				}

				UI::EndPopup();
			}

			if (open)
			{
				Entity entity = m_Context->GetEntityWithUUID(entities[0]);
				TComponent& firstComponent = entity.GetComponent<TComponent>();
				const bool isMultiEdit = entities.size() > 1;
				uiFunction(firstComponent, entities, isMultiEdit);
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				for (auto& entityID : entities)
				{
					Entity entity = m_Context->GetEntityWithUUID(entityID);
					if (entity.HasComponent<TComponent>())
						entity.RemoveComponent<TComponent>();
				}
			}

			if (resetValues)
			{
				for (auto& entityID : entities)
				{
					Entity entity = m_Context->GetEntityWithUUID(entityID);
					if (entity.HasComponent<TComponent>())
					{
						entity.RemoveComponent<TComponent>();
						entity.AddComponent<TComponent>();
					}
				}
			}

			if (!open)
				UI::ShiftCursorY(-(ImGui::GetStyle().ItemSpacing.y + 1.0f));

			ImGui::PopID();
		}
		
	private:
		AspectRef<Scene> m_Context;
		Entity m_SelectionContext;
		SelectionContext m_TestContext;//TODO: repair
		std::function<void(Entity)> m_EntityDeletedCallback;

		//TODO:
		AspectRef<Scene> m_ComponentCopyScene;
		Entity m_ComponentCopyEntity;

		static SelectionContext s_ActiveSelectionContext;

		bool m_IsWindow;
		bool m_IsWindowFocused = false;
		bool m_IsHierarchyOrPropertiesFocused = false;
		bool m_ShiftSelectionRunning = false;

		int32_t m_FirstSelectedRow = -1;
		int32_t m_LastSelectedRow = -1;
	};

}
