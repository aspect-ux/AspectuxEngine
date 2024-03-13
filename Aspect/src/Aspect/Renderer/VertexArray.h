#pragma once

#include <memory>
#include "Aspect/Renderer/Buffer.h"


namespace Aspect
{
	class VertexArray 
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind()const = 0;
		virtual void Unbind()const = 0;

		virtual const void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)  = 0;
		virtual const void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() = 0;

		static Ref<VertexArray> Create();
	};
}
