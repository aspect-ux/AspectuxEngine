#include "aspch.h"

#include "Aspect/Renderer/GraphicsContext.h"
#include "Aspect/Renderer/Renderer.h"
#include "Aspect/Platform/OpenGL/OpenGLContext.h"
//#include "Platform/Vulkan/VulkanContext.h"
//#include "Aspect/Platform/DirectX11/Dx11Context.h"

namespace Aspect
{
	Ref<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLContext>(static_cast<GLFWwindow*>(window));
		//case RendererAPIType::Vulkan:  return CreateRef<VulkanContext>(static_cast<GLFWwindow*>(window));
		//case RendererAPIType::DX11:    return CreateRef<Dx11Context>(static_cast<GLFWwindow*>(window));
		case RendererAPIType::DX12:    return nullptr;
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
