#include "aspch.h"
#include "UniformBufferSet.h"

#include "Aspect/Renderer/Renderer.h"

#include "Aspect/Platform/OpenGL/OpenGLUniformBufferSet.h"

#include "Aspect/Renderer/RendererAPI.h"

namespace Aspect {

	Ref<UniformBufferSet> UniformBufferSet::Create(uint32_t frames)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:     return nullptr;
			case RendererAPIType::OpenGL:  return CreateRef<OpenGLUniformBufferSet>(frames);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
