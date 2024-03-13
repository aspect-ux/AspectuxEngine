#pragma once

#include "Aspect/Renderer/Framebuffer.h"
#include "Aspect/Renderer/VertexArray.h"
#include "Aspect/Renderer/Texture.h"

namespace Aspect
{
	enum class PostProcessingType
	{
		None = 0,
		MSAA = 1,
		Outline,
		Cartoon,
		GrayScale,
		GaussianBlur,
		FxaaConsole,
	};

	class PostProcessing
	{
	public:
		PostProcessing(const PostProcessingType& type);
		virtual ~PostProcessing() {}
	public:
		static void Init();
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<FrameBuffer>& fb) { return 0; };
	protected:
		void DoPostProcessing();
	public:
		PostProcessingType mType;
		static Ref<FrameBuffer> mFramebuffer;
		static Ref<Texture2D> mIntermediateScreenTex;
	protected:
		static Ref<VertexArray> mScreenQuadVAO;
		static Ref<VertexBuffer> mScreenQuadVBO;
		static Ref<IndexBuffer> mScreenQuadIBO;
	};
}
