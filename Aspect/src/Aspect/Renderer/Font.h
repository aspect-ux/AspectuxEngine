#pragma once

#include <filesystem>

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Renderer/Texture.h"

namespace Aspect {

	struct MSDFData;

	class Font 
	{
	public:
		Font(const std::filesystem::path& font);
		~Font();

		const MSDFData* GetMSDFData() const { return m_Data; }
		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

		static Ref<Font> GetDefault();
	private:
		MSDFData* m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};

}
