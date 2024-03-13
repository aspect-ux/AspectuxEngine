#include "aspch.h"
#include "Texture.h"

#include "Aspect/Renderer/Renderer.h"
#include "Aspect/Platform/OpenGL/OpenGLTexture.h"

namespace Aspect
{
	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLTexture2D>(specification);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::OpenGL: return CreateRef<OpenGLTexture2D>(path);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, const std::filesystem::path& filepath)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::OpenGL: return CreateRef<OpenGLTexture2D>(filepath.string());
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	Ref<TextureCube> TextureCube::Create(const TextureSpecification& specification, Buffer imageData)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None: return nullptr;
		//case RendererAPIType::OpenGL: return Ref<OpenGLTextureCube>::Create(specification, imageData);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	//==================CubeMapTexture===================
	Ref<CubeMapTexture> CubeMapTexture::Create(std::vector<std::string>& paths)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLCubeMapTexture>(paths);
		/*case RendererAPIType::Vulkan:  return nullptr;
		case RendererAPIType::DX11:    return nullptr;
		case RendererAPIType::DX12:    return nullptr;*/
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<CubeMapTexture> CubeMapTexture::Create()
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLCubeMapTexture>();
		/*case RendererAPIType::Vulkan:  return nullptr;
		case RendererAPIType::DX11:    return nullptr;
		case RendererAPIType::DX12:    return nullptr;*/
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<CubeMapTexture> CubeMapTexture::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLCubeMapTexture>(width, height);
		/*case RendererAPIType::Vulkan:  return nullptr;
		case RendererAPIType::DX11:    return nullptr;
		case RendererAPIType::DX12:    return nullptr;*/
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture3D> Texture3D::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return CreateRef<OpenGLTexture3D>(width, height);
		/*case RendererAPIType::Vulkan:  return nullptr;
		case RendererAPIType::DX11:    return nullptr;
		case RendererAPIType::DX12:    return nullptr;*/
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
