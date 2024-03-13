#pragma once

#include "Aspect/Asset/AssetSerializer.h"

#include "Aspect/Serialization/FileStream.h"
#include "Aspect/Scene/Scene.h"

namespace Aspect {

	class AssetImporter
	{
	public:
		static void Init();
		static void Serialize(const AssetMetadata& metadata, const AspectRef<Asset>& asset);
		static void Serialize(const AspectRef<Asset>& asset);
		static bool TryLoadData(const AssetMetadata& metadata, AspectRef<Asset>& asset);

		/*static bool SerializeToAssetPack(AssetHandle handle, FileStreamWriter& stream, AssetSerializationInfo& outInfo);
		static Ref<Asset> DeserializeFromAssetPack(FileStreamReader& stream, const AssetPackFile::AssetInfo& assetInfo);
		static AspectRef<Scene> DeserializeSceneFromAssetPack(FileStreamReader& stream, const AssetPackFile::SceneInfo& assetInfo);*/
	private:
		static std::unordered_map<AssetType, Scope<AssetSerializer>> s_Serializers;
	};

}
