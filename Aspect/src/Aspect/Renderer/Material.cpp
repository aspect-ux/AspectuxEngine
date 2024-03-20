#include "aspch.h"

#include "Aspect/Renderer/Material.h"
#include "Aspect/Platform/OpenGL/OpenGLMaterial.h"
#include "Aspect/Renderer/RendererAPI.h"

namespace Aspect
{
	AspectRef<Material> Material::Create(const Ref<Shader>& shader, const std::string& name)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::OpenGL: return AspectRef<OpenGLMaterial>::Create(shader, name);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	AspectRef<Material> Material::Copy(const AspectRef<Material>& other, const std::string& name)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::OpenGL: return AspectRef<OpenGLMaterial>::Create(other, name);
		}
		AS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	void Material::Initialize()
	{
		unsigned char data[4];

		for (size_t i = 0; i < 3; i++)
		{
			data[i] = (unsigned char)(metallic * 255.0f);
		}
		data[3] = (unsigned char)255.0f;
		metallicRGBA->SetData(data, sizeof(unsigned char) * 4);

		for (size_t i = 0; i < 3; i++)
		{
			data[i] = (unsigned char)(roughness * 255.0f);
		}
		data[3] = (unsigned char)255.0f;
		roughnessRGBA->SetData(data, sizeof(unsigned char) * 4);
	}

	void Material::SetBlendable(bool p_transparent)
	{
		m_blendable = p_transparent;
	}

	void Material::SetBackfaceCulling(bool p_backfaceCulling)
	{
		m_backfaceCulling = p_backfaceCulling;
	}

	void Material::SetFrontfaceCulling(bool p_frontfaceCulling)
	{
		m_frontfaceCulling = p_frontfaceCulling;
	}

	void Material::SetDepthTest(bool p_depthTest)
	{
		m_depthTest = p_depthTest;
	}

	void Material::SetDepthWriting(bool p_depthWriting)
	{
		m_depthWriting = p_depthWriting;
	}

	void Material::SetColorWriting(bool p_colorWriting)
	{
		m_colorWriting = p_colorWriting;
	}

	void Material::SetGPUInstances(int p_instances)
	{
		m_gpuInstances = p_instances;
	}

	bool Material::IsBlendable() const
	{
		return m_blendable;
	}

	bool Material::HasBackfaceCulling() const
	{
		return m_backfaceCulling;
	}

	bool Material::HasFrontfaceCulling() const
	{
		return m_frontfaceCulling;
	}

	bool Material::HasDepthTest() const
	{
		return m_depthTest;
	}

	bool Material::HasDepthWriting() const
	{
		return m_depthWriting;
	}

	bool Material::HasColorWriting() const
	{
		return m_colorWriting;
	}

	int Material::GetGPUInstances() const
	{
		return m_gpuInstances;
	}

	uint8_t Material::GenerateStateMask() const
	{
		uint8_t result = 0;

		if (m_depthWriting)								result |= 0b0000'0001;
		if (m_colorWriting)								result |= 0b0000'0010;
		if (m_blendable)								result |= 0b0000'0100;
		if (m_backfaceCulling || m_frontfaceCulling)	result |= 0b0000'1000;
		if (m_depthTest)								result |= 0b0001'0000;
		if (m_backfaceCulling)							result |= 0b0010'0000;
		if (m_frontfaceCulling)							result |= 0b0100'0000;

		return result;
	}

	std::map<std::string, std::any>& Material::GetUniformsData()
	{
		return m_uniformsData;
	}
}
