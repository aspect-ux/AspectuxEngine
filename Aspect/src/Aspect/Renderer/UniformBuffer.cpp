#include "aspch.h"
#include "UniformBuffer.h"

#include "Aspect/Renderer/Renderer.h"
#include "Aspect/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Aspect {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPIType::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
