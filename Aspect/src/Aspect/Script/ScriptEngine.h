#pragma once

#include "Aspect/Scene/Scene.h"
#include "Aspect/Scene/Entity.h"

#include <filesystem>
#include <string>
#include <map>

#include "ScriptAsset.h"
#include "ScriptCache.h"
#include "ScriptTypes.h"

#include "Aspect/Core/Hash.h"
#include "Aspect/Debug/Profiler.h"
#include "Aspect/Scene/Components.h"
#include "Aspect/Scene/Scene.h"
#include "Aspect/Utilities/FileSystem.h"

/*
extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoString MonoString;
}
*/
//=============TODO:===============
#define ASPECT_CORE_ASSEMBLY_INDEX 0
#define ASPECT_APP_ASSEMBLY_INDEX ASPECT_CORE_ASSEMBLY_INDEX + 1
#define ASPECT_MAX_ASSEMBLIES (size_t)2
extern "C" {
	typedef struct _MonoDomain MonoDomain;
}

namespace Aspect {

	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;
		
		MonoClassField* ClassField;
	};

	// ScriptField + data storage
	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[16];

		friend class ScriptEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }
	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		std::map<std::string, ScriptField> m_Fields;

		MonoClass* m_MonoClass = nullptr;

		friend class ScriptEngine;
	};

	class ScriptInstance  
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);

		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);	
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");

			SetFieldValueInternal(name, &value);
		}

		MonoObject* GetManagedObject() { return m_Instance; }
	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		inline static char s_FieldValueBuffer[16];

		friend class ScriptEngine;
		friend struct ScriptFieldInstance;
	};

	//=====================TODO:==========================
	using ScriptEntityMap = std::unordered_map<UUID, std::vector<UUID>>;
	using ScriptInstanceMap = std::unordered_map<UUID, GCHandle>;
	using AssembliesArray = std::array<AspectRef<AssemblyInfo>, ASPECT_MAX_ASSEMBLIES>;

	struct ScriptEngineConfig
	{
		std::filesystem::path CoreAssemblyPath;
		bool EnableDebugging;
		bool EnableProfiling;
	};
	//=====================TODO:==========================

	class ScriptEngine
	{
	public:
		static void Init(const ScriptEngineConfig& config);
		static void Shutdown();

		//TODO:
		static void InitializeRuntime();
		static void ShutdownRuntime();
		static void ShutdownRuntimeInstance(Entity entity);
		static void InitializeScriptEntity(Entity entity);
		static void RuntimeInitializeScriptEntity(Entity entity);
		static void DuplicateScriptInstance(Entity entity, Entity targetEntity);
		static void ShutdownScriptEntity(Entity entity, bool erase = true);

		// load assembly
		static bool LoadAssembly(const std::filesystem::path& filepath);

		// load app assembly
		static bool LoadAppAssembly();
		static bool LoadAppAssembly(const std::filesystem::path& filepath);
		static bool LoadAppAssemblyRuntime(Buffer appAssemblyData);
		static bool ReloadAppAssembly(const bool scheduleReload = false);

		//scene context: to be fixed
		static void SetSceneContext(const AspectRef<Scene>& scene, const Ref<SceneRenderer>& sceneRenderer);
		static AspectRef<Scene> GetSceneContext();
		static Ref<SceneRenderer> GetSceneRenderer();

		//TODO: Repair
		static bool ShouldReloadAppAssembly(){}
		static void UnloadAppAssembly(){}

		// get entity instance
		// NOTE(Peter): Pass false as the second parameter if you don't care if OnCreate has been called yet
		static bool IsEntityInstantiated(Entity entity, bool checkOnCreateCalled = true);
		static GCHandle GetEntityInstance(UUID entityID);
		static const std::unordered_map<UUID, GCHandle>& GetEntityInstances();

		// get field storage
		static AspectRef<FieldStorageBase> GetFieldStorage(Entity entity, uint32_t fieldID);
		static void InitializeRuntimeDuplicatedEntities();

		
		static MonoObject* CreateManagedObject(ManagedClass* managedClass);

		//TODO:
		template<typename... TConstructorArgs>
		static MonoObject* CreateManagedObject(const std::string& className, TConstructorArgs&&... args)
		{
			return CreateManagedObject_Internal(ScriptCache::GetManagedClassByID(AS_SCRIPT_CLASS_ID(className)), std::forward<TConstructorArgs>(args)...);
		}

		template<typename... TConstructorArgs>
		static MonoObject* CreateManagedObject(uint32_t classID, TConstructorArgs&&... args)
		{
			return CreateManagedObject_Internal(ScriptCache::GetManagedClassByID(classID), std::forward<TConstructorArgs>(args)...);
		}
		
		static void ReloadAssembly(){}
		//static bool ReloadAppAssembly(const bool scheduleReload = false);

		static void CallMethod(MonoObject* monoObject, ManagedMethod* managedMethod, const void** parameters);

		// TODO:
		template<typename... TArgs>
		static void CallMethod(MonoObject* managedObject, const std::string& methodName, TArgs&&... args)
		{
			AS_PROFILE_SCOPE_DYNAMIC(methodName.c_str());

			if (managedObject == nullptr)
			{
				AS_CORE_WARN_TAG("ScriptEngine", "Attempting to call method {0} on an invalid instance!", methodName);
				return;
			}

			constexpr size_t argsCount = sizeof...(args);

			ManagedClass* clazz = ScriptCache::GetMonoObjectClass(managedObject);
			if (clazz == nullptr)
			{
				AS_CORE_ERROR_TAG("ScriptEngine", "Failed to find ManagedClass!");
				return;
			}

			ManagedMethod* method = ScriptCache::GetSpecificManagedMethod(clazz, methodName, argsCount);
			if (method == nullptr)
			{
				AS_CORE_ERROR_TAG("ScriptEngine", "Failed to find a C# method called {0} with {1} parameters", methodName, argsCount);
				return;
			}

			if constexpr (argsCount > 0)
			{
				const void* data[] = { &args... };
				CallMethod(managedObject, method, data);
			}
			else
			{
				CallMethod(managedObject, method, nullptr);
			}
		}

		template<typename... TArgs>
		static void CallMethod(GCHandle instance, const std::string& methodName, TArgs&&... args)
		{
			if (instance == nullptr)
			{
				AS_CORE_WARN_TAG("ScriptEngine", "Attempting to call method {0} on an invalid instance!", methodName);
				return;
			}

			CallMethod(GCManager::GetReferencedObject(instance), methodName, std::forward<TArgs>(args)...);
		}

		//static AspectRef<FieldStorageBase> GetFieldStorage(Entity entity, uint32_t fieldID);

		//TODO: set old functions
		//static void OnRuntimeStart(Scene* scene){}
		static void OnRuntimeStop(){}

		static bool EntityClassExists(const std::string& fullClassName) { return false; }
		static void OnCreateEntity(Entity entity){}
		//static void OnUpdateEntity(Entity entity, Timestep ts){}

		//static Scene* GetSceneContext() { return nullptr; }
		static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);
		
		static Ref<ScriptClass> GetEntityClass(const std::string& name);
		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
		static ScriptFieldMap& GetScriptFieldMap(Entity entity);
		
		static MonoImage* GetCoreAssemblyImage();

		static MonoObject* GetManagedInstance(UUID uuid);

		//static void DuplicateScriptInstance(Entity entity, Entity targetEntity);

		//static MonoString* CreateString(const char* string) { return nullptr; }

		static AspectRef<AssemblyInfo> GetCoreAssemblyInfo();
		static AspectRef<AssemblyInfo> GetAppAssemblyInfo();

		static const ScriptEngineConfig& GetConfig();

		static uint32_t GetScriptClassIDFromComponent(const ScriptComponent& sc);
		static bool IsModuleValid(AssetHandle scriptAssetHandle);

		static MonoDomain* GetScriptDomain();

	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);
		static MonoAssembly* LoadMonoAssemblyRuntime(Buffer assemblyData);

		static bool LoadCoreAssembly();
		static void UnloadAssembly(AspectRef<AssemblyInfo> assemblyInfo);

		static void LoadReferencedAssemblies(const AspectRef<AssemblyInfo>& assemblyInfo);
		static AssemblyMetadata GetMetadataForImage(MonoImage* image);
		static std::vector<AssemblyMetadata> GetReferencedAssembliesMetadata(MonoImage* image);

		static void InitRuntimeObject(MonoObject* monoObject);

		static void OnFileSystemChanged(const std::vector<FileSystemChangedEvent>& events);

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses();

	private:
		template<typename... TConstructorArgs>
		static MonoObject* CreateManagedObject_Internal(ManagedClass* managedClass, TConstructorArgs&&... args)
		{
			AS_PROFILE_SCOPE_DYNAMIC(managedClass->FullName.c_str());

			if (managedClass == nullptr)
			{
				AS_CORE_ERROR_TAG("ScriptEngine", "Attempting to create managed object with a null class!");
				return nullptr;
			}

			if (managedClass->IsAbstract)
				return nullptr;

			MonoObject* obj = CreateManagedObject(managedClass);

			if (managedClass->IsStruct)
				return obj;

			//if (ManagedType::FromClass(managedClass).IsValueType())
			//	return obj;

			constexpr size_t argsCount = sizeof...(args);
			ManagedMethod* ctor = ScriptCache::GetSpecificManagedMethod(managedClass, ".ctor", argsCount);

			InitRuntimeObject(obj);

			if constexpr (argsCount > 0)
			{
				if (ctor == nullptr)
				{
					AS_CORE_ERROR_TAG("ScriptEngine", "Failed to call constructor with {} parameters for class '{}'.", argsCount, managedClass->FullName);
					return obj;
				}

				const void* data[] = { &args... };
				CallMethod(obj, ctor, data);
			}

			return obj;
		}

		friend class ScriptClass;
		friend class ScriptGlue;
		friend class ScriptCache;
		friend class ScriptUtils;
		friend class Application;
	};
	/*
	namespace Utils {

		inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
		{
			switch (fieldType)
			{
				case ScriptFieldType::None:    return "None";
				case ScriptFieldType::Float:   return "Float";
				case ScriptFieldType::Double:  return "Double";
				case ScriptFieldType::Bool:    return "Bool";
				case ScriptFieldType::Char:    return "Char";
				case ScriptFieldType::Byte:    return "Byte";
				case ScriptFieldType::Short:   return "Short";
				case ScriptFieldType::Int:     return "Int";
				case ScriptFieldType::Long:    return "Long";
				case ScriptFieldType::UByte:   return "UByte";
				case ScriptFieldType::UShort:  return "UShort";
				case ScriptFieldType::UInt:    return "UInt";
				case ScriptFieldType::ULong:   return "ULong";
				case ScriptFieldType::Vector2: return "Vector2";
				case ScriptFieldType::Vector3: return "Vector3";
				case ScriptFieldType::Vector4: return "Vector4";
				case ScriptFieldType::Entity:  return "Entity";
			}
			AS_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return "None";
		}

		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")    return ScriptFieldType::None;
			if (fieldType == "Float")   return ScriptFieldType::Float;
			if (fieldType == "Double")  return ScriptFieldType::Double;
			if (fieldType == "Bool")    return ScriptFieldType::Bool;
			if (fieldType == "Char")    return ScriptFieldType::Char;
			if (fieldType == "Byte")    return ScriptFieldType::Byte;
			if (fieldType == "Short")   return ScriptFieldType::Short;
			if (fieldType == "Int")     return ScriptFieldType::Int;
			if (fieldType == "Long")    return ScriptFieldType::Long;
			if (fieldType == "UByte")   return ScriptFieldType::UByte;
			if (fieldType == "UShort")  return ScriptFieldType::UShort;
			if (fieldType == "UInt")    return ScriptFieldType::UInt;
			if (fieldType == "ULong")   return ScriptFieldType::ULong;
			if (fieldType == "Vector2") return ScriptFieldType::Vector2;
			if (fieldType == "Vector3") return ScriptFieldType::Vector3;
			if (fieldType == "Vector4") return ScriptFieldType::Vector4;
			if (fieldType == "Entity")  return ScriptFieldType::Entity;

			AS_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return ScriptFieldType::None;
		}

	}
	*/
}
