#pragma once

#include "ScriptTypes.h"
#include "FieldStorage.h"

#define AS_CORE_CLASS(clazz) ScriptCache::GetManagedClassByName("Aspect."#clazz)
#define AS_CACHED_CLASS(clazz) ScriptCache::GetManagedClassByName(clazz)
#define AS_CACHED_CLASS_RAW(clazz) ScriptCache::GetManagedClassByName(clazz)->Class
#define AS_CACHED_METHOD(clazz, method, paramCount) ScriptCache::GetSpecificManagedMethod(ScriptCache::GetManagedClassByName(clazz), method, paramCount)
#define AS_CACHED_FIELD(clazz, field) ScriptCache::GetFieldByName(ScriptCache::GetManagedClassByName(clazz), field)
#define AS_CACHED_FIELD_STORAGE(clazz, field) ScriptCache::GetFieldStorage(ScriptCache::GetFieldByName(ScriptCache::GetManagedClassByName(clazz), field)->ID)
//#define AS_TRY_GET_FIELD_VALUE(ret, className, fieldName, storageObj){\
//																		FieldInfo* field = ScriptCache::GetFieldByName(ScriptCache::GetManagedClassByName(className), fieldName);\
//																		Ref<FieldStorageBase> fieldStorage = ScriptCache::GetFieldStorage(field->ID);\
//																		if (!fieldStorage->IsArray())\
//																		{\
//																			ret = fieldStorage.As<FieldStorage>()->GetValueForObject<decltype(ret)>(storageObj);\
//																		}\
//																	 }
#define AS_SCRIPT_CLASS_ID(name) Hash::GenerateFNVHash(name)

namespace Aspect {

	class ScriptCache
	{
	public:
		static void Init();
		static void Shutdown();

		static void CacheCoreClasses();
		static void ClearCache();

		static void GenerateCacheForAssembly(AspectRef<AssemblyInfo> assemblyInfo);

		static ManagedClass* GetManagedClassByName(const std::string& className);
		static ManagedClass* GetManagedClassByID(uint32_t classID);
		static ManagedClass* GetManagedClass(MonoClass* monoClass);
		static ManagedClass* GetMonoObjectClass(MonoObject* monoObject);
		static FieldInfo* GetFieldByID(uint32_t fieldID);
		static FieldInfo* GetFieldByName(const ManagedClass* managedClass, const std::string& fieldName);
		static MonoClass* GetFieldTypeClass(FieldType fieldType);

		// This version of the method returns ANY method that matches the given name
		static ManagedMethod* GetManagedMethod(ManagedClass* managedClass, const std::string& name, bool ignoreParent = false);
		static ManagedMethod* GetSpecificManagedMethod(ManagedClass* managedClass, const std::string& name, uint32_t parameterCount, bool ignoreParent = false);

	private:
		static void CacheClass(std::string_view className, MonoClass* monoClass);
		static void CacheClassMethods(AspectRef<AssemblyInfo> assemblyInfo, ManagedClass& managedClass);
		static void CacheClassFields(AspectRef<AssemblyInfo> assemblyInfo, ManagedClass& managedClass);
		static void CacheClassProperties(AspectRef<AssemblyInfo> assemblyInfo, ManagedClass& managedClass);

		static const std::unordered_map<uint32_t, ManagedClass>& GetCachedClasses();
		static const std::unordered_map<uint32_t, FieldInfo>& GetCachedFields();
		static const std::unordered_map<uint32_t, std::vector<ManagedMethod>>& GetCachedMethods();

		friend class ScriptEngineDebugPanel;
	};
}
