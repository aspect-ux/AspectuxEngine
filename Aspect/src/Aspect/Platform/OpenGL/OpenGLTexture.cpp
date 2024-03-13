#include "aspch.h"

#include "OpenGLTexture.h"
#include "Aspect/Asset/AssetManager.h"
#include "stb_image.h"


namespace Aspect
{

	namespace Utils {

		static GLenum AspectImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
				//TODO:RGB8 is there a difference between opengl with vulkan?
			case ImageFormat::RGB:  return GL_RGB;
			case ImageFormat::RGBA: return GL_RGBA;
			}

			AS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum AspectImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB:  return GL_RGB8;
			case ImageFormat::RGBA: return GL_RGBA8;
			}

			AS_CORE_ASSERT(false);
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
	{

		AS_PROFILE_FUNCTION();

		m_InternalFormat = Utils::AspectImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = Utils::AspectImageFormatToGLDataFormat(m_Specification.Format);

		AS_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height); // 使用GL_RGB8的格式存储图片

		// 相关API参考：`https://blog.csdn.net/dominiced/article/details/79640058`
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 纹理的坐标系统叫做ST坐标系统，和xy坐标系统一样，s对应x，t对应y，
		//因此GL_TEXTURE_WRAP_S和GL_TEXTURE_WRAP_T表示超出范围的纹理处理方式，可以设置的值如下：GL_CLAMP_TO_EDGE、GL_REPEAT、GL_MIRRORED_REPEAT
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		//stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		AS_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			AS_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;

			AS_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, const std::filesystem::path& filepath)
		: m_Path(filepath.string()), m_Specification(specification)
	{
		//TODO:

	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		AS_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		AS_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		AS_CORE_ASSERT(size == m_Width * m_Height * bpp,"Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		AS_PROFILE_FUNCTION();

		//glBindTextureUnit(slot, m_RendererID);
		// be attention! slot = 0 is the default activated texture, if we want to consecutively use several textures,then we should active the correct texture first then bind texture
		// TODO: if there were some similar problems, you should look up in this.
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void OpenGLTexture2D::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}



	// ---------------Tex3D--------------------
   // TODO
	OpenGLTexture3D::OpenGLTexture3D(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	OpenGLTexture3D::OpenGLTexture3D(uint32_t rendererID, uint32_t width, uint32_t height)
		: m_RendererID(rendererID), m_Width(width), m_Height(height)
	{
	}

	OpenGLTexture3D::~OpenGLTexture3D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture3D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	void OpenGLTexture3D::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}



	// ---------------CubeMap--------------------
	OpenGLCubeMapTexture::OpenGLCubeMapTexture()
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		m_Width = 512;
		m_Height = 512;

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	OpenGLCubeMapTexture::OpenGLCubeMapTexture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	// refer to https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(std::vector<std::string>& paths)
		: m_Paths(paths)
	{
		glGenTextures(1, &m_RendererID);
		OpenGLCubeMapTexture::Generate();
	}

	OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubeMapTexture::SetFace(FaceTarget faceIndex, const std::string& path)
	{
		m_Paths[(uint32_t)faceIndex] = path;
	}

	void OpenGLCubeMapTexture::GenerateMipmap()
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void OpenGLCubeMapTexture::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLCubeMapTexture::UnBind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void OpenGLCubeMapTexture::Generate()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width = 0;
		int height = 0;
		int nrChannels = 0;
		stbi_set_flip_vertically_on_load(false);
		for (unsigned int i = 0; i < m_Paths.size(); i++)
		{
			unsigned char* data = stbi_load(AssetManager::GetFullPath(m_Paths[i]).string().c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				AS_CORE_ERROR("Cubemap don't loaded correctly!");
				stbi_image_free(data);
			}
		}

		m_Width = width;
		m_Height = height;

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
}
