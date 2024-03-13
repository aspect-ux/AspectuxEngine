#pragma once

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Events/Event.h"
#include "Aspect/Core/Timestep.h"
#include <string>

namespace Aspect
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& event) {}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}
