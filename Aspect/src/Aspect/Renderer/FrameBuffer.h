#pragma once

#include "Aspect/Core/Base/Base.h"

namespace Aspect
{
	enum class FrameBufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH32F_TEX3D,
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format)
			: TextureFormat(format) {}

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FrameBufferTextureSpecification> Attachments;
	};

	struct FrameBufferSpecification
	{
		float Scale = 1.0f;
		uint32_t Width = 0, Height = 0;
		FrameBufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};


	class FrameBuffer 
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void BindReadFrameBuffer() = 0;
		virtual void BindDrawFrameBuffer() = 0;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		[[nodiscard]] virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;

		//TODO:
		virtual void FrameBufferTexture2D(uint32_t cubemapIndex, uint32_t cubemapID, uint32_t slot = 0) = 0;
		[[nodiscard]] virtual Ref<class Texture3D> GetDepthTex3D() const = 0;
		virtual void BindDepthTex3D(uint32_t slot) = 0;
		virtual void UnBindDepthTex3D(uint32_t slot) = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
}
