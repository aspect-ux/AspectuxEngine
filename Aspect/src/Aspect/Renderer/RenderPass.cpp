#include "aspch.h"
#include "RenderPass.h"

#include "Renderer.h"
#include "Aspect/Platform/OpenGL/OpenGLRenderPass.h"

#include "Aspect/Renderer/RendererAPI.h"
#include "Aspect/Platform/OpenGL/OpenGLPostProcessing.h"

namespace Aspect {

	/*Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLRenderPass>(spec);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}*/


	void RenderPass::AddPostProcessing(PostProcessingType type)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return;
		case RendererAPIType::OpenGL:  mPostProcessings.emplace_back(CreateScope<OpenGLPostProcessing>(type)); return;
	/*	case RendererAPIType::Vulkan:  return;
		case RendererAPIType::DX11:    return;
		case RendererAPIType::DX12:    return;*/
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return;
	}

	uint32_t RenderPass::ExcuteAndReturnFinalTex()
	{
		//TODO:
		TextureSpecification ts;
		ts.Height = mSpecification.TargetFramebuffer->GetSpecification().Height;
		ts.Width = mSpecification.TargetFramebuffer->GetSpecification().Width;
		PostProcessing::mIntermediateScreenTex = Texture2D::Create(ts);


		uint32_t final = 0;
		for (auto& effect : mPostProcessings)
		{
			final = effect->ExcuteAndReturnFinalTex(mSpecification.TargetFramebuffer);
		}
		return final;
	}

}
