#include "aspch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "GL/GL.h"

namespace Aspect
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		AS_CORE_ASSERT(windowHandle, "WindowHandle is null");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AS_CORE_ASSERT(status, "Failed to initialize Glad!");

		 //GPU–≈œ¢
		/*
		AS_CORE_INFO("OpenGL Info:");
		AS_CORE_INFO(" Vendor:{0}", glGetString(GL_VENDOR));
		AS_CORE_INFO(" Renderer:{0}", glGetString(GL_RENDERER));
		AS_CORE_INFO(" Version:{0}", glGetString(GL_VERSION));*/
		
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
