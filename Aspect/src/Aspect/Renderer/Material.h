#pragma once

#include "Aspect/Renderer/Texture.h"
#include "Aspect/Renderer/Shader.h"
//#include "Aspect/Library/ShaderLibrary.h"
#include "Aspect/Library/TextureLibrary.h"

#include <unordered_map>
#include <any>

namespace Aspect
{
	// from https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
	struct EnumClassHash
	{
		template <typename T>
		std::size_t operator()(T t) const
		{
			return static_cast<std::size_t>(t);
		}
	};

	//TODO:
	enum class ConcreteTextureType
	{
		Albedo = 0,
		Normal,
		Metalness,
		Roughness,
		AmbientOcclusion,
		Specular,
		Height,
		Emission,
	};

	struct MaterialTexture
	{
		Ref<Texture2D> texture2d = nullptr;
		ConcreteTextureType type;
		std::string path;
	};

	class Material : public RefCounted
	{
	public:
		static AspectRef<Material> Create(const Ref<Shader>& shader, const std::string& name = "");
		static AspectRef<Material> Copy(const AspectRef<Material>& other, const std::string& name = "");
		Material() { Initialize(); };
		virtual ~Material() {}
		Material(Ref<Shader> shader) : mShader(shader) { Initialize(); };
	public:
		void SetShader(Ref<Shader> shader) { mShader = shader; }
		[[nodiscard]] Ref<Shader> GetShader() { return mShader; }

		void AddTexture(ConcreteTextureType type, Ref<Texture2D> texture)
		{
			AS_CORE_ASSERT(mTexMap.find(type) == mTexMap.end());
			mTexMap[type] = texture;
		}

		[[nodiscard]] Ref<Texture2D> GetTexture(ConcreteTextureType type) { return mTexMap[type]; }

	public:
		virtual void Invalidate() = 0;
		virtual void OnShaderReloaded() = 0;

		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, uint32_t value) = 0;
		virtual void Set(const std::string& name, bool value) = 0;
		virtual void Set(const std::string& name, const glm::vec2& value) = 0;
		virtual void Set(const std::string& name, const glm::vec3& value) = 0;
		virtual void Set(const std::string& name, const glm::vec4& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec2& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec3& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec4& value) = 0;

		virtual void Set(const std::string& name, const glm::mat3& value) = 0;
		virtual void Set(const std::string& name, const glm::mat4& value) = 0;

		virtual void Set(const std::string& name, const Ref<Texture2D>& texture) = 0;
		virtual void Set(const std::string& name, const Ref<Texture2D>& texture, uint32_t arrayIndex) = 0;
		virtual void Set(const std::string& name, const Ref<TextureCube>& texture) = 0;
		virtual void Set(const std::string& name, const Ref<Image2D>& image) = 0;

		virtual float& GetFloat(const std::string& name) = 0;
		virtual int32_t& GetInt(const std::string& name) = 0;
		virtual uint32_t& GetUInt(const std::string& name) = 0;
		virtual bool& GetBool(const std::string& name) = 0;
		virtual glm::vec2& GetVector2(const std::string& name) = 0;
		virtual glm::vec3& GetVector3(const std::string& name) = 0;
		virtual glm::vec4& GetVector4(const std::string& name) = 0;
		virtual glm::mat3& GetMatrix3(const std::string& name) = 0;
		virtual glm::mat4& GetMatrix4(const std::string& name) = 0;

		virtual Ref<Texture2D> GetTexture2D(const std::string& name) = 0;

		virtual Ref<Texture2D> TryGetTexture2D(const std::string& name) = 0;
		virtual Ref<TextureCube> TryGetTextureCube(const std::string& name) = 0;
	public:
		//From Overload Engine
		/**
		* Set a shader uniform value
		* @param p_key
		* @param p_value
		*/
		template<typename T> void Set(const std::string p_key, const T& p_value);

		/**
		* Set a shader uniform value
		* @param p_key
		*/
		template<typename T> const T& Get(const std::string p_key);
		/**
		* Defines if the material is blendable
		* @param p_blendable
		*/
		void SetBlendable(bool p_blendable);

		/**
		* Defines if the material has backface culling
		* @param p_backfaceCulling
		*/
		void SetBackfaceCulling(bool p_backfaceCulling);

		/**
		* Defines if the material has frontface culling
		* @param p_frontfaceCulling
		*/
		void SetFrontfaceCulling(bool p_frontfaceCulling);

		/**
		* Defines if the material has depth test
		* @param p_depthTest
		*/
		void SetDepthTest(bool p_depthTest);

		/**
		* Defines if the material has depth writting
		* @param p_depthWriting
		*/
		void SetDepthWriting(bool p_depthWriting);

		/**
		* Defines if the material has color writting
		* @param p_colorWriting
		*/
		void SetColorWriting(bool p_colorWriting);

		/**
		* Defines the number of instances
		* @param p_instances
		*/
		void SetGPUInstances(int p_instances);

		/**
		* Returns true if the material is blendable
		*/
		bool IsBlendable() const;

		/**
		* Returns true if the material has backface culling
		*/
		bool HasBackfaceCulling() const;

		/**
		* Returns true if the material has frontface culling
		*/
		bool HasFrontfaceCulling() const;

		/**
		* Returns true if the material has depth test
		*/
		bool HasDepthTest() const;

		/**
		* Returns true if the material has depth writing
		*/
		bool HasDepthWriting() const;

		/**
		* Returns true if the material has color writing
		*/
		bool HasColorWriting() const;

		/**
		* Returns the number of instances
		*/
		int GetGPUInstances() const;

		/**
		* Generate an OpenGL state mask with the current material settings
		*/
		uint8_t GenerateStateMask() const;

		/**
		* Returns the uniforms data of the material
		*/
		std::map<std::string, std::any>& GetUniformsData();

	public:
		void Initialize();
	public:
		std::vector<MaterialTexture> mTextures;

		bool bUseAlbedoMap = false;
		glm::vec4 col = { 1.0f, 1.0f, 1.0f, 1.0f }; // 0 ~ 1

		//TODO:
		Ref<Texture2D> albedoRGBA = Texture2D::Create(TextureSpecification());
		Ref<Texture2D> mAlbedoMap = Library<Texture2D>::GetInstance().GetDefaultTexture();

		bool bUseNormalMap = false;
		Ref<Texture2D> mNormalMap = Library<Texture2D>::GetInstance().Get("DefaultNormal");

		bool bUseMetallicMap = false;
		float metallic = 0.1f;
		Ref<Texture2D> metallicRGBA = Texture2D::Create(TextureSpecification());
		Ref<Texture2D> mMetallicMap = Library<Texture2D>::GetInstance().Get("DefaultMetallicRoughness");

		bool bUseRoughnessMap = false;
		float roughness = 0.9f;
		Ref<Texture2D> roughnessRGBA = Texture2D::Create(TextureSpecification());
		Ref<Texture2D> mRoughnessMap = Library<Texture2D>::GetInstance().Get("DefaultMetallicRoughness");

		bool bUseAoMap = false;
		Ref<Texture2D> mAoMap = Library<Texture2D>::GetInstance().GetWhiteTexture();
	private:
		Ref<Shader> mShader;
		std::unordered_map<ConcreteTextureType, Ref<Texture2D>, EnumClassHash> mTexMap;


	private:
		Shader* m_shader = nullptr;
		std::map<std::string, std::any> m_uniformsData;

		bool m_blendable = false;
		bool m_backfaceCulling = true;
		bool m_frontfaceCulling = false;
		bool m_depthTest = true;
		bool m_depthWriting = true;
		bool m_colorWriting = true;
		int m_gpuInstances = 1;

	};

}
