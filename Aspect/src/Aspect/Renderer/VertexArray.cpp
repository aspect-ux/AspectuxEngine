#include "aspch.h"
#include "VertexArray.h"
#include "Aspect/Renderer/Renderer.h"
#include "Aspect/Platform/OpenGL/OpenGLVertexArray.h"

namespace Aspect
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::OpenGL: return CreateRef<OpenGLVertexArray>();
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
