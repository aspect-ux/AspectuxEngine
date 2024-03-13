#include "aspch.h"

#include "Aspect/Library/ShaderLibrary.h"
#include "Aspect/Asset/AssetManager.h"

namespace Aspect
{
	Library<Shader>::Library()
	{
		/*
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/AspectPBR_Transparent.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/AspectPBR_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/Grid.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/Wireframe.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/Wireframe_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/Skybox.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/DirShadowMap.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/DirShadowMap_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/SpotShadowMap.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/SpotShadowMap_Anim.glsl");
		GetShaderLibrary()->Load("Resources/Resources/TestShaders/HZB.glsl");*/

		// HBAO
		//GetShaderLibrary()->Load("Resources/Resources/TestShaders/Deinterleaving.glsl");
		//GetShaderLibrary()->Load("Resources/Resources/TestShaders/Reinterleaving.glsl");
		//GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/HBAOBlur.glsl");
		//GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/HBAO.glsl");

		//// GTAO
		//GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/GTAO.hlsl");
		//GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/GTAO-Denoise.glsl");

		//// AO
		//GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/AO-Composite.glsl");

		/*
		//SSR
		s_3DData->GetShaderLibrary()->Load("Resources/Resources/TestShaders/Pre-Integration.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/Pre-Convolution.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/SSR.glsl");
		s_3DData->GetShaderLibrary()->Load("Resources/Resources/TestShaders/PostProcessing/SSR-Composite.glsl");*/

		Add("BasePBR", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/BasePBR.glsl")));
		Add("SkyBox", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/SkyBox.glsl")));

		Add("IBL_background", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/IBL/background.glsl")));
		Add("IBL_brdf", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/IBL/brdf.glsl")));
		Add("IBL_equirectangularToCubemap", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/IBL/equirectangularToCubemap.glsl")));
		Add("IBL_irradiance", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/IBL/irradiance.glsl")));
		Add("IBL_prefilter", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/IBL/prefilter.glsl")));

		Add("Post_Outline", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/PostProcessing/Outline.glsl")));
		Add("Post_Cartoon", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/PostProcessing/Cartoon.glsl")));
		Add("Post_GrayScale", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/PostProcessing/GrayScale.glsl")));
		Add("Post_GaussianBlur", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/PostProcessing/GaussianBlur.glsl")));
		Add("Post_FxaaConsole", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/PostProcessing/FxaaConsole.glsl")));
		Add("NormalOutline", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/NormalOutline.glsl")));
		Add("NormalOutline_anim", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/NormalOutline_anim.glsl")));
		
		Add("CSM_Depth", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/Shadow/CSM_Depth.glsl")));
		Add("Debug_Depth", Shader::CreateNativeShader(AssetManager::GetFullStringPath("Resources/TestShaders/Shadow/Debug_Depth.glsl")));

		//TODO: to be added 
		Add("Grid", Shader::Create(AssetManager::GetFullStringPath("Resources/TestShaders/Grid.glsl")));
	}

	Ref<Shader> Library<Shader>::GetDefaultShader()
	{
		return m_Library["BasePBR"];
	}

	Ref<Shader> Library<Shader>::GetSkyBoxShader()
	{
		return m_Library["SkyBox"];
	}
}
