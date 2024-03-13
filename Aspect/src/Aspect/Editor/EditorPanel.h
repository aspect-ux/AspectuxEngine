#pragma once

#include "Aspect/Core/Ref.h"
#include "Aspect/Scene/Scene.h"
#include "Aspect/Project/Project.h"
#include "Aspect/Events/Event.h"

namespace Aspect {

	class EditorPanel : public RefCounted
	{
	public:
		virtual ~EditorPanel() = default;

		virtual void OnImGuiRender(bool& isOpen) = 0;
		virtual void OnEvent(Event& e) {}
		virtual void OnProjectChanged(const Ref<Project>& project) {}
		virtual void SetSceneContext(const AspectRef<Scene>& context) {}
	};

}
