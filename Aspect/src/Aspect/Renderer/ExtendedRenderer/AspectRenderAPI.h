/**
 * @package Aspect (Hazel to Morava)
 * @author  Yan Chernikov (TheCherno)
 * @licence Apache License 2.0
 */

#pragma once

//#include "Aspect/Renderer/RendererCapabilities.h"
//#include "Aspect/Renderer/RendererTypesAspect.h"
#include "Aspect/Renderer/SceneEnvironment.h"
#include "Aspect/Renderer/VertexArray.h"

#include <cstdint>
#include <string>
#include "Aspect/Renderer/RenderPass.h"


namespace Aspect
{

	enum class RendererAPITypeAspect
	{
		None,
		Vulkan,
		OpenGL,
		DX11,
	};

	enum class PrimitiveTypeAspect
	{
		None = 0, Triangles, Lines
	};

	class PipelineAspect;
	class MaterialAspect;
	class MeshAspect;
	class RenderPassAspect;


	class AspectRendererAPI
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginRenderPass(const AspectRef<RenderPass>& renderPass) = 0;
		virtual void EndRenderPass() = 0;
		virtual void SubmitFullscreenQuad(AspectRef<PipelineAspect> pipeline, AspectRef<MaterialAspect> material) = 0;
		// virtual void SubmitQuad(Ref<HazelMaterial> material, const glm::mat4& transform) = 0;

		virtual void SetSceneEnvironment(AspectRef<Environment> environment, AspectRef<Image2D> shadow) = 0;

		virtual void RenderMesh(AspectRef<PipelineAspect> pipeline, AspectRef<MeshAspect> mesh, const glm::mat4& transform) = 0;
		virtual void RenderMeshWithoutMaterial(AspectRef<PipelineAspect> pipeline, AspectRef<MeshAspect> mesh, const glm::mat4& transform) = 0;
		virtual void RenderQuad(AspectRef<PipelineAspect> pipeline, AspectRef<MaterialAspect> material, const glm::mat4& transform) = 0;

		virtual void DrawIndexed(uint32_t indexCount, PrimitiveTypeAspect type, bool depthTest = true) = 0;
		virtual void DrawLines(AspectRef<VertexArray> vertexArray, uint32_t vertexCount) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual std::pair<AspectRef<TextureCube>, AspectRef<TextureCube>> CreateEnvironmentMap(const std::string& filepath) = 0;

		//virtual RendererCapabilities& GetCapabilities() = 0;

		static void Clear(float r, float g, float b, float a);
		static void SetClearColor(float r, float g, float b, float a);
		static void SetLineThickness(float thickness);

		static RendererAPITypeAspect Current() { return s_CurrentRendererAPI; }
		static void SetAPI(RendererAPITypeAspect api);

	private:
		static void LoadRequiredAssets();

	private:
		inline static RendererAPITypeAspect s_CurrentRendererAPI = RendererAPITypeAspect::Vulkan;

	};

}
