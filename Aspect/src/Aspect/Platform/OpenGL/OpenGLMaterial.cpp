#include "aspch.h"
#include "OpenGLMaterial.h"

#include "Aspect/Renderer/Renderer.h"

#include "Aspect/Platform/OpenGL/OpenGLContext.h"
#include "Aspect/Platform/OpenGL/OpenGLTexture.h"
//#include "Aspect/Platform/OpenGL/OpenGLImage.h"
#include "Aspect/Platform/OpenGL/OpenGLPipeline.h"
#include "Aspect/Platform/OpenGL/OpenGLUniformBuffer.h"

#include "Aspect/Core/Timer.h"

namespace Aspect {

	OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const std::string& name)
		: m_Shader(shader), m_Name(name)//,
		//m_WriteDescriptors(Renderer::GetConfig().FramesInFlight),
		//m_DirtyDescriptorSets(Renderer::GetConfig().FramesInFlight, true)
	{
		Initialize();
		return;
		Init();
		//Renderer::RegisterShaderDependency(shader, this);
	}
	
	OpenGLMaterial::OpenGLMaterial(AspectRef<Material> material, const std::string& name)
		: m_Shader(material->GetShader()), m_Name(name)//,
		//m_WriteDescriptors(Renderer::GetConfig().FramesInFlight),
		//m_DirtyDescriptorSets(Renderer::GetConfig().FramesInFlight, true)
	{
		/*
		if (name.empty())
			m_Name = material->GetName();

		//Renderer::RegisterShaderDependency(m_Shader, this);
		auto OpenGLMaterial = material.As<OpenGLMaterial>();
		m_UniformStorageBuffer = Buffer::Copy(OpenGLMaterial->m_UniformStorageBuffer.Data, OpenGLMaterial->m_UniformStorageBuffer.Size);

		//m_ResidentDescriptors = OpenGLMaterial->m_ResidentDescriptors;
		//m_ResidentDescriptorArrays = OpenGLMaterial->m_ResidentDescriptorArrays;
		//m_PendingDescriptors = OpenGLMaterial->m_PendingDescriptors;
		m_Textures = OpenGLMaterial->m_Textures;
		m_TextureArrays = OpenGLMaterial->m_TextureArrays;
		m_Images = OpenGLMaterial->m_Images;*/
	}
	
	OpenGLMaterial::~OpenGLMaterial()
	{
		m_UniformStorageBuffer.Release();
	}

	void OpenGLMaterial::Init()
	{
		AllocateStorage();

		//m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
		//m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;

#define INVALIDATE_ON_RT 1
#if INVALIDATE_ON_RT
		AspectRef<OpenGLMaterial> instance = this;
		Renderer::Submit([instance]() mutable
			{
				instance->Invalidate();
			});
#else
		Invalidate();
#endif
	}

	void OpenGLMaterial::Invalidate()
	{
		/*
		uint32_t framesInFlight = Renderer::GetConfig().FramesInFlight;
		auto shader = m_Shader.As<OpenGLShader>();
		if (shader->HasDescriptorSet(0))
		{
			const auto& shaderDescriptorSets = shader->GetShaderDescriptorSets();
			if (!shaderDescriptorSets.empty())
			{
				for (auto&& [binding, descriptor] : m_ResidentDescriptors)
					m_PendingDescriptors.push_back(descriptor);
			}
		}*/
	}

	void OpenGLMaterial::AllocateStorage()
	{
		return;
		const auto& shaderBuffers = m_Shader->GetShaderBuffers();

		if (shaderBuffers.size() > 0)
		{
			uint32_t size = 0;
			for (auto [name, shaderBuffer] : shaderBuffers)
				size += shaderBuffer.Size;

			m_UniformStorageBuffer.Allocate(size);
			m_UniformStorageBuffer.ZeroInitialize();
		}
	}

	void OpenGLMaterial::OnShaderReloaded()
	{
		/*
		std::unordered_map<uint32_t, std::shared_ptr<PendingDescriptor>> newDescriptors;
		std::unordered_map<uint32_t, std::shared_ptr<PendingDescriptorArray>> newDescriptorArrays;
		for (auto [name, resource] : m_Shader->GetResources())
		{
			const uint32_t binding = resource.GetRegister();

			if (m_ResidentDescriptors.find(binding) != m_ResidentDescriptors.end())
				newDescriptors[binding] = std::move(m_ResidentDescriptors.at(binding));
			else if (m_ResidentDescriptorArrays.find(binding) != m_ResidentDescriptorArrays.end())
				newDescriptorArrays[binding] = std::move(m_ResidentDescriptorArrays.at(binding));
		}
		m_ResidentDescriptors = std::move(newDescriptors);
		m_ResidentDescriptorArrays = std::move(newDescriptorArrays);

		Invalidate();
		InvalidateDescriptorSets();*/
	}

	const ShaderUniform* OpenGLMaterial::FindUniformDeclaration(const std::string& name)
	{
		const auto& shaderBuffers = m_Shader->GetShaderBuffers();

		AS_CORE_ASSERT(shaderBuffers.size() <= 1, "We currently only support ONE material buffer!");

		if (shaderBuffers.size() > 0)
		{
			const ShaderBuffer& buffer = (*shaderBuffers.begin()).second;
			if (buffer.Uniforms.find(name) == buffer.Uniforms.end())
				return nullptr;

			return &buffer.Uniforms.at(name);
		}
		return nullptr;
	}

	const ShaderResourceDeclaration* OpenGLMaterial::FindResourceDeclaration(const std::string& name)
	{
		auto& resources = m_Shader->GetResources();
		if (resources.find(name) != resources.end())
			return &resources.at(name);

		return nullptr;
	}
	/*
	void OpenGLMaterial::SetOpenGLDescriptor(const std::string& name, const Ref<Texture2D>& texture)
	{
		const ShaderResourceDeclaration* resource = FindResourceDeclaration(name);
		HZ_CORE_ASSERT(resource);

		uint32_t binding = resource->GetRegister();

		// Texture is already set
		// TODO(Karim): Shouldn't need to check resident descriptors..
		if (binding < m_Textures.size() && m_Textures[binding] && texture->GetHash() == m_Textures[binding]->GetHash() && m_ResidentDescriptors.find(binding) != m_ResidentDescriptors.end())
			return;

		if (binding >= m_Textures.size())
			m_Textures.resize(binding + 1);
		m_Textures[binding] = texture;

		const VkWriteDescriptorSet* wds = m_Shader.As<OpenGLShader>()->GetDescriptorSet(name);
		HZ_CORE_ASSERT(wds);
		m_ResidentDescriptors[binding] = std::make_shared<PendingDescriptor>(PendingDescriptor{ PendingDescriptorType::Texture2D, *wds, {}, texture.As<Texture>(), nullptr });
		m_PendingDescriptors.push_back(m_ResidentDescriptors.at(binding));

		InvalidateDescriptorSets();
	}

	void OpenGLMaterial::SetOpenGLDescriptor(const std::string& name, const Ref<Texture2D>& texture, uint32_t arrayIndex)
	{
		const ShaderResourceDeclaration* resource = FindResourceDeclaration(name);
		HZ_CORE_ASSERT(resource);

		uint32_t binding = resource->GetRegister();
		// Texture is already set
		if (binding < m_TextureArrays.size() && m_TextureArrays[binding].size() < arrayIndex && texture->GetHash() == m_TextureArrays[binding][arrayIndex]->GetHash())
			return;

		if (binding >= m_TextureArrays.size())
			m_TextureArrays.resize(binding + 1);

		if (arrayIndex >= m_TextureArrays[binding].size())
			m_TextureArrays[binding].resize(arrayIndex + 1);

		m_TextureArrays[binding][arrayIndex] = texture;

		const VkWriteDescriptorSet* wds = m_Shader.As<OpenGLShader>()->GetDescriptorSet(name);
		HZ_CORE_ASSERT(wds);
		if (m_ResidentDescriptorArrays.find(binding) == m_ResidentDescriptorArrays.end())
		{
			m_ResidentDescriptorArrays[binding] = std::make_shared<PendingDescriptorArray>(PendingDescriptorArray{ PendingDescriptorType::Texture2D, *wds, {}, {}, {} });
		}

		auto& residentDesriptorArray = m_ResidentDescriptorArrays.at(binding);
		if (arrayIndex >= residentDesriptorArray->Textures.size())
			residentDesriptorArray->Textures.resize(arrayIndex + 1);

		residentDesriptorArray->Textures[arrayIndex] = texture;

		//m_PendingDescriptors.push_back(m_ResidentDescriptors.at(binding));

		InvalidateDescriptorSets();
	}


	void OpenGLMaterial::SetOpenGLDescriptor(const std::string& name, const Ref<TextureCube>& texture)
	{
		const ShaderResourceDeclaration* resource = FindResourceDeclaration(name);
		HZ_CORE_ASSERT(resource);

		uint32_t binding = resource->GetRegister();
		// Texture is already set
		// TODO(Karim): Shouldn't need to check resident descriptors..
		if (binding < m_Textures.size() && m_Textures[binding] && texture->GetHash() == m_Textures[binding]->GetHash() && m_ResidentDescriptors.find(binding) != m_ResidentDescriptors.end())
			return;

		if (binding >= m_Textures.size())
			m_Textures.resize(binding + 1);
		m_Textures[binding] = texture;

		const VkWriteDescriptorSet* wds = m_Shader.As<OpenGLShader>()->GetDescriptorSet(name);
		HZ_CORE_ASSERT(wds);
		m_ResidentDescriptors[binding] = std::make_shared<PendingDescriptor>(PendingDescriptor{ PendingDescriptorType::TextureCube, *wds, {}, texture.As<Texture>(), nullptr });
		m_PendingDescriptors.push_back(m_ResidentDescriptors.at(binding));

		InvalidateDescriptorSets();
	}

	void OpenGLMaterial::SetOpenGLDescriptor(const std::string& name, const Ref<Image2D>& image)
	{
		HZ_CORE_VERIFY(image);
		HZ_CORE_ASSERT(image.As<OpenGLImage2D>()->GetImageInfo().ImageView, "ImageView is null");

		const ShaderResourceDeclaration* resource = FindResourceDeclaration(name);
		HZ_CORE_VERIFY(resource);

		uint32_t binding = resource->GetRegister();
		// Image is already set
		// TODO(Karim): Shouldn't need to check resident descriptors..
		if (binding < m_Images.size() && m_Images[binding] && m_ResidentDescriptors.find(binding) != m_ResidentDescriptors.end())
			return;

		if (resource->GetRegister() >= m_Images.size())
			m_Images.resize(resource->GetRegister() + 1);
		m_Images[resource->GetRegister()] = image;

		const VkWriteDescriptorSet* wds = m_Shader.As<OpenGLShader>()->GetDescriptorSet(name);
		HZ_CORE_ASSERT(wds);
		m_ResidentDescriptors[binding] = std::make_shared<PendingDescriptor>(PendingDescriptor{ PendingDescriptorType::Image2D, *wds, {}, nullptr, image.As<Image>() });
		m_PendingDescriptors.push_back(m_ResidentDescriptors.at(binding));

		InvalidateDescriptorSets();
	}*/

	void OpenGLMaterial::Set(const std::string& name, float value)
	{
		Set<float>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, int value)
	{
		Set<int>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, uint32_t value)
	{
		Set<uint32_t>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, bool value)
	{
		// Bools are 4-byte ints
		Set<int>(name, (int)value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::ivec2& value)
	{
		Set<glm::ivec2>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::ivec3& value)
	{
		Set<glm::ivec3>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::ivec4& value)
	{
		Set<glm::ivec4>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::vec2& value)
	{
		Set<glm::vec2>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::vec3& value)
	{
		Set<glm::vec3>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::vec4& value)
	{
		Set<glm::vec4>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::mat3& value)
	{
		Set<glm::mat3>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::mat4& value)
	{
		Set<glm::mat4>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const Ref<Texture2D>& texture)
	{
		//SetOpenGLDescriptor(name, texture);
	}

	void OpenGLMaterial::Set(const std::string& name, const Ref<Texture2D>& texture, uint32_t arrayIndex)
	{
		//SetOpenGLDescriptor(name, texture, arrayIndex);
	}

	void OpenGLMaterial::Set(const std::string& name, const Ref<TextureCube>& texture)
	{
		//SetOpenGLDescriptor(name, texture);
	}

	void OpenGLMaterial::Set(const std::string& name, const Ref<Image2D>& image)
	{
		//SetOpenGLDescriptor(name, image);
	}

	float& OpenGLMaterial::GetFloat(const std::string& name)
	{
		return Get<float>(name);
	}

	int32_t& OpenGLMaterial::GetInt(const std::string& name)
	{
		return Get<int32_t>(name);
	}

	uint32_t& OpenGLMaterial::GetUInt(const std::string& name)
	{
		return Get<uint32_t>(name);
	}

	bool& OpenGLMaterial::GetBool(const std::string& name)
	{
		return Get<bool>(name);
	}

	glm::vec2& OpenGLMaterial::GetVector2(const std::string& name)
	{
		return Get<glm::vec2>(name);
	}

	glm::vec3& OpenGLMaterial::GetVector3(const std::string& name)
	{
		return Get<glm::vec3>(name);
	}

	glm::vec4& OpenGLMaterial::GetVector4(const std::string& name)
	{
		return Get<glm::vec4>(name);
	}

	glm::mat3& OpenGLMaterial::GetMatrix3(const std::string& name)
	{
		return Get<glm::mat3>(name);
	}

	glm::mat4& OpenGLMaterial::GetMatrix4(const std::string& name)
	{
		return Get<glm::mat4>(name);
	}

	Ref<Texture2D> OpenGLMaterial::GetTexture2D(const std::string& name)
	{
		return nullptr;
		//return GetResource<Texture2D>(name);
	}
	/*
	Ref<TextureCube> OpenGLMaterial::TryGetTextureCube(const std::string& name)
	{
		return TryGetResource<TextureCube>(name);
	}

	Ref<Texture2D> OpenGLMaterial::TryGetTexture2D(const std::string& name)
	{
		return TryGetResource<Texture2D>(name);
	}

	Ref<TextureCube> OpenGLMaterial::GetTextureCube(const std::string& name)
	{
		return GetResource<TextureCube>(name);
	}

	void OpenGLMaterial::RT_UpdateForRendering(const std::vector<std::vector<VkWriteDescriptorSet>>& uniformBufferWriteDescriptors)
	{
		HZ_SCOPE_PERF("OpenGLMaterial::RT_UpdateForRendering");
		auto OpenGLDevice = OpenGLContext::GetCurrentDevice()->GetOpenGLDevice();
		for (auto&& [binding, descriptor] : m_ResidentDescriptors)
		{
			if (descriptor->Type == PendingDescriptorType::Image2D)
			{
				Ref<OpenGLImage2D> image = descriptor->Image.As<OpenGLImage2D>();
				HZ_CORE_ASSERT(image->GetImageInfo().ImageView, "ImageView is null");
				if (descriptor->WDS.pImageInfo && image->GetImageInfo().ImageView != descriptor->WDS.pImageInfo->imageView)
				{
					// HZ_CORE_WARN("Found out of date Image2D descriptor ({0} vs. {1})", (void*)image->GetImageInfo().ImageView, (void*)descriptor->WDS.pImageInfo->imageView);
					m_PendingDescriptors.emplace_back(descriptor);
					InvalidateDescriptorSets();
				}
			}
		}

		std::vector<VkDescriptorImageInfo> arrayImageInfos;

		uint32_t frameIndex = Renderer::RT_GetCurrentFrameIndex();
		// NOTE(Yan): we can't cache the results atm because we might render the same material in different viewports,
		//            and so we can't bind to the same uniform buffers
		if (m_DirtyDescriptorSets[frameIndex])
		{
			m_DirtyDescriptorSets[frameIndex] = false;
			m_WriteDescriptors[frameIndex].clear();

			if (!uniformBufferWriteDescriptors.empty())
			{
				for (auto& wd : uniformBufferWriteDescriptors[frameIndex])
					m_WriteDescriptors[frameIndex].push_back(wd);
			}

			for (auto&& [binding, pd] : m_ResidentDescriptors)
			{
				if (pd->Type == PendingDescriptorType::Texture2D)
				{
					Ref<OpenGLTexture2D> texture = pd->Texture.As<OpenGLTexture2D>();
					pd->ImageInfo = texture->GetOpenGLDescriptorInfo();
					pd->WDS.pImageInfo = &pd->ImageInfo;
				}
				else if (pd->Type == PendingDescriptorType::TextureCube)
				{
					Ref<OpenGLTextureCube> texture = pd->Texture.As<OpenGLTextureCube>();
					pd->ImageInfo = texture->GetOpenGLDescriptorInfo();
					pd->WDS.pImageInfo = &pd->ImageInfo;
				}
				else if (pd->Type == PendingDescriptorType::Image2D)
				{
					Ref<OpenGLImage2D> image = pd->Image.As<OpenGLImage2D>();
					pd->ImageInfo = image->GetDescriptorInfo();
					pd->WDS.pImageInfo = &pd->ImageInfo;
				}

				m_WriteDescriptors[frameIndex].push_back(pd->WDS);
			}

			for (auto&& [binding, pd] : m_ResidentDescriptorArrays)
			{
				if (pd->Type == PendingDescriptorType::Texture2D)
				{
					for (auto tex : pd->Textures)
					{
						Ref<OpenGLTexture2D> texture = tex.As<OpenGLTexture2D>();
						arrayImageInfos.emplace_back(texture->GetOpenGLDescriptorInfo());
					}
				}
				pd->WDS.pImageInfo = arrayImageInfos.data();
				pd->WDS.descriptorCount = (uint32_t)arrayImageInfos.size();
				m_WriteDescriptors[frameIndex].push_back(pd->WDS);
			}

		}

		auto OpenGLShader = m_Shader.As<OpenGLShader>();
		auto descriptorSet = OpenGLShader->AllocateDescriptorSet();
		m_DescriptorSets[frameIndex] = descriptorSet;
		for (auto& writeDescriptor : m_WriteDescriptors[frameIndex])
			writeDescriptor.dstSet = descriptorSet.DescriptorSets[0];

		vkUpdateDescriptorSets(OpenGLDevice, (uint32_t)m_WriteDescriptors[frameIndex].size(), m_WriteDescriptors[frameIndex].data(), 0, nullptr);
		m_PendingDescriptors.clear();
	}

	void OpenGLMaterial::RT_UpdateForRendering(const std::vector<std::vector<VkWriteDescriptorSet>>& uniformBufferWriteDescriptors, const std::vector<std::vector<VkWriteDescriptorSet>>& storageBufferWriteDescriptors)
	{
		HZ_SCOPE_PERF("OpenGLMaterial::RT_UpdateForRendering");
		auto OpenGLDevice = OpenGLContext::GetCurrentDevice()->GetOpenGLDevice();
		for (auto&& [binding, descriptor] : m_ResidentDescriptors)
		{
			if (descriptor->Type == PendingDescriptorType::Image2D)
			{
				Ref<OpenGLImage2D> image = descriptor->Image.As<OpenGLImage2D>();
				HZ_CORE_ASSERT(image->GetImageInfo().ImageView, "ImageView is null");
				if (descriptor->WDS.pImageInfo && image->GetImageInfo().ImageView != descriptor->WDS.pImageInfo->imageView)
				{
					// HZ_CORE_WARN("Found out of date Image2D descriptor ({0} vs. {1})", (void*)image->GetImageInfo().ImageView, (void*)descriptor->WDS.pImageInfo->imageView);
					m_PendingDescriptors.emplace_back(descriptor);
					InvalidateDescriptorSets();
				}
			}
		}

		std::vector<VkDescriptorImageInfo> arrayImageInfos;

		uint32_t frameIndex = Renderer::RT_GetCurrentFrameIndex();
		// NOTE(Yan): we can't cache the results atm because we might render the same material in different viewports,
		//            and so we can't bind to the same uniform buffers
		if (m_DirtyDescriptorSets[frameIndex])
		{
			m_DirtyDescriptorSets[frameIndex] = false;
			m_WriteDescriptors[frameIndex].clear();

			if (!uniformBufferWriteDescriptors.empty())
			{
				for (auto& wd : uniformBufferWriteDescriptors[frameIndex])
					m_WriteDescriptors[frameIndex].push_back(wd);
			}

			if (!storageBufferWriteDescriptors.empty())
			{
				for (auto& wd : storageBufferWriteDescriptors[frameIndex])
					m_WriteDescriptors[frameIndex].push_back(wd);
			}

			for (auto&& [binding, pd] : m_ResidentDescriptors)
			{
				if (pd->Type == PendingDescriptorType::Texture2D)
				{
					Ref<OpenGLTexture2D> texture = pd->Texture.As<OpenGLTexture2D>();
					pd->ImageInfo = texture->GetOpenGLDescriptorInfo();
					pd->WDS.pImageInfo = &pd->ImageInfo;
				}
				else if (pd->Type == PendingDescriptorType::TextureCube)
				{
					Ref<OpenGLTextureCube> texture = pd->Texture.As<OpenGLTextureCube>();
					pd->ImageInfo = texture->GetOpenGLDescriptorInfo();
					pd->WDS.pImageInfo = &pd->ImageInfo;
				}
				else if (pd->Type == PendingDescriptorType::Image2D)
				{
					Ref<OpenGLImage2D> image = pd->Image.As<OpenGLImage2D>();
					pd->ImageInfo = image->GetDescriptorInfo();
					pd->WDS.pImageInfo = &pd->ImageInfo;
				}

				m_WriteDescriptors[frameIndex].push_back(pd->WDS);
			}

			for (auto&& [binding, pd] : m_ResidentDescriptorArrays)
			{
				if (pd->Type == PendingDescriptorType::Texture2D)
				{
					for (auto tex : pd->Textures)
					{
						Ref<OpenGLTexture2D> texture = tex.As<OpenGLTexture2D>();
						arrayImageInfos.emplace_back(texture->GetOpenGLDescriptorInfo());
					}
				}
				pd->WDS.pImageInfo = arrayImageInfos.data();
				pd->WDS.descriptorCount = (uint32_t)arrayImageInfos.size();
				m_WriteDescriptors[frameIndex].push_back(pd->WDS);
			}

		}

		auto OpenGLShader = m_Shader.As<OpenGLShader>();
		auto descriptorSet = OpenGLShader->AllocateDescriptorSet();
		m_DescriptorSets[frameIndex] = descriptorSet;
		for (auto& writeDescriptor : m_WriteDescriptors[frameIndex])
			writeDescriptor.dstSet = descriptorSet.DescriptorSets[0];

		vkUpdateDescriptorSets(OpenGLDevice, (uint32_t)m_WriteDescriptors[frameIndex].size(), m_WriteDescriptors[frameIndex].data(), 0, nullptr);
		m_PendingDescriptors.clear();
	}

	void OpenGLMaterial::InvalidateDescriptorSets()
	{
		const uint32_t framesInFlight = Renderer::GetConfig().FramesInFlight;
		for (uint32_t i = 0; i < framesInFlight; i++)
			m_DirtyDescriptorSets[i] = true;
	}*/

}
