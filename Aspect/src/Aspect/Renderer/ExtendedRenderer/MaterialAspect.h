/**
 * @package Aspect (Hazel to Morava)
 * @author  Yan Chernikov (TheCherno)
 * @licence Apache License 2.0
 */

#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "Aspect/Core/Ref.h"
#include "Aspect/Renderer/Texture.h"
#include "Aspect/Renderer/Shader.h"

#include "Aspect/Core/Log.h"

// TEMP
#include "Aspect/Renderer/ExtendedRenderer/ShaderUniform.h"

#include <unordered_set>


namespace Aspect {
	/*
	enum class MaterialFlag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2),
		TwoSided = BIT(3),
	};

	class MaterialInstance : public RefCounted
	{
		friend class MaterialAspect;

	public:
		MaterialInstance(const AspectRef<MaterialAspect>& material, const std::string& name = "");
		virtual ~MaterialInstance();

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			m_OverriddenValues.insert(name);
		}

		void Set(const std::string& name, Texture* texture, uint32_t slot)
		{
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, Texture2D* texture)
		{
			Set(name, (Texture*)texture);
		}

		void Set(const std::string& name, TextureCube* texture)
		{
			Set(name, (Texture*)texture);
		}

		void Bind(); // Removed in more recent commits in Vulkan branch
		void AllocateStorage(); // Removed in more recent commits in Vulkan branch?

		uint32_t GetFlags() const { return m_Material->GetFlags(); }
		bool GetFlag(MaterialFlag flag) const { return (uint32_t)flag & m_Material->GetFlags(); }
		void SetFlag(MaterialFlag flag, bool value = true);

		AspectRef<Shader> GetShader() { return m_Material->GetShader(); }

		static MaterialInstanceAspect* Create(MaterialAspect* material);

#if 0
		template<typename T>
		T& Get(const std::string& name)
		{
			auto decl = FindUniformDeclaration(name);
			Aspect_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
			auto& buffer = GetUniformBufferTarget(decl);
			return buffer.Read<T>(decl->GetOffset());
		}

		template<typename T>
		AspectRef<T> GetResource(const std::string& name)
		{
			auto decl = FindResourceDeclaration(name);
			uint32_t slot = decl->GetRegister();
			Aspect_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
			return m_Textures[slot];
		}
#endif

	public:
		static AspectRef<MaterialInstanceAspect> Create(const AspectRef<MaterialAspect>& material);

	private:
		void OnShaderReloaded();
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
		void OnMaterialValueUpdated(ShaderUniformDeclaration* decl);

	private:
		std::string m_Name;
		AspectRef<MaterialAspect> m_Material;

		std::vector<AspectRef<Texture>> m_Textures;

		// Buffer m_UniformStorageBuffer; // The property should be in parent MaterialAspect
		std::vector<AspectRef<Image>> m_Images;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;

		Buffer m_UniformStorageBuffer; // could be obsolete in later versions of the vulkan branch

		// TODO: This is temporary; come up with a proper system to track overrides
		std::unordered_set<std::string> m_OverriddenValues;

	};
	class MaterialAspect : public RefCounted
	{
		friend class MaterialInstanceAspect; // Removed in more recent commits in Vulkan branch?

	public:
		MaterialAspect();
		MaterialAspect(const AspectRef<Shader>& shader, const std::string& name = "");
		static AspectRef<MaterialAspect> Create(const AspectRef<Shader>& shader, const std::string& name = "");
		virtual ~MaterialAspect();

		virtual void Invalidate() = 0;

		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, uint32_t value) = 0;
		virtual void Set(const std::string& name, bool value) = 0;
		virtual void Set(const std::string& name, const glm::ivec2& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec3& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec4& value) = 0;
		virtual void Set(const std::string& name, const glm::vec2& value) = 0;
		virtual void Set(const std::string& name, const glm::vec3& value) = 0;
		virtual void Set(const std::string& name, const glm::vec4& value) = 0;
		virtual void Set(const std::string& name, const glm::mat3& value) = 0;
		virtual void Set(const std::string& name, const glm::mat4& value) = 0;

		virtual void Set(const std::string& name, const AspectRef<Texture2D>& texture) = 0;
		virtual void Set(const std::string& name, const AspectRef<Texture2D>& texture, uint32_t arrayIndex) = 0;
		virtual void Set(const std::string& name, const AspectRef<TextureCube>& texture) = 0;
		virtual void Set(const std::string& name, const AspectRef<Image2D>& image) = 0;

		virtual float& GetFloat(const std::string& name) = 0;
		virtual int32_t& GetInt(const std::string& name) = 0;
		virtual uint32_t& GetUInt(const std::string& name) = 0;
		virtual bool& GetBool(const std::string& name) = 0;
		virtual glm::vec2& GetVector2(const std::string& name) = 0;
		virtual glm::vec3& GetVector3(const std::string& name) = 0;
		virtual glm::vec4& GetVector4(const std::string& name) = 0;
		virtual glm::mat3& GetMatrix3(const std::string& name) = 0;
		virtual glm::mat4& GetMatrix4(const std::string& name) = 0;

		virtual AspectRef<Texture2D> GetTexture2D(const std::string& name) = 0;
		virtual AspectRef<TextureCube> GetTextureCube(const std::string& name) = 0;

		virtual AspectRef<Texture2D> TryGetTexture2D(const std::string& name) = 0;
		virtual AspectRef<TextureCube> TryGetTextureCube(const std::string& name) = 0;

#if 0
		template<typename T>
		T& Get(const std::string& name)
		{
			auto decl = FindUniformDeclaration(name);
			Aspect_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
			auto& buffer = GetUniformBufferTarget(decl);
			return buffer.Read<T>(decl->GetOffset());
		}

		template<typename T>
		AspectRef<T> GetResource(const std::string& name)
		{
			auto decl = FindResourceDeclaration(name);
			uint32_t slot = decl->GetRegister();
			Aspect_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
			return m_Textures[slot];
		}
#endif

		virtual uint32_t GetFlags() const = 0;
		virtual bool GetFlag(MaterialFlag flag) const = 0;
		virtual void SetFlag(MaterialFlag flag, bool value = true) = 0;

		virtual AspectRef<Shader> GetShader() = 0;
		virtual const std::string& GetName() const = 0;

		Buffer GetUniformStorageBuffer() { return m_UniformStorageBuffer; }; // should it be located in HazelMaterial or VulkanMaterial?

		// TODO: obsolete?
		void Bind(); // Removed in more recent commits in Vulkan branch

	private:
		void AllocateStorage(); // Removed in more recent commits in Vulkan branch?
		void BindTextures(); // Removed in more recent commits in Vulkan branch?

		void OnShaderReloaded();

		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);
		ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);

	protected:
		AspectRef<Shader> m_Shader;
		std::string m_Name;
		Buffer m_UniformStorageBuffer; // should it be located in MaterialAspect or VulkanMaterial?
		std::vector<AspectRef<Texture>> m_Textures;
		std::vector<AspectRef<Image>> m_Images;

	private:
		// std::unordered_set<MaterialAspect*> m_MaterialInstances;
		std::unordered_set<MaterialInstance*> m_MaterialInstances;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;

		uint32_t m_MaterialFlags = 0;
	};
	*/
}
