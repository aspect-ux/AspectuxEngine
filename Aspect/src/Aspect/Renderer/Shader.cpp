#include "aspch.h"

#include "Shader.h"
#include "Renderer.h"
#include "Aspect/Platform/OpenGL/OpenGLShader.h"
#include "Aspect/Platform/OpenGL/OpenGLNativeShader.h"

namespace Aspect
{
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::OpenGL: return CreateRef<OpenGLShader>(filepath);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	Ref<Shader> Shader::Create(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPIType::None: AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			case RendererAPIType::OpenGL: return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::CreateNativeShader(const std::filesystem::path& filepath)
	{
		return CreateNativeShader(filepath.string());
	}

	Ref<Shader> Shader::CreateNativeShader(const std::string& filepath)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    AS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPIType::OpenGL:  return std::make_shared<OpenGLNativeShader>(filepath);
		//case RendererAPIType::DX11:    return nullptr;
		/*case RendererAPIType::Vulkan:  return nullptr;
		
		case RendererAPIType::DX12:    return nullptr;*/
		}

		AS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/*
	* 提供两种添加shader的方法：
	* 1. 直接从文件中读取，自动获取文件名，自动分辨vert 和 frag
	* 2. 创建时手动传输vert和frag还有文件名
	*/
	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		AS_CORE_ASSERT(!Exists(name), "shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		AS_CORE_ASSERT(!Exists(name), "shader already exists!");
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name,shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		AS_CORE_ASSERT(Exists(name), "shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}


	/*TODO:
	Ref<Shader> Shader::CreateNative(const std::filesystem::path& filepath)
	{
		return CreateNative(filepath.string());
	}


	
	Ref<Shader> Shader::CreateNative(const std::string& filepath)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPI::RendererAPIType::None:    HE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL:  return std::make_shared<NativeOpenGLShader>(filepath);
		case RendererAPI::RendererAPIType::Vulkan:  return nullptr;
		case RendererAPI::RendererAPIType::DX11:    return nullptr;
		case RendererAPI::RendererAPIType::DX12:    return nullptr;
		}

		HE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::CreateNative(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPI::RendererAPIType::None:    HE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL:  return std::make_shared<NativeOpenGLShader>(name, vertexSrc, fragmentSrc);
		case RendererAPI::RendererAPIType::Vulkan:  return nullptr;
		case RendererAPI::RendererAPIType::DX11:    return nullptr;
		case RendererAPI::RendererAPIType::DX12:    return nullptr;
		}

		HE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}*/
}
