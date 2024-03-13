#pragma once

#include "Aspect/Editor/EditorPanel.h"
#include "Aspect/Renderer/SceneRenderer.h"

namespace Aspect {

	class SceneRendererPanel : public EditorPanel
	{
	public:
		SceneRendererPanel() = default;
		virtual ~SceneRendererPanel() = default;

		void SetContext(const AspectRef<SceneRenderer>& context) { m_Context = context; }
		virtual void OnImGuiRender(bool& isOpen) override;
	private:
		AspectRef<SceneRenderer> m_Context;
	};

}
