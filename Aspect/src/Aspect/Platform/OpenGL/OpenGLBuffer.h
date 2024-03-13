#pragma once

#include "Aspect/Renderer/Buffer.h"

namespace Aspect
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		OpenGLVertexBuffer(void* vertices, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind()const override;
		virtual void Unbind()const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		virtual void SetData(const void* data, uint32_t size) override;

		//static VertexBuffer* Create(float* vertices, uint32_t size);

	private:
		uint32_t m_RendererID;
		VertexBufferUsage m_Usage;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		OpenGLIndexBuffer(uint32_t count);
		OpenGLIndexBuffer(void* data, uint32_t size = 0);//TODO:fix
		virtual ~OpenGLIndexBuffer();
		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void SetData(const void* data, uint32_t count) override;

		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Size = 0;
		uint32_t m_Count;
	};
}
