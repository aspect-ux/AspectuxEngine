#pragma once

#include <string>

//TEMP
//#include "Aspect/Renderer/ExtendedRenderer/ShaderUniform.h"

#include <glm/glm.hpp>

namespace Aspect
{
	/*
	enum class ShaderUniformType
	{
		None = 0, Bool, Int, UInt, Float, Vec2, Vec3, Vec4, Mat3, Mat4,
		IVec2, IVec3, IVec4
	};
	
	class ShaderUniform
	{
	public:
		ShaderUniform() = default;
		ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset);

		const std::string& GetName() const { return m_Name; }
		ShaderUniformType GetType() const { return m_Type; }
		uint32_t GetSize() const { return m_Size; }
		uint32_t GetOffset() const { return m_Offset; }

		static const std::string& UniformTypeToString(ShaderUniformType type);

	private:
		std::string m_Name;
		ShaderUniformType m_Type = ShaderUniformType::None;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	struct ShaderUniformBuffer
	{
		std::string Name;
		uint32_t Index;
		uint32_t BindingPoint;
		uint32_t Size;
		uint32_t RendererID;
		std::vector<ShaderUniform> Uniforms;
	};


	struct ShaderBuffer
	{
		std::string Name;
		uint32_t Size = 0;
		std::unordered_map<std::string, ShaderUniform> Uniforms;
	};*/

	class Shader 
	{
	public:
		//Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		// 使用virtual的话子类会调用自己的析构，而不是基类的析构；不使用可能造成内存泄漏
		virtual ~Shader() = default; // default的作用在于显示声明默认析构函数，这样就算不带参数也能创建对象。 如Shader s;

		// 这里让Shader称为抽象类，不能够创建Shader对象，使用Create函数来根据平台不同创建
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Reload(bool forceCompile = false) { return; }

		// NEW shader system
		/*virtual void SetUniformBuffer(const std::string& name, const void* data, uint32_t size) = 0;

		virtual void SetUniform(const std::string& fullname, float value) = 0;
		virtual void SetUniform(const std::string& fullname, uint32_t value) = 0;
		virtual void SetUniform(const std::string& fullname, int value) = 0;
		virtual void SetUniform(const std::string& fullname, const glm::vec2& value) = 0;
		virtual void SetUniform(const std::string& fullname, const glm::vec3& value) = 0;
		virtual void SetUniform(const std::string& fullname, const glm::vec4& value) = 0;
		virtual void SetUniform(const std::string& fullname, const glm::mat3& value) = 0;
		virtual void SetUniform(const std::string& fullname, const glm::mat4& value) = 0;*/

		virtual void SetBool(const std::string& name, bool value) = 0;
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		//virtual void SetFloat2(const std::string& name, const glm::vec2& value) { return; }
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		// TODO: to be removed
		/*virtual const std::unordered_map<std::string, ShaderBuffer>& GetShaderBuffers() {
			return std::unordered_map<std::string, ShaderBuffer>(0);
		};
		virtual const std::unordered_map<std::string, ShaderResourceDeclaration>& GetResources() const
		{
			return std::unordered_map<std::string, ShaderResourceDeclaration>(0);
		}*/

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc);

		static Ref<Shader> Shader::CreateNativeShader(const std::filesystem::path& filepath);
		static Ref<Shader> Shader::CreateNativeShader(const std::string& filepath);

		//TODO:
		// Use native shader language instead of spir-v
		/*static Ref<Shader> CreateNative(const std::filesystem::path& filepath);
		static Ref<Shader> CreateNative(const std::string& filepath);
		static Ref<Shader> CreateNative(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);*/
	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name,const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;

		std::unordered_map<std::string, Ref<Shader>>& GetShaders() { return m_Shaders; }
		const std::unordered_map<std::string, Ref<Shader>>& GetShaders() const { return m_Shaders; }
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
