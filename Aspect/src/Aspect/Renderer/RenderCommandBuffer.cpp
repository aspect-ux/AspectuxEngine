#include "aspch.h"
#include "RenderCommandBuffer.h"

//#include "Aspect/Platform/Vulkan/VulkanRenderCommandBuffer.h"

#include "Aspect/Renderer/RendererAPI.h"

namespace Aspect {

	Ref<RenderCommandBuffer> RenderCommandBuffer::Create(uint32_t count, const std::string& debugName)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    return nullptr;
		//case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(count, debugName);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<RenderCommandBuffer> RenderCommandBuffer::CreateFromSwapChain(const std::string& debugName)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    return nullptr;
		//case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(debugName, true);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
