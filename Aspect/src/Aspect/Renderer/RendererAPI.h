#pragma once

#include <glm/glm.hpp>
#include "VertexArray.h"

namespace Aspect
{
	enum class RendererAPIType
	{
		None = 0,
		OpenGL = 1,
		DX11,
		DX12,
		Vulkan
	};

	//stencil func and op
	enum class StencilFunc
	{
		ALWAYS,
		NOTEQUAL
	};

	enum class StencilOp
	{
		KEEP,
		INCREMENT,
		DECREMENT
	};

	enum class DepthComp
	{
		EQUAL,
		LEQUAL,
		LESS
	};

	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

		virtual void SetLineWidth(float width) = 0;

		//TODO: to be moved with rendercommand
		//virtual void RenderMesh(const glm::mat4& transform, const glm::vec3& cameraPos, Ref<Shader> shader, int entityID = -1);

		inline static RendererAPIType Current() { return s_CurrentRendererAPI; }
		//inline static API GetAPI() { return s_API; }


		//depth
		virtual void DepthTest(bool enable) = 0;
		virtual void DepthMask(bool maskFlag) = 0;
		virtual void DepthFunc(DepthComp comp) = 0;

		//blend
		virtual void Blend(int32_t Bit) = 0;

		virtual void BindTexture(int32_t slot, uint32_t textureID) = 0;

		//stencil
		virtual void SetStencilFunc(StencilFunc stencilFunc, int32_t ref, int32_t mask) = 0;
		virtual void SetFrontOrBackStencilOp(int32_t FrontOrBack, StencilOp stencilFail, StencilOp depthFail, StencilOp depthSuccess) = 0;
		virtual void StencilTest(int32_t Bit) = 0;
		virtual void ClearStencil() = 0;
		virtual void StencilMask(uint32_t mask) = 0;

		virtual void BindFrameBuffer(uint32_t framebufferID) = 0;

		//cull
		virtual void Cull(int32_t Bit) = 0;
		virtual void CullFrontOrBack(bool bFront) = 0;

		virtual int GetDrawFrameBuffer() = 0;
	private:
		static RendererAPIType s_CurrentRendererAPI; 
	};
}
