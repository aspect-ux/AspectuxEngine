#include "aspch.h"
#include "EditorRenderer.h"
//#include "Aspect/Renderer/ExtendedRenderer/Material.inl"

namespace Aspect
{
	EditorRenderer::EditorRenderer(AspectRef<Scene> scene) {}
	void EditorRenderer::InitMaterials()
	{
		// Prepare Materials
		m_defaultMaterial->SetShader(Library<Shader>().GetInstance().Get("BasePBR"));
		//m_defaultMaterial->Set<Ref<Texture2D>>("albedoMap", Library<Texture2D>().GetInstance().GetDefaultTexture());
		//m_defaultMaterial.Set("albedoMap", glm::vec4(1.f, 1.f, 1.f, 1.f));
		//m_defaultMaterial.Set("u_Shininess", 100.0f);
		//m_defaultMaterial.Set<Texture>("u_DiffuseMap", nullptr);

		/* Empty Material */
		//m_emptyMaterial.SetShader(m_context.shaderManager[":Shaders\\Unlit.glsl"]);
		//m_emptyMaterial.Set("u_Diffuse", FVector4(1.f, 0.f, 1.f, 1.0f));
		//m_emptyMaterial.Set<OvRendering::Resources::Texture*>("u_DiffuseMap", nullptr);

		/* Grid Material */
		/*m_gridMaterial.SetShader(m_context.editorResources->GetShader("Grid"));
		m_gridMaterial.SetBlendable(true);
		m_gridMaterial.SetBackfaceCulling(false);
		m_gridMaterial.SetDepthTest(false);*/

		/* Camera Material */
		/*m_cameraMaterial.SetShader(m_context.shaderManager[":Shaders\\Lambert.glsl"]);
		m_cameraMaterial.Set("u_Diffuse", FVector4(0.0f, 0.3f, 0.7f, 1.0f));
		m_cameraMaterial.Set<OvRendering::Resources::Texture*>("u_DiffuseMap", nullptr);*/

		/* Light Material */
		/*m_lightMaterial.SetShader(m_context.editorResources->GetShader("Billboard"));
		m_lightMaterial.Set("u_Diffuse", FVector4(1.f, 1.f, 0.5f, 0.5f));
		m_lightMaterial.SetBackfaceCulling(false);
		m_lightMaterial.SetBlendable(true);
		m_lightMaterial.SetDepthTest(false);*/

		/* Stencil Fill Material */
		/*m_stencilFillMaterial.SetShader(m_context.shaderManager[":Shaders\\Unlit.glsl"]);
		m_stencilFillMaterial.SetBackfaceCulling(true);
		m_stencilFillMaterial.SetDepthTest(false);
		m_stencilFillMaterial.SetColorWriting(false);
		m_stencilFillMaterial.Set<OvRendering::Resources::Texture*>("u_DiffuseMap", nullptr);*/

		/* Texture Material */
		/*m_textureMaterial.SetShader(m_context.shaderManager[":Shaders\\Unlit.glsl"]);
		m_textureMaterial.Set("u_Diffuse", FVector4(1.f, 1.f, 1.f, 1.f));
		m_textureMaterial.SetBackfaceCulling(false);
		m_textureMaterial.SetBlendable(true);
		m_textureMaterial.Set<OvRendering::Resources::Texture*>("u_DiffuseMap", nullptr);*/

		/* Outline Material */
		/*m_outlineMaterial.SetShader(m_context.shaderManager[":Shaders\\Unlit.glsl"]);
		m_outlineMaterial.Set<OvRendering::Resources::Texture*>("u_DiffuseMap", nullptr);
		m_outlineMaterial.SetDepthTest(false);*/

		/* Gizmo Arrow Material */
		/*m_gizmoArrowMaterial.SetShader(m_context.editorResources->GetShader("Gizmo"));
		m_gizmoArrowMaterial.SetGPUInstances(3);
		m_gizmoArrowMaterial.Set("u_IsBall", false);
		m_gizmoArrowMaterial.Set("u_IsPickable", false);*/

		/* Gizmo Ball Material */
		/*m_gizmoBallMaterial.SetShader(m_context.editorResources->GetShader("Gizmo"));
		m_gizmoBallMaterial.Set("u_IsBall", true);
		m_gizmoBallMaterial.Set("u_IsPickable", false);*/

		/* Gizmo Pickable Material */
		/*m_gizmoPickingMaterial.SetShader(m_context.editorResources->GetShader("Gizmo"));
		m_gizmoPickingMaterial.SetGPUInstances(3);
		m_gizmoPickingMaterial.Set("u_IsBall", false);
		m_gizmoPickingMaterial.Set("u_IsPickable", true);*/

		// Picking Material 
		/*m_actorPickingMaterial.SetShader(m_context.shaderManager[":Shaders\\Unlit.glsl"]);
		m_actorPickingMaterial.Set("u_Diffuse", FVector4(1.f, 1.f, 1.f, 1.0f));
		m_actorPickingMaterial.Set<OvRendering::Resources::Texture*>("u_DiffuseMap", nullptr);
		m_actorPickingMaterial.SetFrontfaceCulling(false);
		m_actorPickingMaterial.SetBackfaceCulling(false);*/
	}
}
