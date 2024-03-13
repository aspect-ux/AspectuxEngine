#pragma once

#include "Aspect/Renderer/Texture.h"

#include <glad/glad.h>

namespace Aspect
{
	/* 一个有关析构和构造的知识
	* 如果将析构/构造 声明为private会怎样，那么只能在类内调用（创建销毁对象）；
	* 在这种情况下外界如何创建对象呢？可以在类里面用静态函数创建静态变量，类外使用类名调用
	*/
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification);
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(const TextureSpecification& specification, const std::filesystem::path& filepath);

		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override {return m_Width;}
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		
		virtual const std::string& GetPath() const override { return m_Path; }
		virtual void SetData(void* data, uint32_t size) override;
		

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void UnBind() const override;

		virtual bool Loaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}
	private:
		TextureSpecification m_Specification;

		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

	class OpenGLTexture3D : public Texture3D
	{
	public:
		OpenGLTexture3D(uint32_t width, uint32_t height);
		OpenGLTexture3D(uint32_t rendererID, uint32_t width, uint32_t height);
		virtual ~OpenGLTexture3D();

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Width; };
		virtual uint32_t GetHeight() const override { return m_Height; };
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void UnBind() const override;

		virtual bool Loaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture3D&)other).m_RendererID;
		}
	private:
		TextureSpecification m_Specification;

		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		bool m_IsLoaded = false;
		//GLenum mInternalFormat, mDataFormat;
	};


	class OpenGLCubeMapTexture : public CubeMapTexture
	{
	public:
		OpenGLCubeMapTexture();
		OpenGLCubeMapTexture(uint32_t width, uint32_t height);
		OpenGLCubeMapTexture(std::vector<std::string>& paths);
		virtual ~OpenGLCubeMapTexture();

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Width; };
		virtual uint32_t GetHeight() const override { return m_Height; };
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetFace(FaceTarget faceIndex, const std::string& path) override;
		virtual void GenerateMipmap() override;

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void UnBind() const override;

		virtual void Generate() override;
		[[nodiscard]] virtual std::vector<std::string> GetPaths() override { return m_Paths; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLCubeMapTexture&)other).m_RendererID;
		}
	private:
		TextureSpecification m_Specification;

		uint32_t m_RendererID;
		uint32_t m_Width, m_Height;
		std::vector<std::string> m_Paths;
	};
}
