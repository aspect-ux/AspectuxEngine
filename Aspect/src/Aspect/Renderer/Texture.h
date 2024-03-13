#pragma once

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Asset/Asset.h"
#include "Aspect/Renderer/Image.h"

#include <string>
#include <imgui.h>

namespace Aspect {

	struct TextureSpecification
	{
		ImageFormat Format = ImageFormat::RGBA;
		uint32_t Width = 1;
		uint32_t Height = 1;
		TextureWrap SamplerWrap = TextureWrap::Repeat;
		TextureFilter SamplerFilter = TextureFilter::Linear;

		bool GenerateMips = true;
		bool SRGB = false;
		bool Storage = false;
		bool StoreLocally = false;

		std::string DebugName;
	};

	// 虚函数继承后可以实现可以不实现，但是纯虚函数必须实现
	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual const std::string& GetPath() const { return ""; } //TODO: =0

		virtual void SetData(void* data, uint32_t size){}

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void UnBind() const = 0;

		virtual bool Loaded() const { return false; }

		virtual bool operator==(const Texture& other) const = 0;

		//TODO:
		/*virtual TextureType GetType() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;
		virtual std::pair<uint32_t, uint32_t> GetMipSize(uint32_t mip) const = 0;

		virtual uint64_t GetHash() const = 0;
		virtual uint32_t GetID() const = 0;
		virtual ImageFormat GetFormat() const = 0;*/
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(const TextureSpecification& specification, const std::filesystem::path& filepath);

		// Just for vulkan
		virtual void Resize(const glm::uvec2& size) {}
		virtual void Resize(const uint32_t width, const uint32_t height) {}
		// TODO:=============
		/*virtual AspectRef<Image2D> GetImage() const = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual Buffer GetWriteableBuffer() = 0;

		virtual const std::string& GetPath() const = 0;*/

		//virtual TextureType GetType() const override { return TextureType::Texture2D; }
		//virtual ImTextureID GetImTextureID();
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(const TextureSpecification& specification, Buffer imageData = Buffer());

		// TODO:Repair
		//virtual TextureType GetType() const override { return TextureType::TextureCube; }

		static AssetType GetStaticType() { return AssetType::EnvMap; }
		//virtual AssetType GetAssetType() const override { return GetStaticType(); }
	};

	class Texture3D : public Texture
	{
	public:
		static Ref<Texture3D> Create(uint32_t width, uint32_t height);
	};


	enum class FaceTarget : uint32_t
	{
		Right = 0,
		Left = 1,
		Top = 2,
		Bottom = 3,
		Front = 4,
		Back = 5,
	};

	class CubeMapTexture : public Texture
	{
	public:
		virtual void SetFace(FaceTarget faceIndex, const std::string& path) = 0;
		virtual void GenerateMipmap() = 0;
		static Ref<CubeMapTexture> Create(std::vector<std::string>& paths);
		static Ref<CubeMapTexture> Create();
		static Ref<CubeMapTexture> Create(uint32_t width, uint32_t height);

		virtual void Generate() = 0;

		[[nodiscard]] virtual std::vector<std::string> GetPaths() = 0;
	};
}
