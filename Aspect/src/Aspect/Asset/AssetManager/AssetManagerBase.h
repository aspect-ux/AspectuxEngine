#pragma once

#include "Aspect/Asset/Asset.h"
#include "Aspect/Asset/AssetTypes.h"

#include <unordered_set>
#include <unordered_map>

namespace Aspect {

	//////////////////////////////////////////////////////////////////
	// AssetManagerBase //////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	// Implementation in RuntimeAssetManager and EditorAssetManager //
	// Static wrapper in AssetManager ////////////////////////////////
	//////////////////////////////////////////////////////////////////
	class AssetManagerBase : public RefCounted
	{
	public:
		AssetManagerBase() = default;
		virtual ~AssetManagerBase() = default;

		virtual AssetType GetAssetType(AssetHandle assetHandle) = 0;
		virtual AspectRef<Asset> GetAsset(AssetHandle assetHandle) = 0;
		virtual void AddMemoryOnlyAsset(AspectRef<Asset> asset) = 0;
		virtual bool ReloadData(AssetHandle assetHandle) = 0;
		virtual bool IsAssetHandleValid(AssetHandle assetHandle) = 0;
		virtual bool IsMemoryAsset(AssetHandle handle) = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) = 0;

		virtual std::unordered_set<AssetHandle> GetAllAssetsWithType(AssetType type) = 0;
		virtual const std::unordered_map<AssetHandle, AspectRef<Asset>>& GetLoadedAssets() = 0;
		virtual const std::unordered_map<AssetHandle, AspectRef<Asset>>& GetMemoryOnlyAssets() = 0;
	};

}
