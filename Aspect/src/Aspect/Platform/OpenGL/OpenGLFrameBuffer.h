#pragma once

#include "Aspect/Renderer/FrameBuffer.h" 
namespace Aspect
{
	/*class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind() override;

		//TODO:
		virtual void BindReadFrameBuffer() override;
		virtual void BindDrawFrameBuffer() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { AS_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		//TODO:
		virtual Ref<class Texture3D> GetDepthTex3D() const override; // have bugs
		virtual void BindDepthTex3D(uint32_t slot) override;
		virtual void UnBindDepthTex3D(uint32_t slot) override;
	private:
		uint32_t m_RendererID = 0;
		FrameBufferSpecification m_Specification;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
		FrameBufferTextureSpecification m_DepthAttachmentSpecification = FrameBufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};*/


	class OpenGLFrameBuffer : public FrameBuffer
	{
		friend class OpenGLPostProcessing;
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual void BindReadFrameBuffer() override;
		virtual void BindDrawFrameBuffer() override;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { AS_CORE_ASSERT(index < mColorAttachments.size()); return mColorAttachments[index]; }
		virtual uint32_t GetDepthAttachmentRendererID() const override { return mDepthAttachment; }

		virtual const FrameBufferSpecification& GetSpecification() const override { return mSpecification; }

		virtual void FrameBufferTexture2D(uint32_t cubemapIndex, uint32_t cubemapID, uint32_t slot = 0) override;

		virtual Ref<class Texture3D> GetDepthTex3D() const override; // have bugs
		virtual void BindDepthTex3D(uint32_t slot) override;
		virtual void UnBindDepthTex3D(uint32_t slot) override;
	private:
		uint32_t mRendererID = 0;
		FrameBufferSpecification mSpecification;

		std::vector<FrameBufferTextureSpecification> mColorAttachmentSpecifications;
		FrameBufferTextureSpecification mDepthAttachmentSpecification = FrameBufferTextureFormat::None;

		std::vector<uint32_t> mColorAttachments;
		uint32_t mDepthAttachment = 0;
	};
}
