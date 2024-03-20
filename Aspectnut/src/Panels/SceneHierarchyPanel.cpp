#include "SceneHierarchyPanel.h"
#include "Aspect/Scene/Components.h"
#include "Aspect/Script/ScriptEngine.h"
#include "Aspect/Utilities/PlatformUtils.h"
#include "Aspect/Asset/ConfigManager/ConfigManager.h"
#include "Aspect/Editor/EditorResources.h"

#include "../ImGuiWrapper/ImGuiWrapper.h"
#include "../IconManager/IconManager.h"

#include "Aspect/UI/UI.h"
#include "Aspect/ImGui/UICore.h"
#include "Aspect/ImGui/Colours.h"
#include "Aspect/ImGui/ImGui.h"
#include "Aspect/Events/KeyEvent.h"
#include "Aspect/Core/Input.h"

//#ifndef IMGUI_DEFINE_MATH_OPERATORS
//#define IMGUI_DEFINE_MATH_OPERATORS
//#endif // !IMGUI_DEFINE_MATH_OPERATORS
//#include <imgui/imgui.h>
//#include <imgui/imgui_internal.h>
////#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <regex>



/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
  #define _CRT_SECURE_NO_WARNINGS
#endif

namespace Aspect {

	static ImRect s_WindowBounds;
	static bool s_ActivateSearchWidget = false;

	SelectionContext SceneHierarchyPanel::s_ActiveSelectionContext = SelectionContext::Scene;

	// TODO: temporarily use m_TestContext to substitute for m_SelectionContect
	SceneHierarchyPanel::SceneHierarchyPanel(const AspectRef<Scene>& context, SelectionContext selectionContext, bool isWindow)
		: m_Context(context), m_TestContext(selectionContext)//, m_IsWindow(isWindow)
	{

	}

	/*
	SceneHierarchyPanel::SceneHierarchyPanel(const AspectRef<Scene>& scene)
	{
		SetContext(context);	
	}*/

	void SceneHierarchyPanel::SetContext(const AspectRef<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		//TODO:
		s_ActiveSelectionContext = m_TestContext;

		ImGui::Begin("Scene Hierarchy");

		//TODO:
		s_WindowBounds = ImGui::GetCurrentWindow()->Rect();

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID , m_Context.Raw() };
					DrawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			//TODO:
			Entity newEntity;
			// Right-click on blank space
			// popup_flag: default 1, 0 for mouse left,1 for mouse right, 2 for mouse middle
			if (ImGui::BeginPopupContextWindow(0, 1,false))//TODO: FALSE IMGUI P
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");

				if (ImGui::MenuItem("Create Point Light"))
				{
					newEntity = m_Context->CreateEntity("Point Light");
					newEntity.AddComponent<PointLightComponent>();
					SetSelectedEntity(newEntity);
				}

				if (ImGui::MenuItem("Create Directional Light"))
				{
					newEntity = m_Context->CreateEntity("Directional Light");
					newEntity.AddComponent<DirectionalLightComponent>();
					SetSelectedEntity(newEntity);
				}

				// TODO:
				//SelectionManager::DeselectAll();
				//SelectionManager::Select(s_ActiveSelectionContext, newEntity.GetUUID());

				/*if (ImGui::MenuItem("Create Audio"))
				{
					auto entity = m_Context->CreateEntity("Audio");
					entity.AddComponent<SoundComponent>();
					SetSelectedEntity(entity);
				}*/

				ImGui::EndPopup();
			}

		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::OnEvent(Event& event)
	{
		if (!m_IsWindowFocused)
			return;

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& e)
			{
				if (ImGui::IsMouseHoveringRect(s_WindowBounds.Min, s_WindowBounds.Max, false) && !ImGui::IsAnyItemHovered())
				{
					m_FirstSelectedRow = -1;
					m_LastSelectedRow = -1;
					SelectionManager::DeselectAll();
					return true;
				}

				return false;
			});

		dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent& e)
			{
				if (!m_IsWindowFocused)
					return false;

				switch (e.GetKeyCode())
				{
				case Key::F:
				{
					s_ActivateSearchWidget = true;
					return true;
				}
				case Key::Escape:
				{
					m_FirstSelectedRow = -1;
					m_LastSelectedRow = -1;
					break;
				}
				}

				return false;
			});
	}

	bool SceneHierarchyPanel::TagSearchRecursive(Entity entity, std::string_view searchFilter, uint32_t maxSearchDepth, uint32_t currentDepth)
	{
		if (searchFilter.empty())
			return false;

		for (auto child : entity.Children())
		{
			Entity e = m_Context->GetEntityWithUUID(child);
			if (e.HasComponent<TagComponent>())
			{
				if (UI::IsMatchingSearch(e.GetComponent<TagComponent>().Tag, searchFilter))
					return true;
			}

			bool found = TagSearchRecursive(e, searchFilter, maxSearchDepth, currentDepth + 1);
			if (found)
				return true;
		}
		return false;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		//TODO:
		// Drag & Drop
		//------------
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			const auto& selectedEntities = SelectionManager::GetSelections(s_ActiveSelectionContext);
			UUID entityID = entity.GetUUID();

			if (!SelectionManager::IsSelected(s_ActiveSelectionContext, entityID))
			{
				const char* name = entity.Name().c_str();
				ImGui::TextUnformatted(name);
				ImGui::SetDragDropPayload("scene_entity_hierarchy", &entityID, 1 * sizeof(UUID));
			}
			else
			{
				for (const auto& selectedEntity : selectedEntities)
				{
					Entity e = m_Context->GetEntityWithUUID(selectedEntity);
					const char* name = e.Name().c_str();
					ImGui::TextUnformatted(name);
				}

				ImGui::SetDragDropPayload("scene_entity_hierarchy", selectedEntities.data(), selectedEntities.size() * sizeof(UUID));
			}

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("scene_entity_hierarchy", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

			if (payload)
			{
				size_t count = payload->DataSize / sizeof(UUID);

				for (size_t i = 0; i < count; i++)
				{
					UUID droppedEntityID = *(((UUID*)payload->Data) + i);
					Entity droppedEntity = m_Context->GetEntityWithUUID(droppedEntityID);
					m_Context->ParentEntity(droppedEntity, entity);
				}
			}

			ImGui::EndDragDropTarget();
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");


		static float addComponentPanelWidth = 250.0f;
		ImVec2 windowPos = ImGui::GetWindowPos();
		const float maxHeight = ImGui::GetContentRegionMax().y - 60.0f;
		if (ImGui::BeginPopup("AddComponent"))
		{
			
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<MeshComponent>("Mesh Renderer");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
			DisplayAddComponentEntry<TextComponent>("Text Component");
			DisplayAddComponentEntry<DirectionalLightComponent>("Directional Light Component");
			DisplayAddComponentEntry<PointLightComponent>("Point Light Component");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();


		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			ImGui::Checkbox("Primary", &component.Primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.SetOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
			}
		});

		/*DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable
		{
			//TODO:===================
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("C# Script");
			ImGui::NextColumn();

			//ImGui::Text("To be a path");

			//ImGui::SameLine();
			//if (ImGui::Button("..."))
			//{
			//	std::string filepath = FileDialogs::OpenFile("C# Script (*.cs)\0*.cs\0");
			//	if (!filepath.empty())
			//	{
			//		filepath = std::regex_replace(filepath, std::regex("\\\\"), "/");
			//		filepath = filepath.substr(filepath.find_last_of("/") + 1, filepath.length());
			//		//component.Path = filepath;
			//	}
			//}
			//ImGui::EndColumns();

			//=========================
			bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

			static char buffer[64];
			strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

			UI::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

			if (ImGui::InputText("Class", buffer, sizeof(buffer)))
			{
				component.ClassName = buffer;
				return;
			}

			//ScriptEngine::InitializeScriptEntity(entity);
			
			// Fields
			// old code to get class
			bool sceneRunning = scene->IsRunning();
			if (sceneRunning)
			{
				Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
				if (scriptInstance)
				{
					const auto& fields = scriptInstance->GetScriptClass()->GetFields();
					for (const auto& [name, field] : fields)
					{
						if (field.Type == ScriptFieldType::Float)
						{
							float data = scriptInstance->GetFieldValue<float>(name);
							if (ImGui::DragFloat(name.c_str(), &data))
							{
								scriptInstance->SetFieldValue(name, data);
							}
						}
					}
				}
			}
			else
			{
				if (scriptClassExists)
				{
					AS_CORE_WARN("Script Class exists");
					Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
					const auto& fields = entityClass->GetFields();

					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					for (const auto& [name, field] : fields)
					{
						// Field has been set in editor
						if (entityFields.find(name) != entityFields.end())
						{
							ScriptFieldInstance& scriptField = entityFields.at(name);

							// Display control to set it maybe
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptField.GetValue<float>();
								if (ImGui::DragFloat(name.c_str(), &data))
									scriptField.SetValue(data);
							}
						}
						else
						{
							// Display control to set it maybe
							if (field.Type == ScriptFieldType::Float)
							{
								float data = 0.0f;
								if (ImGui::DragFloat(name.c_str(), &data))
								{
									ScriptFieldInstance& fieldInstance = entityFields[name];
									fieldInstance.Field = field;
									fieldInstance.SetValue(data);
								}
							}
						}
					}
				}
			}

		});*/

		DrawComponent<ScriptComponent>("Script", entity,[=](ScriptComponent& firstComponent/* const std::vector<UUID>& entities, const bool isMultiEdit*/) mutable
			{
				UI::BeginPropertyGrid();
				/*ImGui::SetColumnWidth(0, 100.0f);
				ImGui::NextColumn();
				ImGui::EndColumns();*/
				const bool inconsistentScriptClass = IsInconsistentPrimitive<AssetHandle, ScriptComponent>([](const ScriptComponent& other) { return other.ScriptClassHandle; });
				//AS_CORE_INFO("Start draw script compo InconsistentScriptClass:{0}", inconsistentScriptClass);
				ImGui::PushItemFlag(ImGuiItemFlags_MixedValue,/* isMultiEdit&&*/ inconsistentScriptClass);

				bool isError = !ScriptEngine::IsModuleValid(firstComponent.ScriptClassHandle);
				const UI::PropertyAssetReferenceSettings c_AssetRefSettings = { true, false, 0.0f, true, (isError && !inconsistentScriptClass) ? ImVec4(0.9f, 0.2f, 0.2f, 1.0f) : ImGui::ColorConvertU32ToFloat4(Colours::Theme::text), ImGui::ColorConvertU32ToFloat4(Colours::Theme::textError), true };

				AssetHandle oldAssetHandle = firstComponent.ScriptClassHandle;

				if (!UI::PropertyAssetReference<ScriptAsset>("Script Class", firstComponent.ScriptClassHandle, nullptr, c_AssetRefSettings))
				{
					isError = !ScriptEngine::IsModuleValid(firstComponent.ScriptClassHandle);
					//for (auto entityID : entities)
					{
						//Entity entity = m_Context->GetEntityWithUUID(entityID);
						auto& sc = entity.GetComponent<ScriptComponent>();
						sc.ScriptClassHandle = firstComponent.ScriptClassHandle;

						if (isError)
						{
							//AS_CORE_ERROR("Error ScriptComponent");
							bool wasCleared = sc.ScriptClassHandle == 0;
							if (wasCleared)
								sc.ScriptClassHandle = oldAssetHandle; // NOTE(Peter): We need the old asset handle to properly shutdown an entity (at least during runtime)

							ScriptEngine::ShutdownScriptEntity(entity);

							if (wasCleared)
								sc.ScriptClassHandle = 0;
						}
						else
						{
							ScriptEngine::InitializeScriptEntity(entity);
						}
					}
				}

				ImGui::PopItemFlag();

				UI::EndPropertyGrid();
#if 0
				// NOTE(Peter): Editing fields doesn't really work if there's inconsistencies with the script classes...
				if (!isError && !inconsistentScriptClass && firstComponent.FieldIDs.size() > 0)
				{
					UI::BeginPropertyGrid();
					Entity firstEntity = m_Context->GetEntityWithUUID(entities[0]);

					for (auto fieldID : firstComponent.FieldIDs)
					{
						FieldInfo* field = ScriptCache::GetFieldByID(fieldID);
						AspectRef<FieldStorageBase> storage = ScriptEngine::GetFieldStorage(firstEntity, field->ID);

						/*const bool isHidden = field->HasAttribute("Aspect.HideFromEditorAttribute") || !field->IsWritable() || !storage->IsLive();

						if (isHidden && !ApplicationSettings::Get().ShowHiddenFields)
							continue;

						UI::ScopedDisable disable(isHidden);
						std::string fieldName = field->DisplayName.empty() ? Utils::String::SubStr(field->Name, field->Name.find(':') + 1) : field->DisplayName;

						// TODO(Peter): Update field input to display "---" when there's mixed values
						if (field->IsArray())
						{
							if (UI::DrawFieldArray(m_Context, fieldName, storage.As<ArrayFieldStorage>()))
							{
								for (auto entityID : entities)
								{
									/*Entity entity = m_Context->GetEntityWithUUID(entityID);
									const auto& sc = entity.GetComponent<ScriptComponent>();
									storage->CopyData(firstComponent.ManagedInstance, sc.ManagedInstance);*/
								}
							}
						}
						else
						{
							if (UI::DrawFieldValue(m_Context, fieldName, storage.As<FieldStorage>()))
							{
								for (auto entityID : entities)
								{
									/*Entity entity = m_Context->GetEntityWithUUID(entityID);
									const auto& sc = entity.GetComponent<ScriptComponent>();
									storage->CopyData(firstComponent.ManagedInstance, sc.ManagedInstance);*/
								}
							}
						}
					}

					UI::EndPropertyGrid();
				}
#endif
				
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			
			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath(path);
					Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
					if (texture->Loaded())
						component.Texture = texture;
					else
						AS_WARN("Could not load texture {0}", texturePath.filename().string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});

		//TODO:
		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
			{
				ImGuiWrapper::DrawTwoUI(
					[]() { ImGui::Text("Light Intensity"); },
					[&component = component]() { ImGui::SliderFloat("##Light Intensity", &component.Intensity, 0.0f, 10.0f, "%.2f"); }
				);
			});

		DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
			{
				ImGuiWrapper::DrawTwoUI(
					[]() { ImGui::Text("Light Intensity"); },
					[&component = component]() { ImGui::SliderFloat("##Light Intensity", &component.Intensity, 0.0f, 10000.0f, "%.1f"); }
				);

				ImGuiWrapper::DrawTwoUI(
					[]() { ImGui::Text("Light Color"); },
					[&component = component]() { ImGui::ColorEdit3("##Light Color", (float*)&component.LightColor); }
				);
			});

		DrawComponent<MeshComponent>("Mesh Renderer", entity, [](MeshComponent& component)
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("Mesh Path");
				ImGui::NextColumn();

				std::string standardPath = std::regex_replace(component.Path, std::regex("\\\\"), "/");
				ImGui::Text(std::string_view(standardPath.c_str() + standardPath.find_last_of("/") + 1, standardPath.length()).data());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						auto path = (const wchar_t*)payload->Data;
						component.Path = (std::filesystem::path("Assets") / path).string();
						component.m_Mesh = AspectRef<Mesh>::Create(component.Path);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				if (ImGui::Button("..."))
				{
					std::string filepath = FileDialogs::OpenFile("Model (*.obj *.fbx *.dae *.gltf)\0");
					if (filepath.find("Assets") != std::string::npos)
					{
						filepath = filepath.substr(filepath.find("Assets"), filepath.length());
					}
					else
					{
						// TODO: Import Mesh
						//AS_CORE_ASSERT(false, "Aspect Now Only support the model from Assets!");
						//filepath = "";
					}
					if (!filepath.empty())
					{
						component.m_Mesh = AspectRef<Mesh>::Create(filepath);
						component.Path = filepath;
					}
				}
				ImGui::EndColumns();

				if (ImGuiWrapper::TreeNodeExStyle2((void*)"Material", "Material"))
				{
					uint32_t matIndex = 0;

					const auto& materialNode = [&matIndex = matIndex](const char* name, AspectRef<Material>& material, Ref<Texture2D>& tex, void(*func)(AspectRef<Material>& mat)) {
						std::string label = std::string(name) + std::to_string(matIndex);
						ImGui::PushID(label.c_str());

						if (ImGui::TreeNode((void*)name, name))
						{
							ImGui::Image((ImTextureID)tex->GetRendererID(), ImVec2(64, 64), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									auto path = (const wchar_t*)payload->Data;
									std::string relativePath = (std::filesystem::path("Assets") / path).string();
									std::filesystem::path texturePath = ConfigManager::GetInstance().GetAssetsFolder() / path;
									relativePath = std::regex_replace(relativePath, std::regex("\\\\"), "/");
									tex = IconManager::GetInstance().LoadOrFindTexture(relativePath);
								}
								ImGui::EndDragDropTarget();
							}

							func(material);

							ImGui::TreePop();
						}

						ImGui::PopID();
						};

					for (auto& material : component.m_Mesh->m_Materials)
					{
						std::string label = std::string("material") + std::to_string(matIndex);
						ImGui::PushID(label.c_str());

						if (ImGui::TreeNode((void*)label.c_str(), std::to_string(matIndex).c_str()))
						{
							materialNode("Albedo", material, material->mAlbedoMap, [](AspectRef<Material>& mat) {
								ImGui::SameLine();
								ImGui::Checkbox("Use", &mat->bUseAlbedoMap);

								if (ImGui::ColorEdit4("##albedo", glm::value_ptr(mat->col)))
								{
									if (!mat->bUseAlbedoMap)
									{
										unsigned char data[4];
										for (size_t i = 0; i < 4; i++)
										{
											data[i] = (unsigned char)(mat->col[i] * 255.0f);
										}
										mat->albedoRGBA->SetData(data, sizeof(unsigned char) * 4);
									}
								}
								});

							materialNode("Normal", material, material->mNormalMap, [](AspectRef<Material>& mat) {
								ImGui::SameLine();
								ImGui::Checkbox("Use", &mat->bUseNormalMap);
								});

							materialNode("Metallic", material, material->mMetallicMap, [](AspectRef<Material>& mat) {
								ImGui::SameLine();

								if (ImGui::BeginTable("Metallic", 1))
								{
									ImGui::TableNextRow();
									ImGui::TableNextColumn();

									ImGui::Checkbox("Use", &mat->bUseMetallicMap);

									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									if (ImGui::SliderFloat("##Metallic", &mat->metallic, 0.0f, 1.0f))
									{
										if (!mat->bUseMetallicMap)
										{
											unsigned char data[4];
											for (size_t i = 0; i < 3; i++)
											{
												data[i] = (unsigned char)(mat->metallic * 255.0f);
											}
											data[3] = (unsigned char)255.0f;
											mat->metallicRGBA->SetData(data, sizeof(unsigned char) * 4);
										}
									}

									ImGui::EndTable();
								}
								});

							materialNode("Roughness", material, material->mRoughnessMap, [](AspectRef<Material>& mat) {
								ImGui::SameLine();

								if (ImGui::BeginTable("Roughness", 1))
								{
									ImGui::TableNextRow();
									ImGui::TableNextColumn();

									ImGui::Checkbox("Use", &mat->bUseRoughnessMap);

									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									if (ImGui::SliderFloat("##Roughness", &mat->roughness, 0.0f, 1.0f))
									{
										if (!mat->bUseRoughnessMap)
										{
											unsigned char data[4];
											for (size_t i = 0; i < 3; i++)
											{
												data[i] = (unsigned char)(mat->roughness * 255.0f);
											}
											data[3] = (unsigned char)255.0f;
											mat->roughnessRGBA->SetData(data, sizeof(unsigned char) * 4);
										}
									}
									
									ImGui::EndTable();
								}
								});

							materialNode("Ambient Occlusion", material, material->mAoMap, [](AspectRef<Material>& mat) {
								ImGui::SameLine();
								ImGui::Checkbox("Use", &mat->bUseAoMap);
								});

							ImGui::TreePop();
						}

						matIndex++;

						ImGui::PopID();
					}

					ImGui::TreePop();
				}

				if (component.m_Mesh->bAnimated)
				{
					if (ImGuiWrapper::TreeNodeExStyle2((void*)"Animation", "Animation"))
					{
						ImGuiWrapper::DrawTwoUI(
							[&mesh = component.m_Mesh]() {
								static std::string label = "Play";
								if (ImGui::Button(label.c_str()))
								{
									mesh->bPlayAnim = !mesh->bPlayAnim;
									if (mesh->bPlayAnim)
										label = "Stop";
									else
									{
										label = "Play";
										mesh->m_Animator.Reset();
									}
								}
							},
							[&mesh = component.m_Mesh]() {
								static std::string label = "Pause";
								if (ImGui::Button(label.c_str()))
								{
									mesh->bStopAnim = !mesh->bStopAnim;
									if (mesh->bStopAnim)
										label = "Resume";
									else
										label = "Pause";
								}
							},
							88.0f
						);

						ImGui::Columns(2, nullptr, false);
						ImGui::Text("Speed");
						ImGui::NextColumn();
						ImGui::SliderFloat("##Speed", &component.m_Mesh->mAnimPlaySpeed, 0.1f, 10.0f);
						ImGui::EndColumns();

						ImGui::ProgressBar(component.m_Mesh->m_Animator.GetProgress(), ImVec2(0.0f, 0.0f));

						ImGui::TreePop();
					}
				}
			});

			//=======================To be fixed ==================================

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic"};
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = (Rigidbody2DComponent::BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
		});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat("Radius", &component.Radius);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});
		/*TODO: TO BE FIXED
		DrawComponent<TextComponent>("Text Renderer", entity, [](auto& component)
		{
			ImGui::InputTextMultiline("Text String", &component.TextString);
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
			ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);
		});*/

	}
	
	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
}
