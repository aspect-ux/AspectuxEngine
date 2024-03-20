#pragma once

#include "Aspect.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneSettingsPanel.h"
#include "Aspect/Renderer/RenderPass.h"

#include "Aspect/Editor/EditorConsolePanel.h"
#include "Aspect/Editor/SceneRendererPanel.h"

#include "Aspect/Renderer/ExtendedRenderer/EditorRenderer.h"

namespace Aspect
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Aspect::Timestep ts) override;
		void OnImGuiRender() override;

		void OnEvent(Aspect::Event& e) override;
		void RegenerateProjectScriptSolution(const std::filesystem::path& projectPath);
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void OnOverlayRender();

		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();
		void EmptyProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(AspectRef<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		void OnScenePause();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();

		// callbacks
		void OnEntityDeleted(Entity e);
	private:
		//Ref<UserPreferences> m_UserPreferences;

		//Scope<PanelManager> m_PanelManager;
		bool m_ShowStatisticsPanel = false;

		std::vector<std::function<void()>> m_PostSceneUpdateQueue;

		Aspect::OrthographicCameraController m_CameraController; // 使用controller来控制相机

		// Temp
		Ref<RenderPass> m_RenderPass;
		Aspect::Ref<Aspect::VertexArray> m_SquareVA;
		Aspect::Ref<Aspect::Shader> m_FlatColorShader;
		Aspect::Ref<Aspect::Texture2D> m_Checkerboard;
		Aspect::Ref<Aspect::Texture2D> m_SpriteSheet;

		Aspect::Ref<Aspect::SubTexture2D> m_TextureStair, m_TextureBush, m_TextureTree;

		struct ProfileResult
		{
			const char* Name;
			float Time;
		};
		std::vector<ProfileResult> m_ProfileResults;

		glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0 };
		glm::vec3 m_SquarePosition;

		glm::vec2 m_ViewportSize = { 1280.0f,720.0f };

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		std::unordered_map<char, Aspect::Ref<Aspect::SubTexture2D>> s_TextureMap;// map数据结构

		uint32_t m_MapWidth, m_MapHeight;

		Aspect::Ref<Aspect::FrameBuffer> m_FrameBuffer;

		// ECS for scene and camera
		AspectRef<Scene> m_RuntimeScene, m_EditorScene, m_SimulationScene, m_CurrentScene, m_ActiveScene;
		AspectRef<SceneRenderer> m_ViewportRenderer;
		AspectRef<SceneRenderer> m_SecondViewportRenderer;
		AspectRef<SceneRenderer> m_FocusedRenderer;

		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;

		glm::vec2 m_ViewportBounds[2];

		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Gizmo
		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		SceneSettingsPanel m_SceneSettingsPanel;
		Scope<ContentBrowserPanel> m_ContentBrowserPanel;
		Scope<EditorConsolePanel> m_EditorConsolePanel;
		Scope<SceneRendererPanel> m_SceneRendererPanel;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop, m_CheckerboardTexture;



		// ImGui Tools
		bool m_ShowMetricsTool = false;
		bool m_ShowStackTool = false;
		bool m_ShowStyleEditor = false;

		bool m_StartedCameraClickInViewport = false;

		bool m_ShowBoundingBoxes = false;
		bool m_ShowBoundingBoxSelectedMeshOnly = true;
		bool m_ShowBoundingBoxSubmeshes = false;

		bool m_ShowIcons = true;
		bool m_ShowGizmos = true;
		bool m_ShowGizmosInPlayMode = false;

		bool m_ViewportPanelMouseOver = false;
		bool m_ViewportPanelFocused = false;
		bool m_AllowViewportCameraEvents = false;

		bool m_ViewportPanel2MouseOver = false;
		bool m_ViewportPanel2Focused = false;

		bool m_ShowSecondViewport = false;

		bool m_EditorCameraInRuntime = false;


		enum class TransformationTarget { MedianPoint, IndividualOrigins };
		TransformationTarget m_MultiTransformTarget = TransformationTarget::MedianPoint;

		struct LoadAutoSavePopupData
		{
			std::string FilePath;
			std::string FilePathAuto;
		} m_LoadAutoSavePopupData;

		//TODO: to be removed
		AspectRef<EditorRenderer> editorRenderer;
	};
}
