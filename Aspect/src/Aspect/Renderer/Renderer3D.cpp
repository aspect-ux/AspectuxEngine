#include "aspch.h"

#include "Renderer3D.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "RendererAPI.h"
#include "UniformBuffer.h"
#include "Aspect/Library/UniformBufferLibrary.h"


namespace Aspect
{
	Ref<FrameBuffer> Renderer3D::lightFBO = nullptr;

	//TODO: Use this struct temporarily, using ptr should be fixed
	struct Renderer3DData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;

		Ref<Texture2D> WhiteTexture;
		Ref<Texture2D> BlackTexture;
		Ref<Texture2D> BRDFLutTexture;
		Ref<Texture2D> HilbertLut;
		Ref<TextureCube> BlackCubeTexture;
		//Ref<Environment> EmptyEnvironment;

		std::unordered_map<std::string, std::string> GlobalShaderMacros;


		// temp
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		//Ref<UniformBuffer> CameraUniformBuffer;
	};

	//TODO:
	static Renderer3DData* s_3DData;

	void Renderer3D::Init()
	{
		FrameBufferSpecification fbSpec;
		fbSpec.Attachments = { FrameBufferTextureFormat::DEPTH32F_TEX3D };
		// light depth texture uses high resolution
		fbSpec.Width = 4096;
		fbSpec.Height = 4096;
		lightFBO = FrameBuffer::Create(fbSpec);
#if 0
		//s_3DData = anew Renderer3DData();

		// Load shaders
		// NOTE: some shaders (compute) need to have optimization disabled because of a shaderc internal error
		/*GetShaderLibrary()->Load("Resources/Shaders/AspectPBR_Static.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/AspectPBR_Transparent.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/AspectPBR_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/Grid.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/Wireframe.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/Wireframe_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/Skybox.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/DirShadowMap.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/DirShadowMap_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/SpotShadowMap.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/SpotShadowMap_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Shaders/HZB.glsl");*/

		// HBAO
		//GetShaderLibrary()->Load("Resources/Shaders/Deinterleaving.glsl");
		//GetShaderLibrary()->Load("Resources/Shaders/Reinterleaving.glsl");
		//GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/HBAOBlur.glsl");
		//GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/HBAO.glsl");

		//// GTAO
		//GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/GTAO.hlsl");
		//GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/GTAO-Denoise.glsl");

		//// AO
		//GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/AO-Composite.glsl");

		/*
		//SSR
		s_3DData->GetShaderLibrary()->Load("Resources/shaders/Pre-Integration.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/Pre-Convolution.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/SSR.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/SSR-Composite.glsl");

		// Environment compute shaders
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/EnvironmentMipFilter.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/EquirectangularToCubeMap.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/EnvironmentIrradiance.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PreethamSky.glsl");

		// Post-processing
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/Bloom.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/DOF.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/EdgeDetection.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PostProcessing/SceneComposite.glsl");

		// Light-culling
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PreDepth.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/PreDepth_Anim.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/LightCulling.glsl");

		// Renderer2D Shaders
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/Renderer2D.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/Renderer2D_Line.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/Renderer2D_Circle.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/Renderer2D_Text.glsl");

		// Jump Flood Shaders
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/JumpFlood_Init.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/JumpFlood_Pass.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/JumpFlood_Composite.glsl");

		// Misc
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/SelectedGeometry.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/SelectedGeometry_Anim.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Shaders/TexturePass.glsl");*/

		// TODO:TEMP
		//s_3DData->CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
#endif
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, MeshComponent& meshComponent, int EntityID)
	{
		Ref<Shader> defaultShader = Library<Shader>::GetInstance().GetDefaultShader();
		defaultShader->Bind();
		if (meshComponent.m_Mesh->bPlayAnim)
			defaultShader->SetBool("u_Animated", true);
		else
			defaultShader->SetBool("u_Animated", false);

		meshComponent.m_Mesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().GetDefaultShader(), EntityID);
	}


	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = camera.GetProjection() * glm::inverse(transform);
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = camera.GetViewProjection();
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));
	}

	void Renderer3D::EndScene()
	{
	}
}
