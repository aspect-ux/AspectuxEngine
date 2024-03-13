#include "aspch.h"
#include "RenderCommand.h"

#include "Aspect/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Aspect
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = std::make_unique<OpenGLRendererAPI>();
}
