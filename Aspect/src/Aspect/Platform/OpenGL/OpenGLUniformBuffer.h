#pragma once

#include "Aspect/Renderer/UniformBuffer.h"

namespace Aspect {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		//TODO:
		virtual void RT_SetData(const void* data, uint32_t size, uint32_t offset = 0) override {};

		virtual uint32_t GetBinding() const override { return 0; };



		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:
		uint32_t m_RendererID = 0;
	};
}
