#include "aspch.h"

#include "Aspect/Asset/AssetManager.h"
#include "Aspect/Library/TextureLibrary.h"
#include "Aspect/Renderer/Renderer.h"

namespace Aspect
{
	Library<Texture2D>::Library()
	{
		Add("DefaultTexture", Texture2D::Create(std::string("Resources/Test/Textures/DefaultTexture.png")));
		Add("DefaultNormal", Texture2D::Create(std::string("Resources/Test/Textures/DefaultNormal.png")));
		Add("DefaultMetallicRoughness", Texture2D::Create(std::string("Resources/Test/Textures/black.png")));
		Add("DefaultHdr", Texture2D::Create(std::string("Resources/Test/Textures/DefaultTexture.png")));
		Add("BRDF_LUT", Texture2D::Create(std::string("Resources/Test/Textures/BRDF_LUT.tga")));

		Ref<Texture2D> whiteTexture = Texture2D::Create(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		Add("WhiteTexture", whiteTexture);

		Ref<Texture2D> blackTexture = Texture2D::Create(TextureSpecification());
		uint32_t blackTextureData = 0x00000000;
		blackTexture->SetData(&blackTextureData, sizeof(uint32_t));
		Add("BlackTexture", blackTexture);

		/*Add("WhiteTexture", Renderer::GetBlackTexture());
		Add("BlackTexture", Renderer::GetBlackTexture());*/
		Add("DefaultTexture", Texture2D::Create(std::string("Resources/Test/Textures/DefaultTexture.png")));
	}

	Ref<Texture2D> Library<Texture2D>::GetDefaultTexture()
	{
		return m_Library["DefaultTexture"];
	}

	Ref<Texture2D> Library<Texture2D>::GetWhiteTexture()
	{
		return m_Library["WhiteTexture"];
	}
}
