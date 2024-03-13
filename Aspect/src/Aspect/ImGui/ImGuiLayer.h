#pragma once

#include "Aspect/Core/Layer.h"

#include "Aspect/Events/ApplicationEvent.h"
#include "Aspect/Events/KeyEvent.h"
#include "Aspect/Events/MouseEvent.h"

namespace Aspect
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const std::string& name);
		~ImGuiLayer();

		void OnEvent(Event& e) override;

		virtual void Begin() {};
		virtual void End() {};
		virtual void OnImGuiRender() {}

		//TODO:fix 
		virtual uint32_t GetActiveWidgetID() const { return 0;}

		static ImGuiLayer* Create();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
	};
}
