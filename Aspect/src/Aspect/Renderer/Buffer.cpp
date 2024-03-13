#include "aspch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Aspect/Platform/OpenGL/OpenGLBuffer.h"
//#include "Aspect/Core/Ref.h"

namespace Aspect
{
	/*Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}*/

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices,size);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	//TODO:
	Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLVertexBuffer>(data, size, usage);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size, usage);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	//======================indexBuffer============================
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	//TODO:
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLIndexBuffer>(size);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t size)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLIndexBuffer>(data, size);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
