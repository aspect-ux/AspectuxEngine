#pragma once

#include "Aspect/Renderer/OrthographicCamera.h"
#include "Aspect/Core/Timestep.h"

#include "Aspect/Events/ApplicationEvent.h"
#include "Aspect/Events/MouseEvent.h"

namespace Aspect
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio,bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; CaculateView(); }

		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
	private:
		void CaculateView();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f; // 摄像机的拉近和拉远

		OrthographicCameraBounds m_Bounds; // Bound需要在Camera前面，不然会先初始化camera，最终图像还没渲染出来就初始化了相机于是初始只有黑屏
		OrthographicCamera m_Camera;

		bool m_Rotation;
		

		glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
		float m_CameraRotation = 0.0f;

		float m_CameraTranslationSpeed = 1.0f, m_CameraRotationSpeed = 180.0f;

	};
}
