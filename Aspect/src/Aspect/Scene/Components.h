#pragma once

#include "SceneCamera.h"
#include "Aspect/Core/UUID.h"
#include "Aspect/Renderer/Texture.h"
#include "Aspect/Renderer/Font.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


//#include "Aspect/Asset/Asset.h"
#include "Aspect/Renderer/Mesh.h"
#include "Aspect/Physics/3D/PhysicsTypes.h"
#include "Aspect/Math/Math.h"
#include "Aspect/Script/GCManager.h"

namespace Aspect {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		//TODO:private
		glm::vec3 RotationEuler = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };  // Euler angles ------- glm::quat
	private:
		// These are private so that you are forced to set them via
		// SetRotation() or SetRotationEuler()
		// This avoids situation where one of them gets set and the other is forgotten.
		//
		// Why do we need both a quat and Euler angle representation for rotation?
		// Because Euler suffers from gimbal lock -> rotations should be stored as quaternions.
		//
		// BUT: quaternions are confusing, and humans like to work with Euler angles.
		// We cannot store just the quaternions and translate to/from Euler because the conversion
		// Euler -> quat -> Euler is not invariant.
		//
		// It's also sometimes useful to be able to store rotations > 360 degrees which
		// quats do not support.
		//
		// Accordingly, we store Euler for "editor" stuff that humans work with, 
		// and quats for everything else.  The two are maintained in-sync via the SetRotation()
		// methods.
	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		void SetTransform(const glm::mat4& transform)
		{
			Math::DecomposeTransform(transform, Translation, Rotation, Scale);
			RotationEuler = glm::eulerAngles(glm::quat(Rotation));
		}

		glm::vec3 GetRotationEuler() const
		{
			return RotationEuler;
		}

		glm::vec3 GetTranslation() const 
		{ 
			return Translation; 
		}

		/*void SetRotationEuler(const glm::vec3& euler)
		{
			RotationEuler = euler;
			Rotation = glm::quat(RotationEuler);
		}*/

		glm::vec3 GetRotation() const
		{
			return Rotation;
		}

		/*void SetRotation(const glm::quat& quat)
		{
			Rotation = quat;
			RotationEuler = glm::eulerAngles(Rotation);
		}*/

		glm::mat4 GetRotationMatrix() const
		{
			return glm::toMat4(glm::quat(Rotation));
		}

		friend class SceneSerializer;
	};
	
	struct MeshComponent
	{
		std::string Path = "None";
		Ref<Mesh> m_Mesh;

		MeshComponent() = default;
		//MeshComponent() { m_Mesh = CreateRef<Mesh>(); };
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: Path(path), m_Mesh(CreateRef<Mesh>(path))
		{
		}
		//MeshComponent(const std::filesystem::path& path)
		//	: Path(path)
		//{
		//}
	};

	struct RelationshipComponent
	{
		UUID ParentHandle = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: ParentHandle(parent) {}
	};


	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		enum class Type { None = -1, Perspective, Orthographic };
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	/*
	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};*/

	struct ScriptComponent
	{
		//TODO:TEMP
		//std::string ClassName;
		AssetHandle ScriptClassHandle = 0;
		GCHandle ManagedInstance = nullptr;
		std::vector<uint32_t> FieldIDs;

		// NOTE(Peter): Get's set to true when OnCreate has been called for this entity
		bool IsRuntimeInitialized = false;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent& other) = default;
		ScriptComponent(AssetHandle scriptClassHandle)
			: ScriptClassHandle(scriptClassHandle) {}
	};

	// Forward declaration
	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	// Physics
	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	enum class CollisionShape
	{
		None = 0,
		Box = 1,
		Sphere,
		ConvexHull

	};
	//TODO:3D Physics
	struct Rigidbody3DComponent
	{
	public:
		enum class Body3DType { Static = 0, Dynamic, Kinematic };
	public:
		Body3DType Type = Body3DType::Static;
		CollisionShape Shape = CollisionShape::Box;

		float mass{ 1.0f };
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;
		float restitution = 1.0f;
		float friction = 1.0f;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody3DComponent() = default;
		Rigidbody3DComponent(const Rigidbody3DComponent&) = default;
	};

	
	struct RigidBodyComponent
	{
		enum class Type { None = -1, Static, Dynamic };

		Type BodyType = Type::Static;
		uint32_t LayerID = 0;

		float Mass = 1.0f;
		float LinearDrag = 0.01f;
		float AngularDrag = 0.05f;
		bool DisableGravity = false;
		bool IsKinematic = false;
		CollisionDetectionType CollisionDetection = CollisionDetectionType::Discrete;

		uint8_t LockFlags = 0;

		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent& other) = default;
	};
	
	struct BoxColliderComponent
	{
		glm::vec3 HalfSize = { 0.5f, 0.5f, 0.5f };
		glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };
		bool IsTrigger = false;
		AssetHandle Material = 0;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent& other) = default;
	};

	struct SphereColliderComponent
	{
		float Radius = 0.5f;
		glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };
		bool IsTrigger = false;
		AssetHandle Material = 0;

		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent& other) = default;
	};

	struct CapsuleColliderComponent
	{
		float Radius = 0.5f;
		float Height = 1.0f;
		glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };
		bool IsTrigger = false;
		AssetHandle Material = 0;

		CapsuleColliderComponent() = default;
		CapsuleColliderComponent(const CapsuleColliderComponent& other) = default;
	};

	/*struct MeshColliderComponent
	{
		AssetHandle ColliderAsset = 0;
		uint32_t SubmeshIndex = 0;
		bool IsTrigger = false;
		bool UseSharedShape = false;
		AssetHandle OverrideMaterial = 0;
		ECollisionComplexity CollisionComplexity = ECollisionComplexity::Default;

		MeshColliderComponent() = default;
		MeshColliderComponent(const MeshColliderComponent& other) = default;
		MeshColliderComponent(AssetHandle colliderAsset, uint32_t submeshIndex = 0)
			: ColliderAsset(colliderAsset), SubmeshIndex(submeshIndex)
		{
		}
	};*/


	struct TextComponent
	{
		std::string TextString;
		Ref<Font> FontAsset = Font::GetDefault();
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};

	enum class LightType
	{
		None = 0, Directional = 1, Point = 2, Spot = 3
	};

	/*struct DirectionalLightComponent
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		bool CastShadows = true;
		bool SoftShadows = true;
		float LightSize = 0.5f; // For PCSS
		float ShadowAmount = 1.0f;
	};*/

	// temp directional light compo
	struct DirectionalLightComponent
	{
		float Intensity = 1.0f;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		DirectionalLightComponent(float intensity)
			: Intensity(intensity) {}

	};

	/*struct PointLightComponent
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float LightSize = 0.5f; // For PCSS
		float MinRadius = 1.f;
		float Radius = 10.0f;
		bool CastsShadows = true;
		bool SoftShadows = true;
		float Falloff = 1.0f;
	};*/
	struct PointLightComponent
	{
		float Intensity = 100.0f;
		glm::vec3 LightColor = { 1.0f, 1.0f, 1.0f };
		
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
		PointLightComponent(float intensity, const glm::vec3& lightColor)
			: Intensity(intensity), LightColor(lightColor) {}
	};

	struct SpotLightComponent
	{
		glm::vec3 Radiance{ 1.0f };
		float Intensity = 1.0f;
		float Range = 10.0f;
		float Angle = 60.0f;
		float AngleAttenuation = 5.0f;
		bool CastsShadows = false;
		bool SoftShadows = false;
		float Falloff = 1.0f;
	};

	struct SkyLightComponent
	{
		AssetHandle SceneEnvironment;
		float Intensity = 1.0f;
		float Lod = 0.0f;

		bool DynamicSky = false;
		glm::vec3 TurbidityAzimuthInclination = { 2.0, 0.0, 0.0 };
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = 
		ComponentGroup<TransformComponent,SpriteRendererComponent,
			CircleRendererComponent, CameraComponent, ScriptComponent,
			NativeScriptComponent, Rigidbody2DComponent, BoxCollider2DComponent,
			CircleCollider2DComponent, TextComponent,MeshComponent,DirectionalLightComponent,
		    RigidBodyComponent, Rigidbody3DComponent,PointLightComponent, RelationshipComponent>;

}
