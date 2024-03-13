#pragma once

#include "Aspect/Core/Layer.h"
#include "Aspect/Events/ApplicationEvent.h"
#include "Aspect/Events/MouseEvent.h"
#include "Aspect/Events/KeyEvent.h"
#include "Aspect/ImGui/ImGuiLayer.h"

namespace Aspect
{
	class OpenGLImGuiLayer : public ImGuiLayer
	{
	public:
		OpenGLImGuiLayer();
		//OpenGLImGuiLayer(const std::string& name) : ImGuiLayer(name) {}
		~OpenGLImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		//virtual void OnUpdate() override;

		//virtual void OnImGuiRender() override;
		void Begin();
		void End();

		//void OnEvent(Event& event);

		//void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();

		virtual uint32_t GetActiveWidgetID() const override;
	/*
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMoveEvent(MouseMoveEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);

		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizedEvent(WindowResizeEvent& e);*/
	private:
		float m_Time = 0.0f;

		//bool m_BlockEvents = true;
	};
}
