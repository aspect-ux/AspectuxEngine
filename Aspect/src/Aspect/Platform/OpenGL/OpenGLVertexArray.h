#pragma once

#include "Aspect/Renderer/VertexArray.h"

namespace Aspect
{
	class OpenGLVertexArray : public VertexArray 
	{
	public:
		OpenGLVertexArray();

		virtual ~OpenGLVertexArray();

		virtual void Bind()const override;
		virtual void Unbind()const override;

		virtual const void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual const void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() override { return m_VertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() override { return m_IndexBuffer; }

	private:
		uint32_t m_RendererId;
		uint32_t m_VertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
