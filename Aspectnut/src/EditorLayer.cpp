#include "EditorLayer.h"

#include "Aspect/Scene/SceneSerializer.h"
// -------Entry Point---------
//#include <Aspect/Core/EntryPoint.h> // 尖括号表示直接从系统类库中找文件，效率更高；  " "搜索范围更广，但是优先包含本地目录中的文件

#include "Aspect/Platform/OpenGL/OpenGLShader.h"
#include "Aspect/Scene/Components.h"

#include "Aspect/Utilities/PlatformUtils.h"
#include "Aspect/Script/ScriptEngine.h"
#include "Aspect/Script/ScriptBuilder.h"
#include "Aspect/Scene/ScriptableEntity.h"
#include "Aspect/Math/Math.h"
#include "Aspect/Renderer/Font.h"

#include "Aspect/Core/Timestep.h"
#include "Aspect/Core/Input.h"
#include "Aspect/Core/KeyCode.h"

//=====================
#include "ImGuiWrapper/ImGuiWrapper.h"
#include "Aspect/Asset/ModeManager/ModeManager.h"
#include "IconManager/IconManager.h"
#include "Aspect/Asset/ConfigManager/ConfigManager.h"
#include "Aspect/Renderer/PostProcessing.h"
#include "Aspect/Asset/AssetManager.h"
#include "Aspect/Editor/SelectionManager.h"
#include "Aspect/Utilities/FileSystem.h"
#include "Aspect/Editor/EditorResources.h"
//=====================

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ImGuizmo.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <magic_enum.hpp>

namespace Aspect
{
	namespace Utils
	{
		// from https://github.com/Acmen-Team/Epoch/tree/dev
		template<typename UIFunction>
		static void SceneToolbar(ImGuiDockNode* node, const float DISTANCE, int* corner, UIFunction uiFunc)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

			ImVec2 work_area_pos = node->Pos;
			ImVec2 work_area_size = node->Size;

			if (*corner != -1)
			{
				window_flags |= ImGuiWindowFlags_NoMove;
				ImGuiViewport* viewport = ImGui::GetMainViewport();

				ImVec2 window_pos = ImVec2((*corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (*corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
				ImVec2 window_pos_pivot = ImVec2((*corner & 1) ? 1.0f : 0.0f, (*corner & 2) ? 1.0f : 0.0f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
				ImGui::SetNextWindowViewport(node->ID);
			}
			ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background

			uiFunc(corner, work_area_size, window_flags);
		}
	}

	static Ref<Font> s_Font;

	class CameraController : public ScriptableEntity {
	public:
		void OnCreate() {}
		void OnDestroy() {}
		void OnUpdate(Timestep ts) {
			// 获取当前挂载CameraController脚本的实体的TransformComponent组件
			auto& transform = GetComponent<TransformComponent>().GetTransform();
			float speed = 5.0f;

			if (Input::IsKeyPressed(Key::A))
				transform[3][0] -= speed * ts;
			if (Input::IsKeyPressed(Key::D))
				transform[3][0] += speed * ts;
			if (Input::IsKeyPressed(Key::W))
				transform[3][1] += speed * ts;

			if (Input::IsKeyPressed(Key::S))
				transform[3][1] -= speed * ts;
		}
	};

	// Window
	static bool bShowViewport = true;
	static bool bShowContentBrowser = true;
	static bool bShowSceneHierachy = true;
	static bool bShowProperties = true;
	static bool bShowStats = false;
	static bool bShowEngineSettings = true;
	static bool bShowSceneSettings = true;
	static bool bShowSRT = true;

	// Help
	static bool bShowTutorial = false;
	static bool bShowAboutMe = false;
	static bool bShowDemoImGui = false;

	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true), m_SquarePosition(0.0f)
	{
		s_Font = Font::GetDefault();
	}

	void EditorLayer::OnAttach()
	{

		// TODO:Should we ever want to actually show editor layer panels in Aspect::Runtime
		// then these lines need to be added to RuntimeLayer::Attach()
		EditorResources::Init();

		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_IconStep = Texture2D::Create("Resources/Icons/StepButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");

		FrameBufferSpecification fbSpec;
		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		fbSpec.Samples = 4; //TODO:? why 4 samplers
		m_FrameBuffer = FrameBuffer::Create(fbSpec);

		//RenderPass
		RenderPassSpecification rpSpec = { m_FrameBuffer, "MainPass",glm::vec4(0.2,0.2,0.2,1.0) };
		m_RenderPass = CreateRef<RenderPass>(rpSpec);
		m_RenderPass->AddPostProcessing(PostProcessingType::MSAA); // default

		m_EditorScene = AspectRef<Scene>::Create();
		m_ActiveScene = m_EditorScene;

		//TODO: Initialize Application specification'
		// Notice: project serialization is ahead of asset loading(temporarily project config is written here)s
		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;

		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
			AS_CORE_WARN("Open project");
		}
		else
		{
			// TODO(Yan): prompt the user to select a directory
			NewProject();

			// If no project is opened, close Aspectnut
			// NOTE: this is while we don't have a new project path
			if (!OpenProject())
				Application::Get().Close();

		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		Renderer2D::SetLineWidth(6.0f);


		//TODO:SetCallBacks
		m_ContentBrowserPanel->RegisterItemActivateCallbackForType(AssetType::Scene, [this](const AssetMetadata& metadata)
		{
			//OpenScene();
		});
		/*m_ContentBrowserPanel->RegisterItemActivateCallbackForType(AssetType::ScriptFile, [this](const AssetMetadata& metadata)
			{
				FileSystem::OpenExternally(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
			});*/

		m_SceneHierarchyPanel.SetEntityDeletedCallback([this](Entity entity) { OnEntityDeleted(entity); });
		

		/*Ref<SceneRendererPanel> sceneRendererPanel = m_PanelManager->AddPanel<SceneRendererPanel>(PanelCategory::View, SCENE_RENDERER_PANEL_ID, "Scene Renderer", true); */

		m_ViewportRenderer = AspectRef<SceneRenderer>::Create(m_CurrentScene);
		m_SceneRendererPanel->SetContext(m_ViewportRenderer);
		//m_SecondViewportRenderer = Ref<SceneRenderer>::Create(m_CurrentScene);
		//m_FocusedRenderer = m_ViewportRenderer;
		//sceneRendererPanel->SetContext(m_FocusedRenderer);
	}

	void EditorLayer::OnDetach()
	{
		AS_PROFILE_FUNCTION();
	}
	/*todo: repair my own update
	void EditorLayer::OnUpdate(Timestep ts)
	{
		// 窗口resize，在每一帧检测
		if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
			// 调整帧缓冲区大小
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			// 调整摄像机投影：之前调整EditorLayer的摄像机宽高比代码
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			//
			// 调整场景内的摄像机:此节所作
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
		
		{
			/// Update
			if(m_ViewportFocused)
				m_CameraController.OnUpdate(ts);

			// 渲染信息初始化
			Renderer2D::ResetStats();
		}
		{
			// 1.绑定自定义的帧缓冲（等于解绑到渲染到默认的帧缓冲中）
			m_FrameBuffer->Bind();

			// Renderer Prep
			RenderCommand::SetClearColor(glm::vec4({ 0.1f, 0.1f, 0.1f, 1.0f }));
			RenderCommand::Clear();
		}
		// TODO: Update Scene
		//m_ActiveScene->OnUpdate(ts);
		// 2.Quad 绘制测试
		{
			static float rotation = 0.0f;
			rotation += ts * 50.0f;
			
			
			//Renderer2D::BeginScene(m_CameraController.GetCamera());
			//Renderer2D::BeginScene(m_CameraEntity.GetComponent<Camera>(),glm::mat4(0.0f));
			//Renderer2D::DrawQuad({ 0.0f,0.0f,0.f }, { 1.0f,1.0f }, m_SpriteSheet, 1.0f);
			
			//Renderer2D::EndScene();
		}
		// 3.解绑帧缓冲
		m_FrameBuffer->UnBind();
	}
	*/

	void EditorLayer::OnUpdate(Timestep ts)
	{
		AS_PROFILE_FUNCTION();

		// 1.Resize
		if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			PostProcessing::mFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// 2. Render
		Renderer2D::ResetStats();
		m_FrameBuffer->Bind();
		RenderCommand::SetClearColor({ 0.4f, 0.4f, 0.4f, 1 });
		RenderCommand::Clear();

		// 2.1 Clear our entity ID attachment to -1
		m_FrameBuffer->ClearAttachment(1, -1);

		// 2.2 switch scene mode
		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			if (m_ViewportFocused)
				m_CameraController.OnUpdate(ts);

			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			m_ActiveScene->OnRenderEditor(ts, m_EditorCamera);
			break;
		}
		case SceneState::Simulate:
		{
			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
			break;
		}
		case SceneState::Play:
		{
			m_ActiveScene->OnUpdateRuntime(ts);
			m_ActiveScene->OnRenderRuntime(ts, m_EditorCamera);
			/*if (m_EditorCameraInRuntime)
			{
				m_EditorCamera.SetActive(m_ViewportPanelMouseOver || m_AllowViewportCameraEvents);
				m_EditorCamera.OnUpdate(ts);
				m_ActiveScene->OnRenderEditor(m_ViewportRenderer, ts, m_EditorCamera);
				OnRender2D();
			}
			else
			{
				m_ActiveScene->OnRenderRuntime(m_ViewportRenderer, ts);
			}*/

			
			for (auto& fn : m_PostSceneUpdateQueue)
				fn();
			m_PostSceneUpdateQueue.clear();
			break;
		}
		}

		
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.Raw());
		}

		
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			ConfigManager::selectedEntity = (int)(uint32_t)selectedEntity;
			if (Input::IsKeyPressed(Key::E))
				m_EditorCamera.SetCenter(selectedEntity.GetComponent<TransformComponent>().Translation);
		}
		else
		{
			ConfigManager::selectedEntity = -1;
		}

		OnOverlayRender();

		m_FrameBuffer->UnBind();
	}

	void EditorLayer::OnImGuiRender()
	{
		AS_PROFILE_FUNCTION();

		static bool bChangeDim = false;

		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					OpenProject();

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
					
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
					ScriptEngine::ReloadAssembly();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("Tutorial", NULL, &bShowTutorial);
				ImGui::MenuItem("About Me", NULL, &bShowAboutMe);
				ImGui::MenuItem("Demo ImGui", NULL, &bShowDemoImGui);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();

		//TODO:fix panel
		static bool bEditorConsoleOpen = true;
		m_EditorConsolePanel->OnImGuiRender(bEditorConsoleOpen);

		m_SceneRendererPanel->OnImGuiRender(bEditorConsoleOpen);

		ImGui::Begin("Stats");

#if 0
		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());
#endif

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::PopStyleColor();  // 恢复默认颜色

		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);

		ImGui::Image((ImTextureID)s_Font->GetAtlasTexture()->GetRendererID(), { 512,512 }, { 0, 1 }, { 1, 0 });

		ImGui::End();

		//TODO:
		if (bShowEngineSettings)
		{
			ImGui::Begin("Engine Settings", &bShowEngineSettings);

			const char* modes[] = { "2D", "3D" };
			int lastMode = ModeManager::b3DMode;
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Mode");
			ImGui::NextColumn();
			if (ImGui::Combo("##Mode", &ModeManager::b3DMode, modes, IM_ARRAYSIZE(modes)))
			{
				if (lastMode != ModeManager::b3DMode)
				{
					bChangeDim = true;
				}
			}
			ImGui::EndColumns();

			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Camera Speed");
			ImGui::NextColumn();
			//ImGui::SliderFloat("##Camera Speed", &m_EditorCamera.m_CameraSpeed, 0.1f, 10.0f);
			ImGui::EndColumns();

			if (ImGuiWrapper::TreeNodeExStyle1((void*)"Physics Settings", "Physics Settings"))
			{
				ImGui::Checkbox("Show physics colliders", &ModeManager::bShowPhysicsColliders);

				if (ImGui::BeginMenu("Show physics colliders modes"))
				{
					using namespace magic_enum::bitwise_operators;

					constexpr auto flags = magic_enum::enum_values<PhysicsDebugDrawModeFlag>();
					for (auto flag : flags)
					{
						if (ImGui::MenuItem(magic_enum::enum_name(flag).data(), NULL, (bool)(ModeManager::mPhysicsDebugDrawModeFlag & flag)))
						{
							/*(bool)(ModeManager::mPhysicsDebugDrawModeFlag& flag) ?
								ModeManager::mPhysicsDebugDrawModeFlag &= ~flag : ModeManager::mPhysicsDebugDrawModeFlag |= flag;
							PhysicSystem3D::SetDebugMode((int)ModeManager::mPhysicsDebugDrawModeFlag);*/
						}
					}

					ImGui::EndMenu();
				}

				ImGui::TreePop();
			}
			
			if (ImGuiWrapper::TreeNodeExStyle1((void*)"Post Processing", "Post Processing"))
			{
				if (ImGui::Button("Add Post Processing"))
					ImGui::OpenPopup("AddPostProcessing");

				if (ImGui::BeginPopup("AddPostProcessing"))
				{
					constexpr auto postTypes = magic_enum::enum_values<PostProcessingType>();

					// Skip None and MSAA
					for (size_t i = 2; i < postTypes.size(); i++)
					{
						if (ImGui::MenuItem(magic_enum::enum_name(postTypes[i]).data()))
						{
							m_RenderPass->AddPostProcessing(postTypes[i]);
							ImGui::CloseCurrentPopup();
						}
					}

					ImGui::EndPopup();
				}
				
				for (size_t i = 1; i < m_RenderPass->mPostProcessings.size(); i++)
				{
					ImGui::Selectable(magic_enum::enum_name(m_RenderPass->mPostProcessings[i]->mType).data());

					// imgui demo: Drag to reorder items (simple)
					if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
					{
						int next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
						if (next >= 1 && next < m_RenderPass->mPostProcessings.size())
						{
							PostProcessingType tempType = m_RenderPass->mPostProcessings[i]->mType;
							m_RenderPass->mPostProcessings[i]->mType = m_RenderPass->mPostProcessings[next]->mType;
							m_RenderPass->mPostProcessings[next]->mType = tempType;
							ImGui::ResetMouseDragDelta();
						}
					}

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
							m_RenderPass->mPostProcessings.erase(m_RenderPass->mPostProcessings.begin() + i);

						ImGui::EndPopup();
					}
				}

				ImGui::TreePop();
			}
			
			ImGui::End();
		}
		if (bShowSceneSettings)
		{
			m_SceneSettingsPanel.OnImGuiRender(&bShowSceneSettings);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		//TODO: remove
		ConfigManager::m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
		textureID = m_RenderPass->ExcuteAndReturnFinalTex();
		//ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		ImGui::Image((void*)(intptr_t)textureID, ImVec2{ ConfigManager::m_ViewportSize.x, ConfigManager::m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(path);
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Camera
			// Runtime camera from entity
			/*auto cameraEntity = mActiveScene->GetPrimaryCameraEntity();
			const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			const glm::mat4& cameraProjection = camera.GetProjection();
			glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());*/

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		static int transCorner = 1;
		ImGuiDockNode* node = ImGui::GetWindowDockNode();
		node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

		Utils::SceneToolbar(node, 10.0f, &transCorner, [&](int* corner, const ImVec2& work_area_size, const ImGuiWindowFlags m_window_flags) {

			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

			if (ImGui::Begin("SRT Toolbar", &bShowSRT, m_window_flags))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.1f, 0.1f, 0.1f, 0.5f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

				if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("TransIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
				{
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("RotateIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), m_GizmoType == ImGuizmo::OPERATION::ROTATE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
				{
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("ScaleIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
				{
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}

				ImGui::PopStyleColor(3);

			}

			ImGui::End();
			ImGui::PopStyleVar();
			});
		//====END Viewport Window======
		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();


		
		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
		bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
		bool hasPauseButton = m_SceneState != SceneState::Edit;

		if (hasPlayButton)
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;

			//TODO:added
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

			//if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}

		/*TODO:if (hasSimulateButton)
		{
			if (hasPlayButton)
				ImGui::SameLine();

			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}*/
		if (hasPauseButton)
		{
			bool isPaused = m_ActiveScene->IsPaused();
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = m_IconPause;
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					m_ActiveScene->SetPaused(!isPaused);
				}
			}

			// Step button
			if (isPaused)
			{
				ImGui::SameLine();
				{
					Ref<Texture2D> icon = m_IconStep;
					bool isPaused = m_ActiveScene->IsPaused();
					if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
					{
						m_ActiveScene->Step();
					}
				}
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::OnEntityDeleted(Entity e)
	{
		SelectionManager::Deselect(SelectionContext::Scene, e.GetUUID());
	}

	
	
	void EditorLayer::OnEvent(Event& e)
	{
		//TODO: Panel stuffs
		m_EditorConsolePanel->OnEvent(e);

		m_SceneRendererPanel->OnEvent(e);

		m_CameraController.OnEvent(e);
		if (m_SceneState == SceneState::Edit)
		{
			m_EditorCamera.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(AS_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(AS_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}
	/*
	void EditorLayer::OnEvent(Event& e)
	{
		//TODO:Call all panel onEvent: AssetEditorPanel::OnEvent(e);
		m_SceneHierarchyPanel.OnEvent(e);

		if (m_SceneState == SceneState::Edit)
		{
			if (m_AllowViewportCameraEvents)
				m_EditorCamera.OnEvent(e);
			/*TODO:
			//if (m_ViewportPanel2MouseOver)
				//m_SecondEditorCamera.OnEvent(e);
		}
		else if (m_SceneState == SceneState::Simulate)
		{
			if (m_AllowViewportCameraEvents)
				m_EditorCamera.OnEvent(e);
		}

		//m_CurrentScene->OnEvent(e);

		EventDispatcher dispatcher(e);
		//TODO:
		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) { return OnKeyPressed(event); });
		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event) { return OnMouseButtonPressed(event); });
		
		dispatcher.Dispatch<WindowTitleBarHitTestEvent>([this](WindowTitleBarHitTestEvent& event)
			{
				event.SetHit(UI_TitleBarHitTest(event.GetX(), event.GetY()));
				return true;
			});
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event)
			{
				if ((m_SceneState == SceneState::Play) || (m_SceneState == SceneState::Pause))
					OnSceneStop();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStopSimulation();

				return false; // give other things a chance to react to window close
			});

		dispatcher.Dispatch<EditorExitPlayModeEvent>([this](EditorExitPlayModeEvent& event)
			{
				if ((m_SceneState == SceneState::Play) || (m_SceneState == SceneState::Pause))
					OnSceneStop();
				return true;
			});
	}

	*/


	void EditorLayer::RegenerateProjectScriptSolution(const std::filesystem::path& projectPath)
	{
		std::string batchFilePath = projectPath.string();
		std::replace(batchFilePath.begin(), batchFilePath.end(), '/', '\\'); // Only windows
		batchFilePath += "\\Win-GenScriptProject.bat";//TODO: path should be fixed
		system(batchFilePath.c_str());
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				OpenProject();

			break;
		}
		case Key::S:
		{
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}

			break;
		}

		// Scene Commands
		case Key::D:
		{
			if (control)
				OnDuplicateEntity();

			break;
		}

		// Gizmos
		case Key::Q:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = -1;
			break;
		}
		case Key::W:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::R:
		{
			if (control)
			{
				ScriptEngine::ReloadAssembly();
			}
			else
			{
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
			break;
		}
		case Key::Delete:
		{
			if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
			{
				Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
				if (selectedEntity)
				{
					m_SceneHierarchyPanel.SetSelectedEntity({});
					m_ActiveScene->DestroyEntity(selectedEntity);
				}
			}
			break;
		}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
				return;

			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.Translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.001f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			// Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
				}
			}
		}

		// Draw selected entity outline 
		if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
		{
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::NewProject()
	{
		Project::New();
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		auto& activeProject = Project::Load(path);
		if (activeProject)
		{
			//TODO:Load script engine after project::load finished
			auto scriptConfig = Application::Get().GetSpecification().ScriptConfig;
			ScriptEngine::Init(scriptConfig);

			// TODO:Open app assembly alone
			auto appAssemblyPath = Project::GetScriptModuleFilePath();
			AS_CORE_WARN("APP ASSEMBLY {0}", appAssemblyPath.string());
			if (!appAssemblyPath.empty())
			{
				if (!FileSystem::Exists(appAssemblyPath))
				{
					RegenerateProjectScriptSolution(Project::GetProjectDirectory());
					ScriptBuilder::BuildScriptAssembly(activeProject);
				}

				ScriptEngine::LoadAppAssembly();
			}
			else
			{
				AS_CONSOLE_LOG_WARN("No C# assembly has been provided in the Project Settings, or it wasn't found. Please make sure to build the Visual Studio Solution for this project if you want to use C# scripts!");
				std::string path = appAssemblyPath.string();
				if (path.empty())
					path = "<empty>";
				AS_CONSOLE_LOG_WARN("App Assembly Path = {}", path);
			}

			auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath);
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
			//TODO:
			m_EditorConsolePanel = CreateScope<EditorConsolePanel>();
			m_SceneRendererPanel = CreateScope<SceneRendererPanel>();
		}
	}

	bool EditorLayer::OpenProject()
	{
		//TODO: Change default project path
		std::string filepath;// = FileDialogs::OpenFile("Aspect Project (*.aproj)\0*.aproj\0");
		filepath = AssetManager::GetFullStringPath("SandboxProject/Sandbox.aproj");
		filepath = "SandboxProject/Sandbox.aproj";

		if (filepath.empty())
			return false;

		OpenProject(filepath);
		return true;
	}

	void EditorLayer::SaveProject()
	{
		//Project::SaveActive();
	}

	void EditorLayer::EmptyProject()
	{
		/*TODO:
		if (Project::GetActive())
			CloseProject();

		Ref<Project> project = Ref<Project>::Create();
		Project::SetActive(project);

		m_PanelManager->OnProjectChanged(project);
		NewScene();

		SelectionManager::DeselectAll();

		FileSystem::StartWatching();

		// Reset cameras
		m_EditorCamera = EditorCamera(45.0f, 1280.0f, 720.0f, 0.1f, 1000.0f);
		m_SecondEditorCamera = EditorCamera(45.0f, 1280.0f, 720.0f, 0.1f, 1000.0f);

		memset(s_ProjectNameBuffer, 0, MAX_PROJECT_NAME_LENGTH);
		memset(s_OpenProjectFilePathBuffer, 0, MAX_PROJECT_FILEPATH_LENGTH);
		memset(s_NewProjectFilePathBuffer, 0, MAX_PROJECT_FILEPATH_LENGTH);*/
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = AspectRef<Scene>::Create();
		m_ActiveScene->OnViewportResize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Aspect Scene (*.aspect)\0*.aspect\0");
		
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		AS_CORE_INFO("Start Open Scene... {0}" + path.string());

		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
			m_SceneState = SceneState::Edit;
		}

		if (path.extension().string() != ".aspect")
		{
			AS_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		// NOTE(Peter): We set the scene context to nullptr here to make sure all old script entities have been destroyed
		ScriptEngine::SetSceneContext(nullptr, nullptr);

		AspectRef<Scene> newScene = AspectRef<Scene>::Create();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;

			//TODO:
			m_CurrentScene = m_EditorScene;
		}

	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Aspect Scene (*.aspect)\0*.aspect\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(AspectRef<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		//m_ActiveScene = Scene::Copy(m_EditorScene);[Obsolete] Entity copy has some errors
		m_ActiveScene = AspectRef<Scene>::Create();
		m_EditorScene->CopyTo(m_ActiveScene);

		AS_CORE_ERROR("Start Play");
		//TODO: Set scene context for script engine when runtime
		// setting context is be ahead of onRuntimeStart
		ScriptEngine::SetSceneContext(m_ActiveScene, nullptr);

		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		AS_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(true);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			Entity newEntity = m_EditorScene->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}
	
}
