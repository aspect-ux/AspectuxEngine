#include "aspch.h"
#include "Aspect/Renderer/OrthographicCamera.h"
#include "OrthographicCameraController.h"
#include "Aspect/Core/Input.h"
#include "Aspect/Core/KeyCode.h"

namespace Aspect
{
	Aspect::OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }), m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top), m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(Key::D))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;  // 乘以timestep,

		if (Input::IsKeyPressed(Key::A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;

		if (Aspect::Input::IsKeyPressed(Aspect::Key::W))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;

		if (Aspect::Input::IsKeyPressed(Aspect::Key::S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;


		if (m_Rotation)
		{
			if (Aspect::Input::IsKeyPressed(Aspect::Key::Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;

			if (Aspect::Input::IsKeyPressed(Aspect::Key::E))
				m_CameraPosition -= m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;  // 使得相机移动速度，随着相机离物体的远近改变而改变
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		// onEvent触发后，绑定事件，设置handled  相机实时更新的数据
		dispatcher.Dispatch<MouseScrolledEvent>(AS_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(AS_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		//TODO:m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);

		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	void OrthographicCameraController::CaculateView()
	{
		AS_PROFILE_FUNCTION();
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		AS_PROFILE_FUNCTION();
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		CaculateView();
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		AS_PROFILE_FUNCTION();
		// 视口比/宽高比;当窗口变换，相机的视口比+VP也会随之变换
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}
