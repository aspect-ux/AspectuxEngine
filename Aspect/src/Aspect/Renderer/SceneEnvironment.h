/**
 * @package H2M (Aspect to Morava)
 * @author  Yan Chernikov (TheCherno)
 * @licence Apache License 2.0
 */

#pragma once

#include "Texture.h"

namespace Aspect {

	struct Environment : public Asset
	{
		std::string FilePath;
		AspectRef<TextureCube> RadianceMap;
		AspectRef<TextureCube> IrradianceMap;

		Environment() = default;
		Environment(const AspectRef<TextureCube>& radianceMap, const AspectRef<TextureCube>& irradianceMap)
			: RadianceMap(radianceMap), IrradianceMap(irradianceMap) {}

		Environment(std::string filePath, const AspectRef<TextureCube>& radianceMap, const AspectRef<TextureCube>& irradianceMap)
			: FilePath(filePath), RadianceMap(radianceMap), IrradianceMap(irradianceMap) {}

		static Environment Load(const std::string& filepath);
		static AssetType GetStaticType() { return AssetType::EnvMap; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }

	};

}
