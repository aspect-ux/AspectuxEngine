#pragma once

#include "Aspect/Core/Base/PublicSingleton.h"
#include "Aspect/Core/Ref.h"
#include "Aspect/Asset/Asset.h"
#include "Aspect/Project/Project.h"

#include <filesystem>

namespace Aspect
{
	class AssetManager : public PublicSingleton<AssetManager>
	{
	public:
		[[nodiscard]] static std::filesystem::path GetFullPath(const std::string& RelativePath);
		[[nodiscard]] inline static std::string GetFullStringPath(const std::string& RelativePath) {
			return GetFullPath(RelativePath).string();
		}

		//==============================TODO:======================================
		
	public:
		using AssetsChangeEventFn = std::function<void(const std::vector<FileSystemChangedEvent>&)>;
	public:
		
		static bool IsAssetHandleValid(AssetHandle assetHandle) { return Project::GetAssetManager()->IsAssetHandleValid(assetHandle); }
		
		static bool ReloadData(AssetHandle assetHandle) { return Project::GetAssetManager()->ReloadData(assetHandle); }

		static AssetType GetAssetType(AssetHandle assetHandle) { return Project::GetAssetManager()->GetAssetType(assetHandle); }
		
		template<typename T>
		static AspectRef<T> GetAsset(AssetHandle assetHandle)
		{
			//static std::mutex mutex;
			//std::scoped_lock<std::mutex> lock(mutex);

			AspectRef<Asset> asset = Project::GetAssetManager()->GetAsset(assetHandle);
			return asset.As<T>();
		}
	
		template<typename T>
		static std::unordered_set<AssetHandle> GetAllAssetsWithType()
		{
			return Project::GetAssetManager()->GetAllAssetsWithType(T::GetStaticType());
		}
		
		static const std::unordered_map<AssetHandle, AspectRef<Asset>>& GetLoadedAssets() { return Project::GetAssetManager()->GetLoadedAssets(); }
		static const std::unordered_map<AssetHandle, AspectRef<Asset>>& GetMemoryOnlyAssets() { return Project::GetAssetManager()->GetMemoryOnlyAssets(); }
		
		template<typename TAsset, typename... TArgs>
		static AssetHandle CreateMemoryOnlyAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			Ref<TAsset> asset = Ref<TAsset>::Create(std::forward<TArgs>(args)...);
			asset->Handle = AssetHandle(); // NOTE(Yan): should handle generation happen here?

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		static AssetHandle CreateMemoryOnlyRendererAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			AspectRef<TAsset> asset = TAsset::Create(std::forward<TArgs>(args)...);
			asset->Handle = AssetHandle();

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}
		
		template<typename TAsset, typename... TArgs>
		static AssetHandle CreateMemoryOnlyAssetWithHandle(AssetHandle handle, TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			AspectRef<TAsset> asset = AspectRef<TAsset>::Create(std::forward<TArgs>(args)...);
			asset->Handle = handle;

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset>
		static AssetHandle AddMemoryOnlyAsset(AspectRef<TAsset> asset)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "AddMemoryOnlyAsset only works for types derived from Asset");
			asset->Handle = AssetHandle(); // NOTE(Yan): should handle generation happen here?

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		static bool IsMemoryAsset(AssetHandle handle) { return Project::GetAssetManager()->IsMemoryAsset(handle); }
	};
}
