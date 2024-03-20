#include "aspch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "Aspect/Script/ScriptEngine.h"
#include "Aspect/Core/UUID.h"

#include "Aspect/Project/Project.h"
#include "Aspect/Utilities/StringUtils.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include "Aspect/Library/Library.h"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Aspect::UUID>
	{
		static Node encode(const Aspect::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, Aspect::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};

}

namespace Aspect {

#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << scriptField.GetValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = scriptField["Data"].as<Type>();    \
		fieldInstance.SetValue(data);                  \
		break;                                         \
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:    return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		AS_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;
	
		AS_CORE_ASSERT(false, "Unknown body type");
		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const AspectRef<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		AS_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}
		/*TODO:
		if (entity.HasComponent<ScriptComponent>())
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent
			//TODO:
			//out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

			// Fields
			Ref<ScriptClass> entityClass;// = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
			const auto& fields = entityClass->GetFields();
			if (fields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields)
				{
					if (entityFields.find(name) == entityFields.end())
						continue;

					out << YAML::BeginMap; // ScriptField
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptField = entityFields.at(name);

					switch (field.Type)
					{
						WRITE_SCRIPT_FIELD(Float,   float     );
						WRITE_SCRIPT_FIELD(Double,  double    );
						WRITE_SCRIPT_FIELD(Bool,    bool      );
						WRITE_SCRIPT_FIELD(Char,    char      );
						WRITE_SCRIPT_FIELD(Byte,    int8_t    );
						WRITE_SCRIPT_FIELD(Short,   int16_t   );
						WRITE_SCRIPT_FIELD(Int,     int32_t   );
						WRITE_SCRIPT_FIELD(Long,    int64_t   );
						WRITE_SCRIPT_FIELD(UByte,   uint8_t   );
						WRITE_SCRIPT_FIELD(UShort,  uint16_t  );
						WRITE_SCRIPT_FIELD(UInt,    uint32_t  );
						WRITE_SCRIPT_FIELD(ULong,   uint64_t  );
						WRITE_SCRIPT_FIELD(Vector2, glm::vec2 );
						WRITE_SCRIPT_FIELD(Vector3, glm::vec3 );
						WRITE_SCRIPT_FIELD(Vector4, glm::vec4 );
						WRITE_SCRIPT_FIELD(Entity,  UUID      );
					}
					out << YAML::EndMap; // ScriptFields
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap; // ScriptComponent
		}*/


		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent

			const auto& sc = entity.GetComponent<ScriptComponent>();

			ManagedClass* scriptClass = ScriptCache::GetManagedClassByID(ScriptEngine::GetScriptClassIDFromComponent(sc));
			out << YAML::Key << "ClassHandle" << YAML::Value << sc.ScriptClassHandle;
			out << YAML::Key << "Name" << YAML::Value << (scriptClass ? scriptClass->FullName : "Null");

			if (sc.FieldIDs.size() > 0)
			{
				out << YAML::Key << "StoredFields" << YAML::Value;
				out << YAML::BeginSeq;

				for (auto fieldID : sc.FieldIDs)
				{
					FieldInfo* fieldInfo = ScriptCache::GetFieldByID(fieldID);

					if (!fieldInfo->IsWritable())
						continue;

					AspectRef<FieldStorageBase> storage = ScriptEngine::GetFieldStorage(entity, fieldID);

					if (!storage)
						continue;

					out << YAML::BeginMap; // Field
					out << YAML::Key << "ID" << YAML::Value << fieldInfo->ID;
					out << YAML::Key << "Name" << YAML::Value << fieldInfo->Name; // This is only here for the sake of debugging. All we need is the ID
					out << YAML::Key << "Type" << YAML::Value << FieldUtils::FieldTypeToString(fieldInfo->Type);

					if (fieldInfo->IsArray())
						out << YAML::Key << "Length" << YAML::Value << storage.As<ArrayFieldStorage>()->GetLength(); // Not strictly necessary but useful for readability

					out << YAML::Key << "Data" << YAML::Value;

					if (fieldInfo->IsArray())
					{
						out << YAML::BeginSeq;

						AspectRef<ArrayFieldStorage> arrayStorage = storage.As<ArrayFieldStorage>();

						for (uintptr_t i = 0; i < arrayStorage->GetLength(); i++)
						{
							switch (fieldInfo->Type)
							{
							case FieldType::Bool:
							{
								out << arrayStorage->GetValue<bool>(i);
								break;
							}
							case FieldType::Int8:
							{
								out << arrayStorage->GetValue<int8_t>(i);
								break;
							}
							case FieldType::Int16:
							{
								out << arrayStorage->GetValue<int16_t>(i);
								break;
							}
							case FieldType::Int32:
							{
								out << arrayStorage->GetValue<int32_t>(i);
								break;
							}
							case FieldType::Int64:
							{
								out << arrayStorage->GetValue<int64_t>(i);
								break;
							}
							case FieldType::UInt8:
							{
								out << arrayStorage->GetValue<uint8_t>(i);
								break;
							}
							case FieldType::UInt16:
							{
								out << arrayStorage->GetValue<uint16_t>(i);
								break;
							}
							case FieldType::UInt32:
							{
								out << arrayStorage->GetValue<uint32_t>(i);
								break;
							}
							case FieldType::UInt64:
							{
								out << arrayStorage->GetValue<uint64_t>(i);
								break;
							}
							case FieldType::Float:
							{
								out << arrayStorage->GetValue<float>(i);
								break;
							}
							case FieldType::Double:
							{
								out << arrayStorage->GetValue<double>(i);
								break;
							}
							case FieldType::String:
							{
								out << arrayStorage->GetValue<std::string>(i);
								break;
							}
							case FieldType::Vector2:
							{
								out << arrayStorage->GetValue<glm::vec2>(i);
								break;
							}
							case FieldType::Vector3:
							{
								out << arrayStorage->GetValue<glm::vec3>(i);
								break;
							}
							case FieldType::Vector4:
							{
								out << arrayStorage->GetValue<glm::vec4>(i);
								break;
							}
							case FieldType::Prefab:
							case FieldType::Entity:
							case FieldType::Mesh:
							case FieldType::StaticMesh:
							case FieldType::Material:
							case FieldType::PhysicsMaterial:
							case FieldType::Texture2D:
							{
								out << arrayStorage->GetValue<UUID>(i);
								break;
							}
							}
						}

						out << YAML::EndSeq;
					}
					else
					{
						AspectRef<FieldStorage> fieldStorage = storage.As<FieldStorage>();
						switch (fieldInfo->Type)
						{
						case FieldType::Bool:
						{
							out << fieldStorage->GetValue<bool>();
							break;
						}
						case FieldType::Int8:
						{
							out << fieldStorage->GetValue<int8_t>();
							break;
						}
						case FieldType::Int16:
						{
							out << fieldStorage->GetValue<int16_t>();
							break;
						}
						case FieldType::Int32:
						{
							out << fieldStorage->GetValue<int32_t>();
							break;
						}
						case FieldType::Int64:
						{
							out << fieldStorage->GetValue<int64_t>();
							break;
						}
						case FieldType::UInt8:
						{
							out << fieldStorage->GetValue<uint8_t>();
							break;
						}
						case FieldType::UInt16:
						{
							out << fieldStorage->GetValue<uint16_t>();
							break;
						}
						case FieldType::UInt32:
						{
							out << fieldStorage->GetValue<uint32_t>();
							break;
						}
						case FieldType::UInt64:
						{
							out << fieldStorage->GetValue<uint64_t>();
							break;
						}
						case FieldType::Float:
						{
							out << fieldStorage->GetValue<float>();
							break;
						}
						case FieldType::Double:
						{
							out << fieldStorage->GetValue<double>();
							break;
						}
						case FieldType::String:
						{
							out << fieldStorage->GetValue<std::string>();
							break;
						}
						case FieldType::Vector2:
						{
							out << fieldStorage->GetValue<glm::vec2>();
							break;
						}
						case FieldType::Vector3:
						{
							out << fieldStorage->GetValue<glm::vec3>();
							break;
						}
						case FieldType::Vector4:
						{
							out << fieldStorage->GetValue<glm::vec4>();
							break;
						}
						case FieldType::Prefab:
						case FieldType::Entity:
						case FieldType::Mesh:
						case FieldType::StaticMesh:
						case FieldType::Material:
						case FieldType::PhysicsMaterial:
						case FieldType::Texture2D:
						{
							out << fieldStorage->GetValue<UUID>();
							break;
						}
						}
					}
					out << YAML::EndMap; // Field
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap; // ScriptComponent
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			if (spriteRendererComponent.Texture)
				out << YAML::Key << "TexturePath" << YAML::Value << spriteRendererComponent.Texture->GetPath();

			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap; // CircleRendererComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& cc2dComponent = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.Radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.RestitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		if (entity.HasComponent<TextComponent>())
		{
			out << YAML::Key << "TextComponent";
			out << YAML::BeginMap; // TextComponent

			auto& textComponent = entity.GetComponent<TextComponent>();
			out << YAML::Key << "TextString" << YAML::Value << textComponent.TextString;
			// TODO: textComponent.FontAsset
			out << YAML::Key << "Color" << YAML::Value << textComponent.Color;
			out << YAML::Key << "Kerning" << YAML::Value << textComponent.Kerning;
			out << YAML::Key << "LineSpacing" << YAML::Value << textComponent.LineSpacing;

			out << YAML::EndMap; // TextComponent
		}

		//TODO:delete
		//if (entity.HasComponent<MeshComponent>())
		//{
		//	out << YAML::Key << "MeshComponent";
		//	out << YAML::BeginMap; // TextComponent

		//	auto& meshComponent = entity.GetComponent<MeshComponent>();
		//	out << YAML::Key << "MeshString" << YAML::Value << "mesh";
		//	// TODO: textComponent.FontAsset
		//	out << YAML::Key << "Color" << YAML::Value << meshComponent.Path;

		//	out << YAML::EndMap; // TextComponent
		//}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.Raw() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		AS_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			AS_CORE_ERROR("Failed to load .Aspect file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		AS_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				AS_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				/*TODO:ScriptCompo
				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.ClassName = scriptComponent["ClassName"].as<std::string>();

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
						if (entityClass)
						{
							const auto& fields = entityClass->GetFields();
							auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

							for (auto scriptField : scriptFields)
							{
								std::string name = scriptField["Name"].as<std::string>();
								std::string typeString = scriptField["Type"].as<std::string>();
								ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

								ScriptFieldInstance& fieldInstance = entityFields[name];

								// TODO(Yan): turn this assert into Aspectnut log warning
								AS_CORE_ASSERT(fields.find(name) != fields.end());

								if (fields.find(name) == fields.end())
									continue;

								fieldInstance.Field = fields.at(name);

								switch (type)
								{
									READ_SCRIPT_FIELD(Float, float);
									READ_SCRIPT_FIELD(Double, double);
									READ_SCRIPT_FIELD(Bool, bool);
									READ_SCRIPT_FIELD(Char, char);
									READ_SCRIPT_FIELD(Byte, int8_t);
									READ_SCRIPT_FIELD(Short, int16_t);
									READ_SCRIPT_FIELD(Int, int32_t);
									READ_SCRIPT_FIELD(Long, int64_t);
									READ_SCRIPT_FIELD(UByte, uint8_t);
									READ_SCRIPT_FIELD(UShort, uint16_t);
									READ_SCRIPT_FIELD(UInt, uint32_t);
									READ_SCRIPT_FIELD(ULong, uint64_t);
									READ_SCRIPT_FIELD(Vector2, glm::vec2);
									READ_SCRIPT_FIELD(Vector3, glm::vec3);
									READ_SCRIPT_FIELD(Vector4, glm::vec4);
									READ_SCRIPT_FIELD(Entity, UUID);
								}
							}
						}
					}

				}*/
				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					AssetHandle scriptAssetHandle = scriptComponent["ClassHandle"] ? scriptComponent["ClassHandle"].as<AssetHandle>(AssetHandle(0)) : AssetHandle(0);
					std::string moduleName = scriptComponent["ModuleName"] ? scriptComponent["ModuleName"].as<std::string>("") : "";
					std::string name = scriptComponent["Name"].as<std::string>("");

					if (scriptAssetHandle == 0 && !moduleName.empty())
						scriptAssetHandle = AS_SCRIPT_CLASS_ID(moduleName);

					if (scriptAssetHandle != 0)
					{
						ScriptComponent& sc = deserializedEntity.AddComponent<ScriptComponent>(scriptAssetHandle);
						ScriptEngine::InitializeScriptEntity(deserializedEntity);

						if (sc.FieldIDs.size() > 0)
						{
							auto storedFields = scriptComponent["StoredFields"];
							if (storedFields)
							{
								for (auto field : storedFields)
								{
									uint32_t id = field["ID"].as<uint32_t>(0);
									std::string fullName = field["Name"].as<std::string>();
									std::string name = Utils::String::SubStr(fullName, fullName.find(':') + 1);
									std::string typeStr = field["Type"].as<std::string>("");
									FieldInfo* fieldData = ScriptCache::GetFieldByID(id);
									AspectRef<FieldStorageBase> storage = ScriptEngine::GetFieldStorage(deserializedEntity, id);

									if (storage == nullptr)
									{
										id = Hash::GenerateFNVHash(name);
										storage = ScriptEngine::GetFieldStorage(deserializedEntity, id);
									}

									if (storage == nullptr)
									{
										AS_CONSOLE_LOG_WARN("Serialized C# field {0} doesn't exist in script cache! This could be because the script field no longer exists or because it's been renamed.", name);
									}
									else
									{
										auto dataNode = field["Data"];

										if (fieldData->IsArray() && dataNode.IsSequence())
										{
											AspectRef<ArrayFieldStorage> arrayStorage = storage.As<ArrayFieldStorage>();
											arrayStorage->Resize(dataNode.size());

											for (uintptr_t i = 0; i < dataNode.size(); i++)
											{
												switch (fieldData->Type)
												{
												case FieldType::Bool:
												{
													arrayStorage->SetValue(i, dataNode[i].as<bool>());
													break;
												}
												case FieldType::Int8:
												{
													arrayStorage->SetValue(i, static_cast<int8_t>(dataNode[i].as<int16_t>()));
													break;
												}
												case FieldType::Int16:
												{
													arrayStorage->SetValue(i, dataNode[i].as<int16_t>());
													break;
												}
												case FieldType::Int32:
												{
													arrayStorage->SetValue(i, dataNode[i].as<int32_t>());
													break;
												}
												case FieldType::Int64:
												{
													arrayStorage->SetValue(i, dataNode[i].as<int64_t>());
													break;
												}
												case FieldType::UInt8:
												{
													arrayStorage->SetValue(i, dataNode[i].as<uint8_t>());
													break;
												}
												case FieldType::UInt16:
												{
													arrayStorage->SetValue(i, dataNode[i].as<uint16_t>());
													break;
												}
												case FieldType::UInt32:
												{
													arrayStorage->SetValue(i, dataNode[i].as<uint32_t>());
													break;
												}
												case FieldType::UInt64:
												{
													arrayStorage->SetValue(i, dataNode[i].as<uint64_t>());
													break;
												}
												case FieldType::Float:
												{
													arrayStorage->SetValue(i, dataNode[i].as<float>());
													break;
												}
												case FieldType::Double:
												{
													arrayStorage->SetValue(i, dataNode[i].as<double>());
													break;
												}
												case FieldType::String:
												{
													arrayStorage->SetValue(i, dataNode[i].as<std::string>());
													break;
												}
												case FieldType::Vector2:
												{
													arrayStorage->SetValue(i, dataNode[i].as<glm::vec2>());
													break;
												}
												case FieldType::Vector3:
												{
													arrayStorage->SetValue(i, dataNode[i].as<glm::vec3>());
													break;
												}
												case FieldType::Vector4:
												{
													arrayStorage->SetValue(i, dataNode[i].as<glm::vec4>());
													break;
												}
												case FieldType::Prefab:
												case FieldType::Entity:
												case FieldType::Mesh:
												case FieldType::StaticMesh:
												case FieldType::Material:
												case FieldType::PhysicsMaterial:
												case FieldType::Texture2D:
												{
													arrayStorage->SetValue(i, dataNode[i].as<UUID>());
													break;
												}
												}
											}
										}
										else
										{
											AspectRef<FieldStorage> fieldStorage = storage.As<FieldStorage>();
											switch (fieldData->Type)
											{
											case FieldType::Bool:
											{
												fieldStorage->SetValue(dataNode.as<bool>());
												break;
											}
											case FieldType::Int8:
											{
												fieldStorage->SetValue(static_cast<int8_t>(dataNode.as<int16_t>()));
												break;
											}
											case FieldType::Int16:
											{
												fieldStorage->SetValue(dataNode.as<int16_t>());
												break;
											}
											case FieldType::Int32:
											{
												fieldStorage->SetValue(dataNode.as<int32_t>());
												break;
											}
											case FieldType::Int64:
											{
												fieldStorage->SetValue(dataNode.as<int64_t>());
												break;
											}
											case FieldType::UInt8:
											{
												fieldStorage->SetValue(dataNode.as<uint8_t>());
												break;
											}
											case FieldType::UInt16:
											{
												fieldStorage->SetValue(dataNode.as<uint16_t>());
												break;
											}
											case FieldType::UInt32:
											{
												fieldStorage->SetValue(dataNode.as<uint32_t>());
												break;
											}
											case FieldType::UInt64:
											{
												fieldStorage->SetValue(dataNode.as<uint64_t>());
												break;
											}
											case FieldType::Float:
											{
												fieldStorage->SetValue(dataNode.as<float>());
												break;
											}
											case FieldType::Double:
											{
												fieldStorage->SetValue(dataNode.as<double>());
												break;
											}
											case FieldType::String:
											{
												fieldStorage->SetValue(dataNode.as<std::string>());
												break;
											}
											case FieldType::Vector2:
											{
												fieldStorage->SetValue(dataNode.as<glm::vec2>());
												break;
											}
											case FieldType::Vector3:
											{
												fieldStorage->SetValue(dataNode.as<glm::vec3>());
												break;
											}
											case FieldType::Vector4:
											{
												fieldStorage->SetValue(dataNode.as<glm::vec4>());
												break;
											}
											case FieldType::Prefab:
											case FieldType::Entity:
											case FieldType::Mesh:
											case FieldType::StaticMesh:
											case FieldType::Material:
											case FieldType::PhysicsMaterial:
											case FieldType::Texture2D:
											{
												fieldStorage->SetValue(dataNode.as<UUID>());
												break;
											}
											}
										}
									}
								}
							}
						}
					}
					else
					{
						AS_CORE_ERROR("Failed to deserialize ScriptComponent for entity '{0}'! Couldn't find a valid ModuleName or ClassHandle field/value!", deserializedEntity.Name());
					}
				}

				//TODO: MeshComponent
				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					std::string str = meshComponent["Path"].as<std::string>();
					auto& mc = deserializedEntity.AddComponent<MeshComponent>(str);

					// Material
					mc.m_Mesh->m_Materials[0]->bUseAlbedoMap = meshComponent["bUseAlbedoMap"].as<bool>();
					mc.m_Mesh->m_Materials[0]->col = meshComponent["col"].as<glm::vec4>();
					mc.m_Mesh->m_Materials[0]->bUseNormalMap = meshComponent["bUseNormalMap"].as<bool>();
					mc.m_Mesh->m_Materials[0]->bUseMetallicMap = meshComponent["bUseMetallicMap"].as<bool>();
					mc.m_Mesh->m_Materials[0]->metallic = meshComponent["metallic"].as<float>();
					mc.m_Mesh->m_Materials[0]->bUseRoughnessMap = meshComponent["bUseRoughnessMap"].as<bool>();
					mc.m_Mesh->m_Materials[0]->roughness = meshComponent["roughness"].as<float>();
					mc.m_Mesh->m_Materials[0]->bUseAoMap = meshComponent["bUseAoMap"].as<bool>();
					// End Material
				}

				// TODO: Fix the rigidbodycomponent and rigidbody3Dcomponent
				auto rigidBodyComponent = entity["RigidBodyComponent"];
				if (rigidBodyComponent)
				{
					auto& component = deserializedEntity.AddComponent<RigidBodyComponent>();
					component.BodyType = (RigidBodyComponent::Type)rigidBodyComponent["BodyType"].as<int>(0);
					component.Mass = rigidBodyComponent["Mass"].as<float>(1.0f);
					component.LinearDrag = rigidBodyComponent["LinearDrag"].as<float>(0.0f);
					component.AngularDrag = rigidBodyComponent["AngularDrag"].as<float>(0.05f);
					component.DisableGravity = rigidBodyComponent["DisableGravity"].as<bool>(false);
					component.IsKinematic = rigidBodyComponent["IsKinematic"].as<bool>(false);
					component.LayerID = rigidBodyComponent["LayerID"].as<uint32_t>(0);

					component.CollisionDetection = (CollisionDetectionType)rigidBodyComponent["CollisionDetection"].as<uint32_t>(0);

					auto lockFlagsNode = rigidBodyComponent["LockFlags"];
					if (lockFlagsNode)
					{
						component.LockFlags = lockFlagsNode.as<uint8_t>(0);
					}
					else
					{
						component.LockFlags |= rigidBodyComponent["Constraints"]["LockPositionX"].as<bool>(false) ? (uint8_t)ActorLockFlag::TranslationX : 0;
						component.LockFlags |= rigidBodyComponent["Constraints"]["LockPositionY"].as<bool>(false) ? (uint8_t)ActorLockFlag::TranslationY : 0;
						component.LockFlags |= rigidBodyComponent["Constraints"]["LockPositionZ"].as<bool>(false) ? (uint8_t)ActorLockFlag::TranslationZ : 0;
						component.LockFlags |= rigidBodyComponent["Constraints"]["LockRotationX"].as<bool>(false) ? (uint8_t)ActorLockFlag::RotationX : 0;
						component.LockFlags |= rigidBodyComponent["Constraints"]["LockRotationY"].as<bool>(false) ? (uint8_t)ActorLockFlag::RotationY : 0;
						component.LockFlags |= rigidBodyComponent["Constraints"]["LockRotationZ"].as<bool>(false) ? (uint8_t)ActorLockFlag::RotationZ : 0;
					}
				}


				auto rigidbody3DComponent = entity["Rigidbody3DComponent"];
				if (rigidbody3DComponent)
				{
					auto& rb3d = deserializedEntity.AddComponent<Rigidbody3DComponent>();
					rb3d.mass = rigidbody3DComponent["mass"].as<float>();
					rb3d.Type = (Rigidbody3DComponent::Body3DType)rigidbody3DComponent["type"].as<uint32_t>();
					rb3d.Shape = (CollisionShape)rigidbody3DComponent["shape"].as<uint32_t>();
					rb3d.linearDamping = rigidbody3DComponent["linearDamping"].as<float>();
					rb3d.angularDamping = rigidbody3DComponent["angularDamping"].as<float>();
					rb3d.restitution = rigidbody3DComponent["restitution"].as<float>();
					rb3d.friction = rigidbody3DComponent["friction"].as<float>();
				}

				auto pointLightComponent = entity["PointLightComponent"];
				if (pointLightComponent)
				{
					float intensity = pointLightComponent["pointIntensity"].as<float>();
					glm::vec3 color = pointLightComponent["Color"].as<glm::vec3>();
					auto& src = deserializedEntity.AddComponent<PointLightComponent>(intensity, color);
				}

				auto directionalLightComponent = entity["DirectionalLightComponent"];
				if (directionalLightComponent)
				{
					float intensity = directionalLightComponent["dirIntensity"].as<float>();
					auto& src = deserializedEntity.AddComponent<DirectionalLightComponent>(intensity);
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if (spriteRendererComponent["TexturePath"])
					{
						std::string texturePath = spriteRendererComponent["TexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Texture = Texture2D::Create(path.string());
					}

					if (spriteRendererComponent["TilingFactor"])
						src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
					crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
					crc.Thickness = circleRendererComponent["Thickness"].as<float>();
					crc.Fade = circleRendererComponent["Fade"].as<float>();
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto textComponent = entity["TextComponent"];
				if (textComponent)
				{
					auto& tc = deserializedEntity.AddComponent<TextComponent>();
					tc.TextString = textComponent["TextString"].as<std::string>();
					// tc.FontAsset // TODO
					tc.Color = textComponent["Color"].as<glm::vec4>();
					tc.Kerning = textComponent["Kerning"].as<float>();
					tc.LineSpacing = textComponent["LineSpacing"].as<float>();
				}
			}
		}



		// Sort IdComponent by by entity handle (which is essentially the order in which they were created)
		// This ensures a consistent ordering when iterating IdComponent (for example: when rendering scene hierarchy panel)
		m_Scene->m_Registry.sort<IDComponent>([this](const auto lhs, const auto rhs)
			{
				auto lhsEntity = m_Scene->m_EntityIDMap.find(lhs.ID);
				auto rhsEntity = m_Scene->m_EntityIDMap.find(rhs.ID);
				return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
			});
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		AS_CORE_ASSERT(false);
		return false;
	}

}
