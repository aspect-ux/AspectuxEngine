#include "aspch.h"

#include "Aspect/Renderer/PostProcessing.h"
#include "Aspect/Renderer/RenderCommand.h"
#include "Aspect/Asset/ConfigManager/ConfigManager.h"

namespace Aspect
{
	Ref<VertexArray>  PostProcessing::mScreenQuadVAO = nullptr;
	Ref<VertexBuffer> PostProcessing::mScreenQuadVBO = nullptr;
	Ref<IndexBuffer>  PostProcessing::mScreenQuadIBO = nullptr;
	Ref<FrameBuffer>  PostProcessing::mFramebuffer = nullptr;
	Ref<Texture2D>    PostProcessing::mIntermediateScreenTex = nullptr;

	PostProcessing::PostProcessing(const PostProcessingType& type)
		: mType{ type }
	{
	}

	void PostProcessing::Init()
	{
		AS_CORE_WARN("PostProcessing Init...");
		mScreenQuadVAO = VertexArray::Create();
		float screenQuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		uint32_t screenQuadIndices[] = {
			0, 1, 2, 0, 2, 3
		};
		mScreenQuadIBO = IndexBuffer::Create(screenQuadIndices, 6);
		mScreenQuadVBO = VertexBuffer::Create(screenQuadVertices, sizeof(screenQuadVertices));
		mScreenQuadVBO->SetLayout({
			{ ShaderDataType::Float2,	"a_Position" },
			{ ShaderDataType::Float2,	"a_TexCoord" },
			});
		mScreenQuadVAO->AddVertexBuffer(mScreenQuadVBO);
		mScreenQuadVAO->SetIndexBuffer(mScreenQuadIBO);

		FrameBufferSpecification fbSpec;
		fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8 };
		fbSpec.Width = ConfigManager::m_ViewportSize.x;
		fbSpec.Height = ConfigManager::m_ViewportSize.y;
		mFramebuffer = FrameBuffer::Create(fbSpec);
	}

	void PostProcessing::DoPostProcessing()
	{
		RenderCommand::DrawIndexed(mScreenQuadVAO);
	}
}
