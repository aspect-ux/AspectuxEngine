#include "aspch.h"
#include "FrameBuffer.h"
#include "Renderer.h"

#include "Aspect/Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Aspect
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::OpenGL: return CreateRef<OpenGLFrameBuffer>(spec);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
