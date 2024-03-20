#pragma once

#include "Aspect/Editor/EditorPanel.h"
#include "Aspect/Scene/Entity.h"
#include "Aspect/Renderer/Texture.h"

namespace Aspect {

	class MaterialsPanel : public EditorPanel
	{
	public:
		MaterialsPanel();
		~MaterialsPanel();

		virtual void SetSceneContext(const AspectRef<Scene>& context) override;
		virtual void OnImGuiRender(bool& isOpen) override;

	private:
		void RenderMaterial(size_t materialIndex, AssetHandle materialAssetHandle){}

	private:
		AspectRef<Scene> m_Context;
		Entity m_SelectedEntity;
		Ref<Texture2D> m_CheckerBoardTexture;
	};

}
