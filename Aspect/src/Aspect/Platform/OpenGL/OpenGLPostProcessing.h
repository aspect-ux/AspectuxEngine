#pragma once

#include "Aspect/Renderer/PostProcessing.h"

namespace Aspect
{
	class OpenGLPostProcessing : public PostProcessing
	{
	public:
		OpenGLPostProcessing(const PostProcessingType& type) : PostProcessing(type) {}
	public:
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<FrameBuffer>& fb) override;
	private:
		uint32_t DoMSAA(const Ref<FrameBuffer>& fb);
		uint32_t DoPostWithShader(const Ref<FrameBuffer>& fb, const Ref<class Shader>& shader);
	};
}
