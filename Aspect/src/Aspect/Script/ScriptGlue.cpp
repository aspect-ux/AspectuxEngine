#include "aspch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"
#include "ScriptUtils.h"
#include "ScriptCache.h"

//#include "Aspect/Animation/AnimationController.h"

#include "Aspect/ImGui/ImGui.h"
//#include "Aspect/Core/Events/EditorEvents.h"

#include "Aspect/Asset/AssetManager.h"

//#include "Aspect/Audio/AudioEngine.h"
//#include "Aspect/Audio/AudioComponent.h"
//#include "Aspect/Audio/AudioPlayback.h"
//#include "Aspect/Audio/AudioEvents/AudioCommandRegistry.h"

#include "Aspect/Core/Application.h"
#include "Aspect/Core/Hash.h"
//#include "Aspect/Core/Math/Noise.h"
//
#include "Aspect/Physics/Physics2D.h"
//#include "Aspect/Renderer/SceneRenderer.h"
//#include "Aspect/Physics/3D/PhysicsScene.h"
//#include "Aspect/Physics/3D/PhysicsJoints.h"
//#include "Aspect/Physics/3D/PhysicsLayer.h"
//
//#include "Aspect/Renderer/MeshFactory.h"
//
//#include "Aspect/Scene/Prefab.h"

#include "Aspect/Utilities/TypeInfo.h"

#include <box2d/box2d.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/exception.h>
#include <mono/jit/jit.h>

#include <functional>

namespace Aspect {

#define AS_ADD_INTERNAL_CALL(icall) mono_add_internal_call("Aspect.InternalCalls::"#icall, (void*)InternalCalls::icall)

	std::unordered_map<MonoType*, std::function<void(Entity&)>> s_CreateComponentFuncs;
	std::unordered_map<MonoType*, std::function<bool(Entity&)>> s_HasComponentFuncs;
	std::unordered_map<MonoType*, std::function<void(Entity&)>> s_RemoveComponentFuncs;

	template<typename TComponent>
	static void RegisterManagedComponent()
	{
		// NOTE(Peter): Get the demangled type name of TComponent
		const TypeNameString& componentTypeName = TypeInfo<TComponent, true>().Name();
		std::string componentName = fmt::format("Aspect.{}", componentTypeName);

		MonoType* managedType = mono_reflection_type_from_name(componentName.data(), ScriptEngine::GetCoreAssemblyInfo()->AssemblyImage);

		if (managedType)
		{
			s_CreateComponentFuncs[managedType] = [](Entity& entity) { entity.AddComponent<TComponent>(); };
			s_HasComponentFuncs[managedType] = [](Entity& entity) { return entity.HasComponent<TComponent>(); };
			s_RemoveComponentFuncs[managedType] = [](Entity& entity) { entity.RemoveComponent<TComponent>(); };
		}
		else
		{
			AS_CORE_VERIFY(false, "No C# component class found for {}!", componentName);
		}
	}

	void ScriptGlue::RegisterGlue()
	{
		if (s_CreateComponentFuncs.size() > 0)
		{
			s_CreateComponentFuncs.clear();
			s_HasComponentFuncs.clear();
			s_RemoveComponentFuncs.clear();
		}

		RegisterComponentTypes();
		RegisterInternalCalls();
	}

	void ScriptGlue::RegisterComponentTypes()
	{
		RegisterManagedComponent<TransformComponent>();
		//RegisterManagedComponent<TagComponent>();
		//RegisterManagedComponent<MeshComponent>();
		//RegisterManagedComponent<StaticMeshComponent>();
		//RegisterManagedComponent<AnimationComponent>();
		//RegisterManagedComponent<ScriptComponent>();
		//RegisterManagedComponent<CameraComponent>();
		//RegisterManagedComponent<DirectionalLightComponent>();
		//RegisterManagedComponent<PointLightComponent>();
		//RegisterManagedComponent<SpotLightComponent>();
		//RegisterManagedComponent<SkyLightComponent>();
		//RegisterManagedComponent<SpriteRendererComponent>();
		//RegisterManagedComponent<RigidBody2DComponent>();
		//RegisterManagedComponent<BoxCollider2DComponent>();
		//RegisterManagedComponent<RigidBodyComponent>();
		//RegisterManagedComponent<BoxColliderComponent>();
		//RegisterManagedComponent<SphereColliderComponent>();
		//RegisterManagedComponent<CapsuleColliderComponent>();
		//RegisterManagedComponent<MeshColliderComponent>();
		//RegisterManagedComponent<CharacterControllerComponent>();
		//RegisterManagedComponent<FixedJointComponent>();
		//RegisterManagedComponent<TextComponent>();
		//RegisterManagedComponent<AudioListenerComponent>();
		//RegisterManagedComponent<AudioComponent>();
	}

	void ScriptGlue::RegisterInternalCalls()
	{
		/*AS_ADD_INTERNAL_CALL(AssetHandle_IsValid);

		AS_ADD_INTERNAL_CALL(Application_Quit);
		AS_ADD_INTERNAL_CALL(Application_GetWidth);
		AS_ADD_INTERNAL_CALL(Application_GetHeight);

		AS_ADD_INTERNAL_CALL(SceneManager_IsSceneValid);
		AS_ADD_INTERNAL_CALL(SceneManager_IsSceneIDValid);
		AS_ADD_INTERNAL_CALL(SceneManager_LoadScene);
		AS_ADD_INTERNAL_CALL(SceneManager_LoadSceneByID);
		AS_ADD_INTERNAL_CALL(SceneManager_GetCurrentSceneID);*/

		/*AS_ADD_INTERNAL_CALL(Scene_FindEntityByTag);
		AS_ADD_INTERNAL_CALL(Scene_IsEntityValid);
		AS_ADD_INTERNAL_CALL(Scene_CreateEntity);
		AS_ADD_INTERNAL_CALL(Scene_InstantiatePrefab);
		AS_ADD_INTERNAL_CALL(Scene_InstantiatePrefabWithTranslation);
		AS_ADD_INTERNAL_CALL(Scene_InstantiatePrefabWithTransform);
		AS_ADD_INTERNAL_CALL(Scene_InstantiateChildPrefabWithTranslation);
		AS_ADD_INTERNAL_CALL(Scene_InstantiateChildPrefabWithTransform);
		AS_ADD_INTERNAL_CALL(Scene_DestroyEntity);
		AS_ADD_INTERNAL_CALL(Scene_DestroyAllChildren);
		AS_ADD_INTERNAL_CALL(Scene_GetEntities);
		AS_ADD_INTERNAL_CALL(Scene_GetChildrenIDs);*/

		//AS_ADD_INTERNAL_CALL(Entity_GetParent);
		//AS_ADD_INTERNAL_CALL(Entity_SetParent);
		//AS_ADD_INTERNAL_CALL(Entity_GetChildren);
		//AS_ADD_INTERNAL_CALL(Entity_CreateComponent);
		AS_ADD_INTERNAL_CALL(Entity_HasComponent);
		//AS_ADD_INTERNAL_CALL(Entity_RemoveComponent);

		//AS_ADD_INTERNAL_CALL(TagComponent_GetTag);
		//AS_ADD_INTERNAL_CALL(TagComponent_SetTag);

		AS_ADD_INTERNAL_CALL(TransformComponent_GetTransform);
		AS_ADD_INTERNAL_CALL(TransformComponent_SetTransform);
		AS_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		AS_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		/*AS_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		AS_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		AS_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		AS_ADD_INTERNAL_CALL(TransformComponent_SetScale);
		AS_ADD_INTERNAL_CALL(TransformComponent_GetWorldSpaceTransform);
		AS_ADD_INTERNAL_CALL(TransformComponent_GetTransformMatrix);
		AS_ADD_INTERNAL_CALL(TransformComponent_SetTransformMatrix);
		AS_ADD_INTERNAL_CALL(TransformComponent_SetRotationQuat);
		AS_ADD_INTERNAL_CALL(TransformMultiply_Native);
		
		AS_ADD_INTERNAL_CALL(MeshComponent_GetMesh);
		AS_ADD_INTERNAL_CALL(MeshComponent_SetMesh);
		AS_ADD_INTERNAL_CALL(MeshComponent_HasMaterial);
		AS_ADD_INTERNAL_CALL(MeshComponent_GetMaterial);
		AS_ADD_INTERNAL_CALL(MeshComponent_GetIsRigged);
		
		AS_ADD_INTERNAL_CALL(StaticMeshComponent_GetMesh);
		AS_ADD_INTERNAL_CALL(StaticMeshComponent_SetMesh);
		AS_ADD_INTERNAL_CALL(StaticMeshComponent_HasMaterial);
		AS_ADD_INTERNAL_CALL(StaticMeshComponent_GetMaterial);
		AS_ADD_INTERNAL_CALL(StaticMeshComponent_IsVisible);
		AS_ADD_INTERNAL_CALL(StaticMeshComponent_SetVisible);

		AS_ADD_INTERNAL_CALL(AnimationComponent_GetIsAnimationPlaying);
		AS_ADD_INTERNAL_CALL(AnimationComponent_SetIsAnimationPlaying);
		AS_ADD_INTERNAL_CALL(AnimationComponent_GetPlaybackSpeed);
		AS_ADD_INTERNAL_CALL(AnimationComponent_SetPlaybackSpeed);
		AS_ADD_INTERNAL_CALL(AnimationComponent_GetAnimationTime);
		AS_ADD_INTERNAL_CALL(AnimationComponent_SetAnimationTime);
		AS_ADD_INTERNAL_CALL(AnimationComponent_GetStateIndex);
		AS_ADD_INTERNAL_CALL(AnimationComponent_SetStateIndex);
		AS_ADD_INTERNAL_CALL(AnimationComponent_GetEnableRootMotion);
		AS_ADD_INTERNAL_CALL(AnimationComponent_SetEnableRootMotion);
		AS_ADD_INTERNAL_CALL(AnimationComponent_GetRootMotion);*/
		/*
		AS_ADD_INTERNAL_CALL(ScriptComponent_GetInstance);
		
		AS_ADD_INTERNAL_CALL(CameraComponent_SetPerspective);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetOrthographic);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetVerticalFOV);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetVerticalFOV);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetPerspectiveNearClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetPerspectiveNearClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetPerspectiveFarClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetPerspectiveFarClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetOrthographicSize);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetOrthographicSize);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetOrthographicNearClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetOrthographicNearClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetOrthographicFarClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetOrthographicFarClip);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetProjectionType);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetProjectionType);
		AS_ADD_INTERNAL_CALL(CameraComponent_GetPrimary);
		AS_ADD_INTERNAL_CALL(CameraComponent_SetPrimary);*/
		/*
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_GetRadiance);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_SetRadiance);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_GetIntensity);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_SetIntensity);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_GetCastShadows);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_SetCastShadows);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_GetSoftShadows);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_SetSoftShadows);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_GetLightSize);
		AS_ADD_INTERNAL_CALL(DirectionalLightComponent_SetLightSize);

		AS_ADD_INTERNAL_CALL(PointLightComponent_GetRadiance);
		AS_ADD_INTERNAL_CALL(PointLightComponent_SetRadiance);
		AS_ADD_INTERNAL_CALL(PointLightComponent_GetIntensity);
		AS_ADD_INTERNAL_CALL(PointLightComponent_SetIntensity);
		AS_ADD_INTERNAL_CALL(PointLightComponent_GetRadius);
		AS_ADD_INTERNAL_CALL(PointLightComponent_SetRadius);
		AS_ADD_INTERNAL_CALL(PointLightComponent_GetFalloff);
		AS_ADD_INTERNAL_CALL(PointLightComponent_SetFalloff);

		AS_ADD_INTERNAL_CALL(SkyLightComponent_GetIntensity);
		AS_ADD_INTERNAL_CALL(SkyLightComponent_SetIntensity);
		AS_ADD_INTERNAL_CALL(SkyLightComponent_GetTurbidity);
		AS_ADD_INTERNAL_CALL(SkyLightComponent_SetTurbidity);
		AS_ADD_INTERNAL_CALL(SkyLightComponent_GetAzimuth);
		AS_ADD_INTERNAL_CALL(SkyLightComponent_SetAzimuth);
		AS_ADD_INTERNAL_CALL(SkyLightComponent_GetInclination);
		AS_ADD_INTERNAL_CALL(SkyLightComponent_SetInclination);
		
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetRadiance);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetRadiance);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetIntensity);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetIntensity);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetRange);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetRange);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetAngle);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetAngle);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetAngleAttenuation);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetAngleAttenuation);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetFalloff);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetFalloff);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetCastsShadows);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetCastsShadows);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_GetSoftShadows);
		AS_ADD_INTERNAL_CALL(SpotLightComponent_SetSoftShadows);

		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTilingFactor);
		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTilingFactor);
		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_GetUVStart);
		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_SetUVStart);
		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_GetUVEnd);
		AS_ADD_INTERNAL_CALL(SpriteRendererComponent_SetUVEnd);/
		/*
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetBodyType);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetBodyType);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetTranslation);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetTranslation);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetRotation);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetRotation);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetMass);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetMass);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetLinearVelocity);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetLinearVelocity);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetGravityScale);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetGravityScale);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyAngularImpulse);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_AddForce);
		AS_ADD_INTERNAL_CALL(RigidBody2DComponent_AddTorque);
		
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetTranslation);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetTranslation);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetRotation);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetRotation);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_AddForce);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_AddForceAtLocation);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_AddTorque);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetLinearVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetLinearVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetAngularVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetAngularVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetMaxLinearVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetMaxLinearVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetMaxAngularVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetMaxAngularVelocity);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetLinearDrag);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetLinearDrag);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetAngularDrag);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetAngularDrag);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_Rotate);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetLayer);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetLayer);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetLayerName);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetLayerByName);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetMass);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetMass);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetBodyType);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetBodyType);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_IsKinematic);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetIsKinematic);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetKinematicTarget);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetKinematicTarget);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetLockFlag);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_IsLockFlagSet);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_GetLockFlags);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_IsSleeping);
		AS_ADD_INTERNAL_CALL(RigidBodyComponent_SetIsSleeping);

		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_GetSlopeLimit);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_SetSlopeLimit);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_GetStepOffset);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_SetStepOffset);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_Move);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_Jump);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_GetSpeedDown);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_IsGrounded);
		AS_ADD_INTERNAL_CALL(CharacterControllerComponent_GetCollisionFlags);

		AS_ADD_INTERNAL_CALL(FixedJointComponent_GetConnectedEntity);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_SetConnectedEntity);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_IsBreakable);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_SetIsBreakable);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_IsBroken);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_Break);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_GetBreakForce);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_SetBreakForce);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_GetBreakTorque);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_SetBreakTorque);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_IsCollisionEnabled);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_SetCollisionEnabled);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_IsPreProcessingEnabled);
		AS_ADD_INTERNAL_CALL(FixedJointComponent_SetPreProcessingEnabled);

		AS_ADD_INTERNAL_CALL(BoxColliderComponent_GetHalfSize);
		AS_ADD_INTERNAL_CALL(BoxColliderComponent_SetHalfSize);
		AS_ADD_INTERNAL_CALL(BoxColliderComponent_GetOffset);
		AS_ADD_INTERNAL_CALL(BoxColliderComponent_SetOffset);
		AS_ADD_INTERNAL_CALL(BoxColliderComponent_IsTrigger);
		AS_ADD_INTERNAL_CALL(BoxColliderComponent_SetTrigger);
		AS_ADD_INTERNAL_CALL(BoxColliderComponent_GetMaterialHandle);

		AS_ADD_INTERNAL_CALL(SphereColliderComponent_GetRadius);
		AS_ADD_INTERNAL_CALL(SphereColliderComponent_SetRadius);
		AS_ADD_INTERNAL_CALL(SphereColliderComponent_GetOffset);
		AS_ADD_INTERNAL_CALL(SphereColliderComponent_SetOffset);
		AS_ADD_INTERNAL_CALL(SphereColliderComponent_IsTrigger);
		AS_ADD_INTERNAL_CALL(SphereColliderComponent_SetTrigger);
		AS_ADD_INTERNAL_CALL(SphereColliderComponent_GetMaterialHandle);

		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_GetRadius);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_SetRadius);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_GetHeight);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_SetHeight);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_GetOffset);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_SetOffset);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_IsTrigger);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_SetTrigger);
		AS_ADD_INTERNAL_CALL(CapsuleColliderComponent_GetMaterialHandle);

		AS_ADD_INTERNAL_CALL(MeshColliderComponent_IsMeshStatic);
		AS_ADD_INTERNAL_CALL(MeshColliderComponent_IsColliderMeshValid);
		AS_ADD_INTERNAL_CALL(MeshColliderComponent_GetColliderMesh);
		AS_ADD_INTERNAL_CALL(MeshColliderComponent_SetColliderMesh);
		AS_ADD_INTERNAL_CALL(MeshColliderComponent_IsTrigger);
		AS_ADD_INTERNAL_CALL(MeshColliderComponent_SetTrigger);
		AS_ADD_INTERNAL_CALL(MeshColliderComponent_GetMaterialHandle);

		AS_ADD_INTERNAL_CALL(MeshCollider_IsStaticMesh);

		AS_ADD_INTERNAL_CALL(PhysicsMaterial_GetStaticFriction);
		AS_ADD_INTERNAL_CALL(PhysicsMaterial_SetStaticFriction);
		AS_ADD_INTERNAL_CALL(PhysicsMaterial_GetDynamicFriction);
		AS_ADD_INTERNAL_CALL(PhysicsMaterial_SetDynamicFriction);
		AS_ADD_INTERNAL_CALL(PhysicsMaterial_GetRestitution);
		AS_ADD_INTERNAL_CALL(PhysicsMaterial_SetRestitution);

		AS_ADD_INTERNAL_CALL(AudioComponent_IsPlaying);
		AS_ADD_INTERNAL_CALL(AudioComponent_Play);
		AS_ADD_INTERNAL_CALL(AudioComponent_Stop);
		AS_ADD_INTERNAL_CALL(AudioComponent_Pause);
		AS_ADD_INTERNAL_CALL(AudioComponent_Resume);
		AS_ADD_INTERNAL_CALL(AudioComponent_GetVolumeMult);
		AS_ADD_INTERNAL_CALL(AudioComponent_SetVolumeMult);
		AS_ADD_INTERNAL_CALL(AudioComponent_GetPitchMult);
		AS_ADD_INTERNAL_CALL(AudioComponent_SetPitchMult);
		AS_ADD_INTERNAL_CALL(AudioComponent_SetEvent);*/

		/*AS_ADD_INTERNAL_CALL(TextComponent_GetHash);
		AS_ADD_INTERNAL_CALL(TextComponent_GetText);
		AS_ADD_INTERNAL_CALL(TextComponent_SetText);
		AS_ADD_INTERNAL_CALL(TextComponent_GetColor);
		AS_ADD_INTERNAL_CALL(TextComponent_SetColor);*/
		/*
		//============================================================================================
		/// Audio
		AS_ADD_INTERNAL_CALL(Audio_PostEvent);
		AS_ADD_INTERNAL_CALL(Audio_PostEventFromAC);
		AS_ADD_INTERNAL_CALL(Audio_PostEventAtLocation);
		AS_ADD_INTERNAL_CALL(Audio_StopEventID);
		AS_ADD_INTERNAL_CALL(Audio_PauseEventID);
		AS_ADD_INTERNAL_CALL(Audio_ResumeEventID);
		AS_ADD_INTERNAL_CALL(Audio_CreateAudioEntity);

		AS_ADD_INTERNAL_CALL(AudioCommandID_Constructor);
		//============================================================================================
		/// Audio Parameters Interface
		AS_ADD_INTERNAL_CALL(Audio_SetParameterFloat);
		AS_ADD_INTERNAL_CALL(Audio_SetParameterInt);
		AS_ADD_INTERNAL_CALL(Audio_SetParameterBool);
		AS_ADD_INTERNAL_CALL(Audio_SetParameterFloatForAC);
		AS_ADD_INTERNAL_CALL(Audio_SetParameterIntForAC);
		AS_ADD_INTERNAL_CALL(Audio_SetParameterBoolForAC);

		AS_ADD_INTERNAL_CALL(Audio_SetParameterFloatForEvent);
		AS_ADD_INTERNAL_CALL(Audio_SetParameterIntForEvent);
		AS_ADD_INTERNAL_CALL(Audio_SetParameterBoolForEvent);
		//============================================================================================
		AS_ADD_INTERNAL_CALL(Audio_PreloadEventSources);
		AS_ADD_INTERNAL_CALL(Audio_UnloadEventSources);

		AS_ADD_INTERNAL_CALL(Audio_SetLowPassFilterValue);
		AS_ADD_INTERNAL_CALL(Audio_SetHighPassFilterValue);
		AS_ADD_INTERNAL_CALL(Audio_SetLowPassFilterValue_Event);
		AS_ADD_INTERNAL_CALL(Audio_SetHighPassFilterValue_Event);

		AS_ADD_INTERNAL_CALL(Audio_SetLowPassFilterValue_AC);
		AS_ADD_INTERNAL_CALL(Audio_SetHighPassFilterValue_AC);

		//============================================================================================
		
		AS_ADD_INTERNAL_CALL(Texture2D_Create);
		AS_ADD_INTERNAL_CALL(Texture2D_GetSize);
		AS_ADD_INTERNAL_CALL(Texture2D_SetData);
		//AS_ADD_INTERNAL_CALL(Texture2D_GetData);

		AS_ADD_INTERNAL_CALL(Mesh_GetMaterialByIndex);
		AS_ADD_INTERNAL_CALL(Mesh_GetMaterialCount);

		AS_ADD_INTERNAL_CALL(StaticMesh_GetMaterialByIndex);
		AS_ADD_INTERNAL_CALL(StaticMesh_GetMaterialCount);

		AS_ADD_INTERNAL_CALL(Material_GetAlbedoColor);
		AS_ADD_INTERNAL_CALL(Material_SetAlbedoColor);
		AS_ADD_INTERNAL_CALL(Material_GetMetalness);
		AS_ADD_INTERNAL_CALL(Material_SetMetalness);
		AS_ADD_INTERNAL_CALL(Material_GetRoughness);
		AS_ADD_INTERNAL_CALL(Material_SetRoughness);
		AS_ADD_INTERNAL_CALL(Material_GetEmission);
		AS_ADD_INTERNAL_CALL(Material_SetEmission);
		AS_ADD_INTERNAL_CALL(Material_SetFloat);
		AS_ADD_INTERNAL_CALL(Material_SetVector3);
		AS_ADD_INTERNAL_CALL(Material_SetVector4);
		AS_ADD_INTERNAL_CALL(Material_SetTexture);

		AS_ADD_INTERNAL_CALL(MeshFactory_CreatePlane);*/
		/*
		AS_ADD_INTERNAL_CALL(Physics_Raycast);
		AS_ADD_INTERNAL_CALL(Physics_SphereCast);
		AS_ADD_INTERNAL_CALL(Physics_Raycast2D);
		AS_ADD_INTERNAL_CALL(Physics_OverlapBox);
		AS_ADD_INTERNAL_CALL(Physics_OverlapCapsule);
		AS_ADD_INTERNAL_CALL(Physics_OverlapSphere);
		AS_ADD_INTERNAL_CALL(Physics_OverlapBoxNonAlloc);
		AS_ADD_INTERNAL_CALL(Physics_OverlapCapsuleNonAlloc);
		AS_ADD_INTERNAL_CALL(Physics_OverlapSphereNonAlloc);
		AS_ADD_INTERNAL_CALL(Physics_GetGravity);
		AS_ADD_INTERNAL_CALL(Physics_SetGravity);
		AS_ADD_INTERNAL_CALL(Physics_AddRadialImpulse);

		AS_ADD_INTERNAL_CALL(Matrix4_LookAt);

		AS_ADD_INTERNAL_CALL(Noise_Constructor);
		AS_ADD_INTERNAL_CALL(Noise_Destructor);
		AS_ADD_INTERNAL_CALL(Noise_GetFrequency);
		AS_ADD_INTERNAL_CALL(Noise_SetFrequency);
		AS_ADD_INTERNAL_CALL(Noise_GetFractalOctaves);
		AS_ADD_INTERNAL_CALL(Noise_SetFractalOctaves);
		AS_ADD_INTERNAL_CALL(Noise_GetFractalLacunarity);
		AS_ADD_INTERNAL_CALL(Noise_SetFractalLacunarity);
		AS_ADD_INTERNAL_CALL(Noise_GetFractalGain);
		AS_ADD_INTERNAL_CALL(Noise_SetFractalGain);
		AS_ADD_INTERNAL_CALL(Noise_Get);

		AS_ADD_INTERNAL_CALL(Noise_SetSeed);
		AS_ADD_INTERNAL_CALL(Noise_Perlin);*/

		AS_ADD_INTERNAL_CALL(Log_LogMessage);
		/*
		AS_ADD_INTERNAL_CALL(Input_IsKeyPressed);
		AS_ADD_INTERNAL_CALL(Input_IsKeyHeld);
		AS_ADD_INTERNAL_CALL(Input_IsKeyDown);
		AS_ADD_INTERNAL_CALL(Input_IsKeyReleased);
		AS_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
		AS_ADD_INTERNAL_CALL(Input_IsMouseButtonHeld);
		AS_ADD_INTERNAL_CALL(Input_IsMouseButtonDown);
		AS_ADD_INTERNAL_CALL(Input_IsMouseButtonReleased);
		AS_ADD_INTERNAL_CALL(Input_GetMousePosition);
		AS_ADD_INTERNAL_CALL(Input_SetCursorMode);
		AS_ADD_INTERNAL_CALL(Input_GetCursorMode);
		AS_ADD_INTERNAL_CALL(Input_IsControllerPresent);
		AS_ADD_INTERNAL_CALL(Input_GetConnectedControllerIDs);
		AS_ADD_INTERNAL_CALL(Input_GetControllerName);
		AS_ADD_INTERNAL_CALL(Input_IsControllerButtonPressed);
		AS_ADD_INTERNAL_CALL(Input_IsControllerButtonHeld);
		AS_ADD_INTERNAL_CALL(Input_IsControllerButtonDown);
		AS_ADD_INTERNAL_CALL(Input_IsControllerButtonReleased);
		AS_ADD_INTERNAL_CALL(Input_GetControllerAxis);
		AS_ADD_INTERNAL_CALL(Input_GetControllerHat);
		AS_ADD_INTERNAL_CALL(Input_GetControllerDeadzone);
		AS_ADD_INTERNAL_CALL(Input_SetControllerDeadzone);
		
		AS_ADD_INTERNAL_CALL(SceneRenderer_GetOpacity);
		AS_ADD_INTERNAL_CALL(SceneRenderer_SetOpacity);

		AS_ADD_INTERNAL_CALL(SceneRenderer_DepthOfField_IsEnabled);
		AS_ADD_INTERNAL_CALL(SceneRenderer_DepthOfField_SetEnabled);
		AS_ADD_INTERNAL_CALL(SceneRenderer_DepthOfField_GetFocusDistance);
		AS_ADD_INTERNAL_CALL(SceneRenderer_DepthOfField_SetFocusDistance);
		AS_ADD_INTERNAL_CALL(SceneRenderer_DepthOfField_GetBlurSize);
		AS_ADD_INTERNAL_CALL(SceneRenderer_DepthOfField_SetBlurSize);

		AS_ADD_INTERNAL_CALL(DebugRenderer_DrawLine);
		AS_ADD_INTERNAL_CALL(DebugRenderer_DrawQuadBillboard);
		AS_ADD_INTERNAL_CALL(DebugRenderer_SetLineWidth);

#ifndef AS_DIST
		// Editor Only
		AS_ADD_INTERNAL_CALL(EditorUI_Text);
		AS_ADD_INTERNAL_CALL(EditorUI_Button);
		AS_ADD_INTERNAL_CALL(EditorUI_BeginPropertyHeader);
		AS_ADD_INTERNAL_CALL(EditorUI_EndPropertyHeader);
		AS_ADD_INTERNAL_CALL(EditorUI_PropertyGrid);
		AS_ADD_INTERNAL_CALL(EditorUI_PropertyFloat);
		AS_ADD_INTERNAL_CALL(EditorUI_PropertyVec2);
		AS_ADD_INTERNAL_CALL(EditorUI_PropertyVec3);
		AS_ADD_INTERNAL_CALL(EditorUI_PropertyVec4);
#endif*/
	}

	namespace InternalCalls {

		static inline Entity GetEntity(uint64_t entityID)
		{
			AspectRef<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			return scene->TryGetEntityWithUUID(entityID);
		};
/*
		static inline Ref<PhysicsActor> GetPhysicsActor(Entity entity)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No scene active!");
			Ref<PhysicsScene> physicsScene = scene->GetPhysicsScene();
			AS_CORE_ASSERT(physicsScene, "No physics scene active!");
			return physicsScene->GetActor(entity);
		}

#pragma region AssetHandle

		bool AssetHandle_IsValid(AssetHandle* assetHandle)
		{
			return AssetManager::IsAssetHandleValid(*assetHandle);
		}

#pragma endregion

#pragma region Application

		void Application_Quit()
		{
#ifdef AS_DIST
			Application::Get().DispatchEvent<WindowCloseEvent>();
#else
			Application::Get().DispatchEvent<EditorExitPlayModeEvent>();
#endif
		}

		uint32_t Application_GetWidth() { return ScriptEngine::GetSceneContext()->GetViewportWidth(); }
		uint32_t Application_GetHeight() { return ScriptEngine::GetSceneContext()->GetViewportHeight(); }

#pragma endregion

#pragma region SceneManager

		bool SceneManager_IsSceneValid(MonoString* inScene)
		{
			std::string sceneFilePath = ScriptUtils::MonoStringToUTF8(inScene);
			return FileSystem::Exists(Project::GetAssetDirectory() / sceneFilePath);
		}

		bool SceneManager_IsSceneIDValid(uint64_t sceneID)
		{
			return AssetManager::GetAsset<Scene>(sceneID) != nullptr;
		}

		void SceneManager_LoadScene(MonoString* inScene)
		{
			Ref<Scene> activeScene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(activeScene, "No active scene!");
			activeScene->OnSceneTransition(ScriptUtils::MonoStringToUTF8(inScene));
		}

		void SceneManager_LoadSceneByID(uint64_t sceneID)
		{
			Ref<Scene> activeScene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(activeScene, "No active scene!");

			// TODO(Yan): OnSceneTransition should take scene by AssetHandle, NOT filepath (because this won't work in runtime)
			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(sceneID);

			if (metadata.Type != AssetType::Scene || !metadata.IsValid())
			{
				AS_CONSOLE_LOG_ERROR("SceneManager: Tried to load a scene with an invalid ID!");
				return;
			}

			activeScene->OnSceneTransition(std::filesystem::relative(metadata.FilePath, Project::GetAssetDirectory()).string());
		}

		uint64_t SceneManager_GetCurrentSceneID() { return ScriptEngine::GetSceneContext()->GetUUID(); }

#pragma endregion

#pragma region Scene

		uint64_t Scene_FindEntityByTag(MonoString* tag)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			Entity entity = scene->TryGetEntityWithTag(ScriptUtils::MonoStringToUTF8(tag));
			return entity ? entity.GetUUID() : 0;
		}

		bool Scene_IsEntityValid(uint64_t entityID)
		{
			if (entityID == 0)
				return false;

			return (bool)(ScriptEngine::GetSceneContext()->TryGetEntityWithUUID(entityID));
		}

		uint64_t Scene_CreateEntity(MonoString* tag)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			return scene->CreateEntity(ScriptUtils::MonoStringToUTF8(tag)).GetUUID();
		}

		uint64_t Scene_InstantiatePrefab(AssetHandle* prefabHandle)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();

			if (!AssetManager::IsAssetHandleValid(*prefabHandle))
				return 0;

			Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(*prefabHandle);
			return scene->Instantiate(prefab).GetUUID();
		}

		uint64_t Scene_InstantiatePrefabWithTranslation(AssetHandle* prefabHandle, glm::vec3* inTranslation)
		{
			return Scene_InstantiatePrefabWithTransform(prefabHandle, inTranslation, nullptr, nullptr);
		}

		uint64_t Scene_InstantiatePrefabWithTransform(AssetHandle* prefabHandle, glm::vec3* inTranslation, glm::vec3* inRotation, glm::vec3* inScale)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();

			if (!AssetManager::IsAssetHandleValid(*prefabHandle))
				return 0;

			Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(*prefabHandle);
			return scene->Instantiate(prefab, inTranslation, inRotation, inScale).GetUUID();
		}

		uint64_t Scene_InstantiateChildPrefabWithTranslation(uint64_t parentID, AssetHandle* prefabHandle, glm::vec3* inTranslation)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();

			Entity parent = scene->TryGetEntityWithUUID(parentID);
			if (!parent)
				return 0;

			if (!AssetManager::IsAssetHandleValid(*prefabHandle))
				return 0;

			Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(*prefabHandle);
			return scene->InstantiateChild(prefab, parent, inTranslation, nullptr, nullptr).GetUUID();
		}

		uint64_t Scene_InstantiateChildPrefabWithTransform(uint64_t parentID, AssetHandle* prefabHandle, glm::vec3* inTranslation, glm::vec3* inRotation, glm::vec3* inScale)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();

			Entity parent = scene->TryGetEntityWithUUID(parentID);
			if (!parent)
				return 0;

			if (!AssetManager::IsAssetHandleValid(*prefabHandle))
				return 0;

			Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(*prefabHandle);
			return scene->InstantiateChild(prefab, parent, inTranslation, inRotation, inScale).GetUUID();
		}

		void Scene_DestroyEntity(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);
			if (!entity)
				return;

			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			scene->SubmitToDestroyEntity(entity);
		}

		void Scene_DestroyAllChildren(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			Entity entity = GetEntity(entityID);
			AS_CORE_VERIFY(entity, "Entity is invalid!");
			if (!entity)
				return;
			const std::vector<UUID> children = entity.Children();
			for (UUID id : children)
				scene->DestroyEntity(id);
		}

		MonoArray* Scene_GetEntities()
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			auto entities = scene->GetAllEntitiesWith<IDComponent>();
			MonoArray* result = ManagedArrayUtils::Create<Entity>(entities.size());
			uint32_t i = 0;
			for (auto entity : entities)
				ManagedArrayUtils::SetValue(result, i++, entities.get<IDComponent>(entity).ID);

			return result;
		}

		MonoArray* Scene_GetChildrenIDs(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			Entity entity = GetEntity(entityID);
			AS_CORE_VERIFY(entity, "Entity is invalid!");
			if (!entity)
				return nullptr;
			const auto& children = entity.Children();
			MonoArray* result = ManagedArrayUtils::Create<uint64_t>(children.size());
			uint32_t i = 0;
			for (size_t i = 0; i < children.size(); i++)
				ManagedArrayUtils::SetValue(result, i, children[i]);
			return result;
		}

#pragma endregion
*/
#pragma region Entity

		uint64_t Entity_GetParent(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
				return 0;
			return entity.GetParentUUID();
		}

		void Entity_SetParent(uint64_t entityID, uint64_t parentID)
		{
			Entity child = GetEntity(entityID);
			
			if (!child)
			{
				AS_CONSOLE_LOG_ERROR("Entity.SetParent - Invalid entity!");
				return;
			}

			if (parentID == 0)
			{
				ScriptEngine::GetSceneContext()->UnparentEntity(child);
			}
			else
			{
				Entity parent = GetEntity(parentID);

				if (!parent)
				{
					AS_CONSOLE_LOG_ERROR("Entity.SetParent - Invalid parent entity!");
					return;
				}

				child.SetParent(parent);
			}
		}

		MonoArray* Entity_GetChildren(uint64_t entityID)
		{
			AS_PROFILE_FUNC();

			Entity entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("Entity.GetChildren - Invalid entity!");
				return ManagedArrayUtils::Create<Entity>(0);
			}

			const auto& children = entity.Children();
			MonoArray* result = ManagedArrayUtils::Create<Entity>(children.size());
			for (uint32_t i = 0; i < children.size(); i++)
				ManagedArrayUtils::SetValue(result, i, children[i]);
			return result;
		}

		void Entity_CreateComponent(uint64_t entityID, MonoReflectionType* reflectionType)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("Entity.CreateComponent - Invalid entity!");
				return;
			}

			if (reflectionType == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("Entity.CreateComponent - Cannot create a null component!");
				return;
			}

			MonoType* managedType = mono_reflection_type_get_type(reflectionType);
			if (s_CreateComponentFuncs.find(managedType) == s_CreateComponentFuncs.end())
			{
				char* typeName = mono_type_get_name(managedType);
				AS_CONSOLE_LOG_ERROR("Entity.CreateComponent - Unknown component type {0}!", typeName);
				mono_free(typeName);
				return;
			}

			s_CreateComponentFuncs.at(managedType)(entity);
		}

		bool Entity_HasComponent(uint64_t entityID, MonoReflectionType* reflectionType)
		{
			AS_PROFILE_FUNC();

			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("Entity.HasComponent - Invalid entity!");
				return false;
			}

			if (reflectionType == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("Entity.HasComponent - Entities can never have a null component!");
				return false;
			}

			MonoType* managedType = mono_reflection_type_get_type(reflectionType);
			if (s_HasComponentFuncs.find(managedType) == s_HasComponentFuncs.end())
			{
				char* typeName = mono_type_get_name(managedType);
				AS_CONSOLE_LOG_ERROR("Entity.HasComponent - Unknown component type {0}!", typeName);
				mono_free(typeName);
				return false;
			}

			return s_HasComponentFuncs.at(managedType)(entity);
		}

		bool Entity_RemoveComponent(uint64_t entityID, MonoReflectionType* componentType)
		{
			AS_PROFILE_FUNC();

			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("Entity.RemoveComponent called on an invalid Entity!");
				return false;
			}

			if (componentType == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("Tried to call Entity.RemoveComponent with a 'null' component!");
				return false;
			}

			MonoType* managedType = mono_reflection_type_get_type(componentType);
			char* typeName = mono_type_get_name(managedType);
			if (s_RemoveComponentFuncs.find(managedType) == s_RemoveComponentFuncs.end())
			{
				AS_CONSOLE_LOG_ERROR("Entity.RemoveComponent called with an unknown component type '{0}'!", typeName);
				return false;
			}

			if (!s_HasComponentFuncs.at(managedType)(entity))
			{
				AS_CONSOLE_LOG_WARN("Tried to remove component '{0}' from Entity '{1}' even though it doesn't have that component.", typeName, entity.Name());
				return false;
			}

			mono_free(typeName);
			s_RemoveComponentFuncs.at(managedType)(entity);
			return true;
		}

#pragma endregion
/*
#pragma region TagComponent

		MonoString* TagComponent_GetTag(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TagComponent.GetTag - Invalid entity!");
				return nullptr;
			}

			const auto& tagComponent = entity.GetComponent<TagComponent>();
			return ScriptUtils::UTF8StringToMono(tagComponent.Tag);
		}

		void TagComponent_SetTag(uint64_t entityID, MonoString* inTag)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TagComponent.SetTag - Invalid entity!");
				return;
			}

			auto& tagComponent = entity.GetComponent<TagComponent>();
			tagComponent.Tag = ScriptUtils::MonoStringToUTF8(inTag);
		}

#pragma endregion
*/
#pragma region TransformComponent

		void TransformComponent_GetTransform(uint64_t entityID, Transform* outTransform)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.GetTransform - Invalid entity!");
				*outTransform = Transform();
				return;
			}

			const auto& tc = entity.GetComponent<TransformComponent>();
			outTransform->Translation = tc.Translation;
			outTransform->Rotation = tc.GetRotationEuler();
			outTransform->Scale = tc.Scale;
		}

		void TransformComponent_SetTransform(uint64_t entityID, Transform* inTransform)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetTransform - Invalid entity!");
				return;
			}

			if (inTransform == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetTransform - Attempting to set null transform!");
				return;
			}
		
			auto tc = entity.GetComponent<TransformComponent>();
			tc.Translation = inTransform->Translation;
			//tc.SetRotationEuler(inTransform->Rotation);
			tc.Scale = inTransform->Scale;
		}

		void TransformComponent_GetTranslation(uint64_t entityID, glm::vec3* outTranslation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.GetTranslation - Invalid entity!");
				*outTranslation = glm::vec3(0.0f);
				return;
			}

			if (entity.HasComponent<RigidBodyComponent>())
			{
				/*auto actor = GetPhysicsActor(entity);

				if (!actor)
				{
					AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetTranslation - No actor found!");
					return;
				}

				*outTranslation = actor->GetTranslation();*/
			}
			else
			{
				*outTranslation = entity.GetComponent<TransformComponent>().Translation;
			}
		}

		void TransformComponent_SetTranslation(uint64_t entityID, glm::vec3* inTranslation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetTranslation - Invalid entity!");
				return;
			}

			if (inTranslation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetTranslation - Attempting to set null translation!");
				return;
			}

			if (entity.HasComponent<RigidBodyComponent>())
			{
				/*
				auto actor = GetPhysicsActor(entity);

				if (!actor)
				{
					AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetTranslation - No actor found!");
					return;
				}

				actor->SetTranslation(*inTranslation);*/
			}
			else
			{
				entity.GetComponent<TransformComponent>().Translation = *inTranslation;
			}
		}

		void TransformComponent_GetRotation(uint64_t entityID, glm::vec3* outRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.GetRotation - Invalid entity!");
				*outRotation = glm::vec3(0.0f);
				return;
			}
			/*
			if (entity.HasComponent<RigidBodyComponent>())
			{
				
				auto actor = GetPhysicsActor(entity);

				if (!actor)
				{
					AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetRotation - No actor found!");
					return;
				}

				if (!actor->IsAllRotationLocked())
				{
					*outRotation = actor->GetRotationEuler();
					return;
				}

			}
			*outRotation = entity.GetComponent<TransformComponent>().GetRotationEuler();*/
		}

		void TransformComponent_SetRotation(uint64_t entityID, glm::vec3* inRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetRotation - Invalid entity!");
				return;
			}

			if (inRotation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetRotation - Attempting to set null rotation!");
				return;
			}
			/*
			if (entity.HasComponent<RigidBodyComponent>())
			{
				
				auto actor = GetPhysicsActor(entity);

				if (!actor)
				{
					AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetRotation - No actor found!");
					return;
				}

				if (!actor->IsAllRotationLocked())
					actor->SetRotation(glm::quat(*inRotation));
			}
			entity.GetComponent<TransformComponent>().SetRotationEuler(*inRotation);*/
		}

		void TransformComponent_GetScale(uint64_t entityID, glm::vec3* outScale)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.GetScale - Invalid entity!");
				*outScale = glm::vec3(0.0f);
				return;
			}

			*outScale = entity.GetComponent<TransformComponent>().Scale;
		}

		void TransformComponent_SetScale(uint64_t entityID, glm::vec3* inScale)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetScale - Invalid entity!");
				return;
			}

			if (inScale == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetScale - Attempting to set null scale!");
				return;
			}

			entity.GetComponent<TransformComponent>().Scale = *inScale;
		}

		void TransformComponent_GetWorldSpaceTransform(uint64_t entityID, Transform* outTransform)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.GetWorldSpaceTransform - Invalid entity!");
				*outTransform = Transform();
				return;
			}
			/*
			AspectRef<Scene> scene = ScriptEngine::GetSceneContext();
			const auto& wt = scene->GetWorldSpaceTransform(entity);
			outTransform->Translation = wt.Translation;
			outTransform->Rotation = wt.GetRotationEuler();
			outTransform->Scale = wt.Scale;*/
		}

		void TransformComponent_GetTransformMatrix(uint64_t entityID, glm::mat4* outTransform)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.GetWorldSpaceTransform - Invalid entity!");
				*outTransform = glm::mat4(1.0f);
				return;
			}

			*outTransform = entity.Transform().GetTransform();
		}

		void TransformComponent_SetTransformMatrix(uint64_t entityID, glm::mat4* inTransform)

		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.GetWorldSpaceTransform - Invalid entity!");
				return;
			}

			entity.Transform().SetTransform(*inTransform);
		}

		void TransformComponent_SetRotationQuat(uint64_t entityID, glm::quat* inRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetRotation - Invalid entity!");
				return;
			}

			if (inRotation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("TransformComponent.SetRotation - Attempting to set null rotation!");
				return;
			}

			if (entity.HasComponent<RigidBodyComponent>())
			{
				/*
				auto actor = GetPhysicsActor(entity);

				if (!actor)
				{
					AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetRotation - No actor found!");
					return;
				}

				if (!actor->IsAllRotationLocked())
					actor->SetRotation(*inRotation);*/
			}
			//entity.GetComponent<TransformComponent>().SetRotation(*inRotation);
		}

		void TransformMultiply_Native(Transform* inA, Transform* inB, Transform* outResult)
		{
			TransformComponent a;
			a.Translation = inA->Translation;
			//a.SetRotationEuler(inA->Rotation);
			a.Scale = inA->Scale;

			TransformComponent b;
			b.Translation = inB->Translation;
			//b.SetRotationEuler(inB->Rotation);
			b.Scale = inB->Scale;

			glm::mat4 transform = a.GetTransform() * b.GetTransform();
			b.SetTransform(transform);
			outResult->Translation = b.Translation;
			outResult->Rotation = b.GetRotationEuler();
			outResult->Scale = b.Scale;
		}

#pragma endregion
		/*
#pragma region MeshComponent

		bool MeshComponent_GetMesh(uint64_t entityID, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.GetMesh - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			if (entity.HasComponent<MeshComponent>())
			{
				const auto& meshComponent = entity.GetComponent<MeshComponent>();
				auto mesh = AssetManager::GetAsset<Mesh>(meshComponent.Mesh);

				if (!mesh)
				{
					AS_CONSOLE_LOG_ERROR("MeshComponent.GetMesh - Component has an invalid mesh asset!");
					*outHandle = AssetHandle(0);
					return false;
				}

				*outHandle = meshComponent.Mesh;
				return true;

			}

			AS_CONSOLE_LOG_ERROR("MeshComponent.GetMesh - This message should never appear. If it does it means the engine is broken.");
			*outHandle = AssetHandle(0);
			return false;

		}

		void MeshComponent_SetMesh(uint64_t entityID, AssetHandle* meshHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.SetMesh - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<MeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.SetMesh - Entity doesn't have a MeshComponent!");
				return;
			}

			auto& meshComponent = entity.GetComponent<MeshComponent>();
			meshComponent.Mesh = *meshHandle;
		}

		bool MeshComponent_HasMaterial(uint64_t entityID, int index)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.HasMaterial - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<MeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.HasMaterial - Entity doesn't have a MeshComponent!");
				return false;
			}

			Ref<MaterialTable> materialTable = entity.GetComponent<MeshComponent>().MaterialTable;
			return materialTable && materialTable->HasMaterial(index);
		}

		bool MeshComponent_GetMaterial(uint64_t entityID, int index, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.GetMaterial - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			if (!entity.HasComponent<MeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.GetMaterial - Entity doesn't have a MeshComponent!");
				*outHandle = AssetHandle(0);
				return false;
			}

			Ref<MaterialTable> materialTable = entity.GetComponent<MeshComponent>().MaterialTable;

			if ((uint32_t)index >= materialTable->GetMaterialCount())
			{
				AS_CONSOLE_LOG_WARN("MeshComponent.GetMaterial - Material index out of range: {0}. Expected index less than {1}", index, materialTable->GetMaterialCount());
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = materialTable->GetMaterial(index);
			return true;
		}

		bool MeshComponent_GetIsRigged(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.GetIsRigged - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<MeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("MeshComponent.GetIsRigged - Entity doesn't have a MeshComponent!");
				return false;
			}

			auto& meshComponent = entity.GetComponent<MeshComponent>();
			auto mesh = AssetManager::GetAsset<Mesh>(meshComponent.Mesh);
			if (mesh)
			{
				auto meshSource = mesh->GetMeshSource();
				return meshSource ? meshSource->IsSubmeshRigged(meshComponent.SubmeshIndex) : false;
			}
			return false;
		}

#pragma endregion

#pragma region StaticMeshComponent

		bool StaticMeshComponent_GetMesh(uint64_t entityID, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.GetMesh - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			if (entity.HasComponent<StaticMeshComponent>())
			{
				const auto& meshComponent = entity.GetComponent<StaticMeshComponent>();
				auto mesh = AssetManager::GetAsset<StaticMesh>(meshComponent.StaticMesh);

				if (!mesh)
				{
					AS_CONSOLE_LOG_ERROR("StaticMeshComponent.GetMesh - Component has an invalid mesh asset!");
					*outHandle = AssetHandle(0);
					return false;
				}

				*outHandle = meshComponent.StaticMesh;
				return true;
			}

			AS_CONSOLE_LOG_ERROR("StaticMeshComponent.GetMesh - This message should never appear. If it does it means the engine is broken.");
			*outHandle = AssetHandle(0);
			return false;
		}

		void StaticMeshComponent_SetMesh(uint64_t entityID, AssetHandle* meshHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.SetMesh - Invalid entity!");
				return;
			}

			// TODO: Verify that meshHandle is valid

			if (!entity.HasComponent<StaticMeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.SetMesh - Entity doesn't have a MeshComponent!");
				return;
			}

			auto& meshComponent = entity.GetComponent<StaticMeshComponent>();
			meshComponent.StaticMesh = *meshHandle;
		}

		bool StaticMeshComponent_HasMaterial(uint64_t entityID, int index)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.HasMaterial - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<StaticMeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.HasMaterial - Entity doesn't have a MeshComponent!");
				return false;
			}

			Ref<MaterialTable> materialTable = entity.GetComponent<StaticMeshComponent>().MaterialTable;
			return materialTable && materialTable->HasMaterial(index);
		}

		bool StaticMeshComponent_GetMaterial(uint64_t entityID, int index, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.GetMaterial - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			if (!entity.HasComponent<StaticMeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.GetMaterial - Entity doesn't have a MeshComponent!");
				*outHandle = AssetHandle(0);
				return false;
			}

			Ref<MaterialTable> materialTable = entity.GetComponent<StaticMeshComponent>().MaterialTable;

			if ((uint32_t)index >= materialTable->GetMaterialCount())
			{
				AS_CONSOLE_LOG_WARN("StaticMeshComponent.GetMaterial - Material index out of range: {0}. Expected index less than {1}", index, materialTable->GetMaterialCount());
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = materialTable->GetMaterial(index);
			return true;
		}

		bool StaticMeshComponent_IsVisible(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.IsVisible - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<StaticMeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.IsVisible - Entity has no StaticMeshComponent!");
				return false;
			}

			const auto& meshComponent = entity.GetComponent<StaticMeshComponent>();
			return meshComponent.Visible;
		}

		void StaticMeshComponent_SetVisible(uint64_t entityID, bool visible)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.IsVisible - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<StaticMeshComponent>())
			{
				AS_CONSOLE_LOG_ERROR("StaticMeshComponent.IsVisible - Entity has no StaticMeshComponent!");
				return;
			}

			auto& meshComponent = entity.GetComponent<StaticMeshComponent>();
			meshComponent.Visible = visible;
		}

#pragma endregion

#pragma region AnimationComponent
		bool AnimationComponent_GetIsAnimationPlaying(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetIsAnimationPlaying - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetIsAnimationPlaying - Entity doesn't have an AnimationComponent!");
				return false;
			}

			return entity.GetComponent<AnimationComponent>().AnimationData->m_IsAnimationPlaying;
		}

		void AnimationComponent_SetIsAnimationPlaying(uint64_t entityID, bool value)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetIsAnimationPlaying - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetIsAnimationPlaying - Entity doesn't have an AnimationComponent!");
				return;
			}

			entity.GetComponent<AnimationComponent>().AnimationData->m_IsAnimationPlaying = value;
		}

		float AnimationComponent_GetPlaybackSpeed(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetPlaybackSpeed - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetPlaybackSpeed - Entity doesn't have an AnimationComponent!");
				return false;
			}

			return entity.GetComponent<AnimationComponent>().AnimationData->m_PlaybackSpeed;
		}

		void AnimationComponent_SetPlaybackSpeed(uint64_t entityID, float value)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetPlaybackSpeed - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetPlaybackSpeed - Entity doesn't have an AnimationComponent!");
				return;
			}

			entity.GetComponent<AnimationComponent>().AnimationData->m_PlaybackSpeed = value;
		}

		float AnimationComponent_GetAnimationTime(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetPlaybackSpeed - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetPlaybackSpeed - Entity doesn't have an AnimationComponent!");
				return false;
			}

			return entity.GetComponent<AnimationComponent>().AnimationData->m_AnimationTime;
		}

		void AnimationComponent_SetAnimationTime(uint64_t entityID, float value)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetPlaybackSpeed - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetPlaybackSpeed - Entity doesn't have an AnimationComponent!");
				return;
			}

			entity.GetComponent<AnimationComponent>().AnimationData->m_AnimationTime = value;
		}

		uint32_t AnimationComponent_GetStateIndex(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetStateIndex - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetStateIndex - Entity doesn't have an AnimationComponent!");
				return false;
			}

			return (uint32_t)entity.GetComponent<AnimationComponent>().AnimationData->m_StateIndex;
		}

		void AnimationComponent_SetStateIndex(uint64_t entityID, uint32_t value)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetStateIndex - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetStateIndex - Entity doesn't have an AnimationComponent!");
				return;
			}

			auto& animationComponent = entity.GetComponent<AnimationComponent>();
			if (AssetManager::IsAssetHandleValid(animationComponent.AnimationController))
			{
				AssetManager::GetAsset<AnimationController>(animationComponent.AnimationController)->SetStateIndex(value, *animationComponent.AnimationData);
			}
		}

		bool AnimationComponent_GetEnableRootMotion(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetEnableRootMotion - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetEnableRootMotion - Entity doesn't have an AnimationComponent!");
				return false;
			}

			return entity.GetComponent<AnimationComponent>().EnableRootMotion;
		}

		void AnimationComponent_SetEnableRootMotion(uint64_t entityID, bool value)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetEnableRootMotion - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.SetEnableRootMotion - Entity doesn't have an AnimationComponent!");
				return;
			}

			entity.GetComponent<AnimationComponent>().EnableRootMotion = value;
		}

		void AnimationComponent_GetRootMotion(uint64_t entityID, TransformComponent* outTransform)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetRootMotion - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<AnimationComponent>())
			{
				AS_CONSOLE_LOG_ERROR("AnimationComponent.GetRootMotion - Entity doesn't have an AnimationComponent!");
				return;
			}

			auto& animationComponent = entity.GetComponent<AnimationComponent>();
			auto& rootMotion = animationComponent.AnimationData->m_RootMotion;
			outTransform->Translation = rootMotion.Translation;
			outTransform->SetRotation(rootMotion.Rotation);
			// outTransform->Scale defaults to 1.0, which is what we want.
		}

#pragma endregion

#pragma region SpotLightComponent

		void SpotLightComponent_GetRadiance(uint64_t entityID, glm::vec3* outRadiance)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetRadiance - Invalid entity!");
				*outRadiance = glm::vec3(0.0f);
				return;
			}

			*outRadiance = entity.GetComponent<SpotLightComponent>().Radiance;
		}

		void SpotLightComponent_SetRadiance(uint64_t entityID, glm::vec3* inRadiance)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetRadiance - Invalid entity!");
				return;
			}

			if (inRadiance == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetRadiance - Attempting to set null radiance on SpotLightComponent!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().Radiance = *inRadiance;
		}

		float SpotLightComponent_GetIntensity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetIntensity - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SpotLightComponent>().Intensity;
		}

		void SpotLightComponent_SetIntensity(uint64_t entityID, float intensity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetIntensity - Invalid entity!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().Intensity = intensity;
		}

		float SpotLightComponent_GetRange(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetRange - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SpotLightComponent>().Range;
		}

		void SpotLightComponent_SetRange(uint64_t entityID, float range)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetRange - Invalid entity!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().Range = range;
		}

		float SpotLightComponent_GetAngle(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetRadius - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SpotLightComponent>().Angle;
		}

		void SpotLightComponent_SetAngle(uint64_t entityID, float angle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetAngle - Invalid entity!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().Angle = angle;
		}

		float SpotLightComponent_GetFalloff(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetFalloff - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SpotLightComponent>().Falloff;
		}

		void SpotLightComponent_SetFalloff(uint64_t entityID, float falloff)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetFalloff - Invalid entity!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().Falloff = falloff;
		}

		float SpotLightComponent_GetAngleAttenuation(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetAngleAttenuation - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SpotLightComponent>().AngleAttenuation;
		}

		void SpotLightComponent_SetAngleAttenuation(uint64_t entityID, float angleAttenuation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetAngleAttenuation - Invalid entity!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().AngleAttenuation = angleAttenuation;
		}

		bool SpotLightComponent_GetCastsShadows(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetCastsShadows - Invalid entity!");
				return false;
			}

			return entity.GetComponent<SpotLightComponent>().CastsShadows;
		}

		void SpotLightComponent_SetCastsShadows(uint64_t entityID, bool castsShadows)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetCastsShadows - Invalid entity!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().CastsShadows = castsShadows;
		}

		bool SpotLightComponent_GetSoftShadows(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.GetSoftShadows - Invalid entity!");
				return false;
			}

			return entity.GetComponent<SpotLightComponent>().SoftShadows;
		}

		void SpotLightComponent_SetSoftShadows(uint64_t entityID, bool softShadows)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpotLightComponent.SetSoftShadows - Invalid entity!");
				return;
			}

			entity.GetComponent<SpotLightComponent>().SoftShadows = softShadows;
		}

#pragma endregion

#pragma region ScriptComponent

		MonoObject* ScriptComponent_GetInstance(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("ScriptComponent.Instance - Invalid entity!");
				return nullptr;
			}

			if (!entity.HasComponent<ScriptComponent>())
			{
				AS_CONSOLE_LOG_ERROR("ScriptComponent.Instance - Entity doesn't have a ScriptComponent?");
				return nullptr;
			}

			const auto& component = entity.GetComponent<ScriptComponent>();

			if (!ScriptEngine::IsModuleValid(component.ScriptClassHandle))
			{
				AS_CONSOLE_LOG_ERROR("ScriptComponent.Instance - Entity is referencing an invalid C# class!");
				return nullptr;
			}

			if (!ScriptEngine::IsEntityInstantiated(entity))
			{
				// Check if the entity is instantiated WITHOUT checking if the OnCreate method has run
				if (ScriptEngine::IsEntityInstantiated(entity, false))
				{
					// If so, call OnCreate here...
					ScriptEngine::CallMethod(component.ManagedInstance, "OnCreate");
			
					// NOTE(Peter): Don't use scriptComponent as a reference and modify it here
					//				If OnCreate spawns a lot of entities we would loose our reference
					//				to the script component...
					entity.GetComponent<ScriptComponent>().IsRuntimeInitialized = true;
			
					return GCManager::GetReferencedObject(component.ManagedInstance);
				}
				else if (component.ManagedInstance == nullptr)
				{
					ScriptEngine::RuntimeInitializeScriptEntity(entity);
					return GCManager::GetReferencedObject(component.ManagedInstance);
				}
			
				AS_CONSOLE_LOG_ERROR("ScriptComponent.Instance - Entity '{0}' isn't instantiated?", entity.Name());
				return nullptr;
			}

			return GCManager::GetReferencedObject(component.ManagedInstance);
		}

#pragma endregion

#pragma region CameraComponent

		void CameraComponent_SetPerspective(uint64_t entityID, float inVerticalFOV, float inNearClip, float inFarClip)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			camera.SetPerspective(inVerticalFOV, inNearClip, inFarClip);
		}

		void CameraComponent_SetOrthographic(uint64_t entityID, float inSize, float inNearClip, float inFarClip)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			camera.SetOrthographic(inSize, inNearClip, inFarClip);
		}

		float CameraComponent_GetVerticalFOV(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.GetVerticalFOV - Invalid entity!");
				return 0.0f;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.GetVerticalFOV - Entity doesn't have a CameraComponent");
				return 0.0f;
			}

			const auto& component = entity.GetComponent<CameraComponent>();
			return component.Camera.GetDegPerspectiveVerticalFOV();
		}

		void CameraComponent_SetVerticalFOV(uint64_t entityID, float inVerticalFOV)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetVerticalFOV - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetVerticalFOV - Entity doesn't have a CameraComponent");
				return;
			}

			auto& component = entity.GetComponent<CameraComponent>();
			return component.Camera.SetDegPerspectiveVerticalFOV(inVerticalFOV);
		}

		float CameraComponent_GetPerspectiveNearClip(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return 0.0f;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return 0.0f;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			return camera.GetPerspectiveNearClip();
		}

		void CameraComponent_SetPerspectiveNearClip(uint64_t entityID, float inNearClip)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			camera.SetPerspectiveNearClip(inNearClip);
		}

		float CameraComponent_GetPerspectiveFarClip(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return 0.0f;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return 0.0f;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			return camera.GetPerspectiveFarClip();
		}

		void CameraComponent_SetPerspectiveFarClip(uint64_t entityID, float inFarClip)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			camera.SetPerspectiveFarClip(inFarClip);
		}

		float CameraComponent_GetOrthographicSize(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return 0.0f;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return 0.0f;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			return camera.GetOrthographicSize();
		}

		void CameraComponent_SetOrthographicSize(uint64_t entityID, float inSize)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			camera.SetOrthographicSize(inSize);
		}

		float CameraComponent_GetOrthographicNearClip(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return 0.0f;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return 0.0f;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			return camera.GetOrthographicNearClip();
		}

		void CameraComponent_SetOrthographicNearClip(uint64_t entityID, float inNearClip)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			camera.SetOrthographicNearClip(inNearClip);
		}

		float CameraComponent_GetOrthographicFarClip(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return 0.0f;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return 0.0f;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			return camera.GetOrthographicFarClip();
		}

		void CameraComponent_SetOrthographicFarClip(uint64_t entityID, float inFarClip)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
			camera.SetOrthographicFarClip(inFarClip);
		}

		CameraComponent::Type CameraComponent_GetProjectionType(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return CameraComponent::Type::None;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return CameraComponent::Type::None;
			}

			const auto& component = entity.GetComponent<CameraComponent>();
			return component.ProjectionType;
		}

		void CameraComponent_SetProjectionType(uint64_t entityID, CameraComponent::Type inType)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			auto& component = entity.GetComponent<CameraComponent>();
			component.Camera.SetProjectionType((SceneCamera::ProjectionType)inType);
		}

		bool CameraComponent_GetPrimary(uint64_t entityID)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return false;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return false;
			}

			const auto& component = entity.GetComponent<CameraComponent>();
			return component.Primary;
		}

		void CameraComponent_SetPrimary(uint64_t entityID, bool inValue)
		{
			Entity entity = GetEntity(entityID);

			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Invalid entity!");
				return;
			}

			if (!entity.HasComponent<CameraComponent>())
			{
				AS_CONSOLE_LOG_ERROR("CameraComponent.SetPerspective - Entity doesn't have a CameraComponent");
				return;
			}

			auto& component = entity.GetComponent<CameraComponent>();
			component.Primary = inValue;
		}

#pragma endregion

#pragma region DirectionalLightComponent

		void DirectionalLightComponent_GetRadiance(uint64_t entityID, glm::vec3* outRadiance)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.GetRadiance - Invalid entity!");
				*outRadiance = glm::vec3(0.0f);
				return;
			}

			*outRadiance = entity.GetComponent<DirectionalLightComponent>().Radiance;
		}

		void DirectionalLightComponent_SetRadiance(uint64_t entityID, glm::vec3* inRadiance)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.SetRadiance - Invalid entity!");
				return;
			}

			entity.GetComponent<DirectionalLightComponent>().Radiance = *inRadiance;
		}

		float DirectionalLightComponent_GetIntensity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.GetIntensity - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<DirectionalLightComponent>().Intensity;
		}

		void DirectionalLightComponent_SetIntensity(uint64_t entityID, float intensity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.SetIntensity - Invalid entity!");
				return;
			}

			entity.GetComponent<DirectionalLightComponent>().Intensity = intensity;
		}

		bool DirectionalLightComponent_GetCastShadows(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.GetCastShadows - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<DirectionalLightComponent>().CastShadows;
		}

		void DirectionalLightComponent_SetCastShadows(uint64_t entityID, bool castShadows)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.SetCastShadows - Invalid entity!");
				return;
			}

			entity.GetComponent<DirectionalLightComponent>().CastShadows = castShadows;
		}

		bool DirectionalLightComponent_GetSoftShadows(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.GetCastShadows - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<DirectionalLightComponent>().SoftShadows;
		}

		void DirectionalLightComponent_SetSoftShadows(uint64_t entityID, bool softShadows)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.SetCastShadows - Invalid entity!");
				return;
			}

			entity.GetComponent<DirectionalLightComponent>().SoftShadows = softShadows;
		}

		float DirectionalLightComponent_GetLightSize(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.GetLightSize - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<DirectionalLightComponent>().LightSize;
		}

		void DirectionalLightComponent_SetLightSize(uint64_t entityID, float lightSize)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("DirectionalLightComponent.SetLightSize - Invalid entity!");
				return;
			}

			entity.GetComponent<DirectionalLightComponent>().LightSize = lightSize;
		}

#pragma endregion

#pragma region PointLightComponent

		void PointLightComponent_GetRadiance(uint64_t entityID, glm::vec3* outRadiance)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.GetRadiance - Invalid entity!");
				*outRadiance = glm::vec3(0.0f);
				return;
			}

			*outRadiance = entity.GetComponent<PointLightComponent>().Radiance;
		}

		void PointLightComponent_SetRadiance(uint64_t entityID, glm::vec3* inRadiance)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.SetRadiance - Invalid entity!");
				return;
			}

			if (inRadiance == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.SetRadiance - Attempting to set null radiance on PointLightComponent!");
				return;
			}

			entity.GetComponent<PointLightComponent>().Radiance = *inRadiance;
		}

		float PointLightComponent_GetIntensity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.GetIntensity - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<PointLightComponent>().Intensity;
		}

		void PointLightComponent_SetIntensity(uint64_t entityID, float intensity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.SetIntensity - Invalid entity!");
				return;
			}

			entity.GetComponent<PointLightComponent>().Intensity = intensity;
		}

		float PointLightComponent_GetRadius(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.GetRadius - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<PointLightComponent>().Radius;
		}

		void PointLightComponent_SetRadius(uint64_t entityID, float radius)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.SetRadius - Invalid entity!");
				return;
			}

			entity.GetComponent<PointLightComponent>().Radius = radius;
		}

		float PointLightComponent_GetFalloff(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.GetFalloff - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<PointLightComponent>().Falloff;
		}

		void PointLightComponent_SetFalloff(uint64_t entityID, float falloff)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("PointLightComponent.SetFalloff - Invalid entity!");
				return;
			}

			entity.GetComponent<PointLightComponent>().Falloff = falloff;
		}

#pragma endregion

#pragma region SkyLightComponent

		float SkyLightComponent_GetIntensity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.GetIntensity - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SkyLightComponent>().Intensity;
		}

		void SkyLightComponent_SetIntensity(uint64_t entityID, float intensity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.SetIntensity - Invalid entity!");
				return;
			}

			entity.GetComponent<SkyLightComponent>().Intensity = intensity;
		}

		float SkyLightComponent_GetTurbidity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.GetTurbidity - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SkyLightComponent>().TurbidityAzimuthInclination.x;
		}

		void SkyLightComponent_SetTurbidity(uint64_t entityID, float turbidity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.SetTurbidity - Invalid entity!");
				return;
			}

			entity.GetComponent<SkyLightComponent>().TurbidityAzimuthInclination.x = turbidity;
		}

		float SkyLightComponent_GetAzimuth(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.GetAzimuth - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SkyLightComponent>().TurbidityAzimuthInclination.y;
		}

		void SkyLightComponent_SetAzimuth(uint64_t entityID, float azimuth)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.SetAzimuth - Invalid entity!");
				return;
			}

			entity.GetComponent<SkyLightComponent>().TurbidityAzimuthInclination.y = azimuth;
		}

		float SkyLightComponent_GetInclination(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.GetInclination - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SkyLightComponent>().TurbidityAzimuthInclination.z;
		}

		void SkyLightComponent_SetInclination(uint64_t entityID, float inclination)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SkyLightComponent.SetInclination - Invalid entity!");
				return;
			}

			entity.GetComponent<SkyLightComponent>().TurbidityAzimuthInclination.z = inclination;
		}

#pragma endregion
		
#pragma region SpriteRendererComponent

		void SpriteRendererComponent_GetColor(uint64_t entityID, glm::vec4* outColor)
		{
			auto entity = GetEntity(entityID); // how should we be checking for entities now? because GetEntity will always return an entity it seems
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent.GetColor - Invalid entity!");
				*outColor = glm::vec4(0.0f);
				return;
			}

			*outColor = entity.GetComponent<SpriteRendererComponent>().Color;
			return;
		}

		void SpriteRendererComponent_SetColor(uint64_t entityID, glm::vec4* inColor)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent.SetColor - Invalid entity!");
				*inColor = glm::vec4(0.0f);
				return;
			}

			entity.GetComponent<SpriteRendererComponent>().Color = *inColor;
			return;
		}

		float SpriteRendererComponent_GetTilingFactor(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent.GetTilingFactor - Invalid entity!");
				return 1.0f;
			}

			return entity.GetComponent<SpriteRendererComponent>().TilingFactor;
		}

		void SpriteRendererComponent_SetTilingFactor(uint64_t entityID, float tilingFactor)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent.SetTilingFactor - Invalid entity!");
				return;
			}

			entity.GetComponent<SpriteRendererComponent>().TilingFactor = tilingFactor;
			return;
		}

		void SpriteRendererComponent_GetUVStart(uint64_t entityID, glm::vec2* outUVStart)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent_GetUVStart - Invalid entity!");
				return;
			}

			*outUVStart = entity.GetComponent<SpriteRendererComponent>().UVStart;
		}

		void SpriteRendererComponent_SetUVStart(uint64_t entityID, glm::vec2* inUVStart)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent_GetUVStart - Invalid entity!");
				return;
			}

			entity.GetComponent<SpriteRendererComponent>().UVStart = *inUVStart;
		}

		void SpriteRendererComponent_GetUVEnd(uint64_t entityID, glm::vec2* outUVEnd)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent_GetUVStart - Invalid entity!");
				return;
			}

			*outUVEnd = entity.GetComponent<SpriteRendererComponent>().UVEnd;
		}

		void SpriteRendererComponent_SetUVEnd(uint64_t entityID, glm::vec2* inUVEnd)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SpriteRendererComponent_GetUVStart - Invalid entity!");
				return;
			}

			entity.GetComponent<SpriteRendererComponent>().UVEnd = *inUVEnd;
		}

#pragma endregion*/
/*
#pragma region RigidBody2DComponent

		RigidBody2DComponent::Type RigidBody2DComponent_GetBodyType(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetBodyType - Invalid entity!");
				return RigidBody2DComponent::Type::None;
			}

			return entity.GetComponent<RigidBody2DComponent>().BodyType;
		}

		void RigidBody2DComponent_SetBodyType(uint64_t entityID, RigidBody2DComponent::Type inType)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetBodyType - Invalid entity!");
				return;
			}

			entity.GetComponent<RigidBody2DComponent>().BodyType = inType;
		}

		void RigidBody2DComponent_GetTranslation(uint64_t entityID, glm::vec2* outTranslation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetTranslation - Invalid entity!");
				*outTranslation = glm::vec2(0.0f);
				return;
			}

			const auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetTranslation - No b2Body exists for this entity!");
				*outTranslation = glm::vec2(0.0f);
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			const b2Vec2& translation = body->GetPosition();
			outTranslation->x = translation.x;
			outTranslation->y = translation.y;
		}

		void RigidBody2DComponent_SetTranslation(uint64_t entityID, glm::vec2* inTranslation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetTranslation - Invalid entity!");
				return;
			}

			const auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetTranslation - No b2Body exists for this entity!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->SetTransform(b2Vec2(inTranslation->x, inTranslation->y), body->GetAngle());
		}

		float RigidBody2DComponent_GetRotation(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetRotation - Invalid entity!");
				return 0.0f;
			}

			const auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetRotation - No b2Body exists for this entity!");
				return 0.0f;
			}

			return ((b2Body*)component.RuntimeBody)->GetAngle();
		}

		void RigidBody2DComponent_SetRotation(uint64_t entityID, float rotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetRotation - Invalid entity!");
				return;
			}

			const auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetRotation - No b2Body exists for this entity!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->SetTransform(body->GetPosition(), rotation);
		}

		float RigidBody2DComponent_GetMass(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetMass - Invalid entity!");
				return 0.0f;
			}

			const auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetMass - No b2Body exists for this entity!");
				return 0.0f;
			}

			return ((b2Body*)component.RuntimeBody)->GetMass();
		}

		void RigidBody2DComponent_SetMass(uint64_t entityID, float mass)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetMass - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetMass - No b2Body exists for this entity!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			b2MassData massData;
			body->GetMassData(&massData);
			massData.mass = mass;
			body->SetMassData(&massData);

			component.Mass = mass;
		}

		void RigidBody2DComponent_GetLinearVelocity(uint64_t entityID, glm::vec2* outVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetLinearVelocity - Invalid entity!");
				return;
			}

			const auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetLinearVelocity - No b2Body exists for this entity!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			const b2Vec2& linearVelocity = body->GetLinearVelocity();
			outVelocity->x = linearVelocity.x;
			outVelocity->y = linearVelocity.y;
		}

		void RigidBody2DComponent_SetLinearVelocity(uint64_t entityID, glm::vec2* inVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetLinearVelocity - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetLinearVelocity - No b2Body exists for this entity!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->SetLinearVelocity(b2Vec2(inVelocity->x, inVelocity->y));
		}

		float RigidBody2DComponent_GetGravityScale(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetGravityScale - Invalid entity!");
				return 0.0f;
			}

			const auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.GetGravityScale - No b2Body exists for this entity!");
				return 0.0f;
			}

			return ((b2Body*)component.RuntimeBody)->GetGravityScale();
		}

		void RigidBody2DComponent_SetGravityScale(uint64_t entityID, float gravityScale)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetGravityScale - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.SetGravityScale - No b2Body exists for this entity!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->SetGravityScale(gravityScale);
			component.GravityScale = gravityScale;
		}

		void RigidBody2DComponent_ApplyLinearImpulse(uint64_t entityID, glm::vec2* inImpulse, glm::vec2* inOffset, bool wake)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.ApplyLinearImpulse - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.ApplyLinearImpulse - No b2Body exists for this entity!");
				return;
			}

			if (component.BodyType != RigidBody2DComponent::Type::Dynamic)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.ApplyLinearImpulse - Cannot add linear impulse to non-dynamic body!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->ApplyLinearImpulse(*(const b2Vec2*)inImpulse, body->GetWorldCenter() + *(const b2Vec2*)inOffset, wake);
		}

		void RigidBody2DComponent_ApplyAngularImpulse(uint64_t entityID, float impulse, bool wake)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.ApplyAngularImpulse - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.ApplyAngularImpulse - No b2Body exists for this entity!");
				return;
			}

			if (component.BodyType != RigidBody2DComponent::Type::Dynamic)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.ApplyAngularImpulse - Cannot add angular impulse to non-dynamic body!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->ApplyAngularImpulse(impulse, wake);
		}

		void RigidBody2DComponent_AddForce(uint64_t entityID, glm::vec3* inForce, glm::vec3* inOffset, bool wake)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.AddForce - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.AddForce - No b2Body exists for this entity!");
				return;
			}

			if (component.BodyType != RigidBody2DComponent::Type::Dynamic)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.AddForce - Cannot apply force to non-dynamic body!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->ApplyForce(*(const b2Vec2*)inForce, body->GetWorldCenter() + *(const b2Vec2*)inOffset, wake);
		}

		void RigidBody2DComponent_AddTorque(uint64_t entityID, float torque, bool wake)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.AddTorque - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			if (component.RuntimeBody == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.AddTorque - No b2Body exists for this entity!");
				return;
			}

			if (component.BodyType != RigidBody2DComponent::Type::Dynamic)
			{
				AS_CONSOLE_LOG_ERROR("RigidBody2DComponent.AddTorque - Cannot apply torque to non-dynamic body!");
				return;
			}

			b2Body* body = (b2Body*)component.RuntimeBody;
			body->ApplyTorque(torque, wake);
		}

#pragma endregion
		
#pragma region RigidBodyComponent

		void RigidBodyComponent_GetTranslation(uint64_t entityID, glm::vec3* outTranslation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetTranslation - Invalid entity!");
				*outTranslation = glm::vec3(0.0f);
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetTranslation - No actor found!");
				*outTranslation = glm::vec3(0.0f);
				return;
			}

			*outTranslation = actor->GetTranslation();
		}

		void RigidBodyComponent_SetTranslation(uint64_t entityID, glm::vec3* inTranslation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetTranslation - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetTranslation - No actor found!");
				return;
			}

			if (inTranslation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetTranslation - Cannot set null translation on a RigidBodyComponent!");
				return;
			}

			actor->SetTranslation(*inTranslation);
		}

		void RigidBodyComponent_GetRotation(uint64_t entityID, glm::vec3* outRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetRotation - Invalid entity!");
				*outRotation = glm::vec3(0.0f);
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetRotation - No actor found!");
				*outRotation = glm::vec3(0.0f);
				return;
			}

			*outRotation = actor->GetRotationEuler();
		}

		void RigidBodyComponent_SetRotation(uint64_t entityID, glm::vec3* inRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetRotation - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetRotation - No actor found!");
				return;
			}

			if (inRotation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetRotation - Cannot set null rotation on a RigidBodyComponent!");
				return;
			}

			actor->SetRotation(*inRotation);
		}

		void RigidBodyComponent_AddForce(uint64_t entityID, glm::vec3* inForce, ForceMode forceMode)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddForce - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddForce - No actor found!");
				return;
			}

			if (inForce == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddForce - Cannot add null force to a RigidBodyComponent!");
				return;
			}

			actor->AddForce(*inForce, forceMode);
		}

		void RigidBodyComponent_AddForceAtLocation(uint64_t entityID, glm::vec3* inForce, glm::vec3* inLocation, ForceMode forceMode)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddForce - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddForce - No actor found!");
				return;
			}

			if (inForce == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddForce - Cannot add null force to a RigidBodyComponent!");
				return;
			}

			actor->AddForceAtLocation(*inForce, *inLocation, forceMode);
		}

		void RigidBodyComponent_AddTorque(uint64_t entityID, glm::vec3* inTorque, ForceMode forceMode)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddTorque - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddTorque - No actor found!");
				return;
			}

			if (inTorque == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.AddTorque - Cannot add null torque to a RigidBodyComponent!");
				return;
			}

			actor->AddTorque(*inTorque, forceMode);
		}

		void RigidBodyComponent_GetLinearVelocity(uint64_t entityID, glm::vec3* outVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLinearVelocity - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLinearVelocity - No actor found!");
				return;
			}

			*outVelocity = actor->GetLinearVelocity();
		}

		void RigidBodyComponent_SetLinearVelocity(uint64_t entityID, glm::vec3* inVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLinearVelocity - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLinearVelocity - No actor found!");
				return;
			}

			if (inVelocity == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLinearVelocity - Cannot set null linear velocity on RigidBodyComponent!");
				return;
			}

			actor->SetLinearVelocity(*inVelocity);
		}

		void RigidBodyComponent_GetAngularVelocity(uint64_t entityID, glm::vec3* outVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetAngularVelocity - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetAngularVelocity - No actor found!");
				return;
			}

			*outVelocity = actor->GetAngularVelocity();
		}

		void RigidBodyComponent_SetAngularVelocity(uint64_t entityID, glm::vec3* inVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetAngularVelocity - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetAngularVelocity - No actor found!");
				return;
			}

			if (inVelocity == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetAngularVelocity - Cannot set null Angular velocity on RigidBodyComponent!");
				return;
			}

			actor->SetAngularVelocity(*inVelocity);
		}

		float RigidBodyComponent_GetMaxLinearVelocity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetMaxLinearVelocity - Invalid entity!");
				return 0.0f;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetMaxLinearVelocity - No actor found!");
				return 0.0f;
			}

			return actor->GetMaxLinearVelocity();
		}

		void RigidBodyComponent_SetMaxLinearVelocity(uint64_t entityID, float maxVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetMaxLinearVelocity - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetMaxLinearVelocity - No actor found!");
				return;
			}

			actor->SetMaxLinearVelocity(maxVelocity);
		}

		float RigidBodyComponent_GetMaxAngularVelocity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetMaxAngularVelocity - Invalid entity!");
				return 0.0f;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetMaxAngularVelocity - No actor found!");
				return 0.0f;
			}

			return actor->GetMaxAngularVelocity();
		}

		void RigidBodyComponent_SetMaxAngularVelocity(uint64_t entityID, float maxVelocity)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetMaxAngularVelocity - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetMaxAngularVelocity - No actor found!");
				return;
			}

			actor->SetMaxAngularVelocity(maxVelocity);
		}

		float RigidBodyComponent_GetLinearDrag(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLinearDrag - Invalid entity!");
				return 0.0f;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLinearDrag - No actor found!");
				return 0.0f;
			}

			return actor->GetLinearDrag();
		}

		void RigidBodyComponent_SetLinearDrag(uint64_t entityID, float linearDrag)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLinearDrag - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLinearDrag - No actor found!");
				return;
			}

			actor->SetLinearDrag(linearDrag);
		}

		float RigidBodyComponent_GetAngularDrag(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetAngularDrag - Invalid entity!");
				return 0.0f;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetAngularDrag - No actor found!");
				return 0.0f;
			}

			return actor->GetAngularDrag();
		}

		void RigidBodyComponent_SetAngularDrag(uint64_t entityID, float angularDrag)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetAngularDrag - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetAngularDrag - No actor found!");
				return;
			}

			actor->SetAngularDrag(angularDrag);
		}

		void RigidBodyComponent_Rotate(uint64_t entityID, glm::vec3* inRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.Rotate - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.Rotate - No actor found!");
				return;
			}

			if (inRotation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.Rotate - Cannot rotate by 'null' rotation!");
				return;
			}

			actor->Rotate(*inRotation);
		}

		uint32_t RigidBodyComponent_GetLayer(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLayer - Invalid entity!");
				return 0;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLayer - No actor found!");
				return 0;
			}

			return entity.GetComponent<RigidBodyComponent>().LayerID;
		}

		void RigidBodyComponent_SetLayer(uint64_t entityID, uint32_t layerID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLayer - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLayer - No actor found!");
				return;
			}

			if (!PhysicsLayerManager::IsLayerValid(layerID))
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLayer called with an invalid layer ID '{0}'!", layerID);
				return;
			}

			if (actor->SetSimulationData(layerID))
			{
				auto& component = entity.GetComponent<RigidBodyComponent>();
				component.LayerID = layerID;
			}
			else
			{
				AS_CONSOLE_LOG_ERROR("Cannot change the layer of an actor that has shared collider shapes!");
			}
		}

		MonoString* RigidBodyComponent_GetLayerName(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLayerName - Invalid entity!");
				return nullptr;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLayerName - No actor found!");
				return nullptr;
			}

			const auto& component = entity.GetComponent<RigidBodyComponent>();

			if (!PhysicsLayerManager::IsLayerValid(component.LayerID))
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLayerName can't find a layer with ID '{0}'!", component.LayerID);
				return nullptr;
			}

			const auto& layer = PhysicsLayerManager::GetLayer(component.LayerID);
			return ScriptUtils::UTF8StringToMono(layer.Name);
		}

		void RigidBodyComponent_SetLayerByName(uint64_t entityID, MonoString* inName)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLayerByName - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLayerByName - No actor found!");
				return;
			}

			if (inName == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLayerByName called with 'null' name!");
				return;
			}

			std::string layerName = ScriptUtils::MonoStringToUTF8(inName);

			if (!PhysicsLayerManager::IsLayerValid(layerName))
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLayerByName called with an invalid layer name '{0}'!", layerName);
				return;
			}

			const auto& layer = PhysicsLayerManager::GetLayer(layerName);

			if (actor->SetSimulationData(layer.LayerID))
			{
				auto& component = entity.GetComponent<RigidBodyComponent>();
				component.LayerID = layer.LayerID;
			}
			else
			{
				AS_CONSOLE_LOG_ERROR("Cannot change the layer of an actor that has shared collider shapes!");
			}
		}

		float RigidBodyComponent_GetMass(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetMass - Invalid entity!");
				return 0.0f;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetMass - No actor found!");
				return 0.0f;
			}

			return actor->GetMass();
		}

		void RigidBodyComponent_SetMass(uint64_t entityID, float mass)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetMass - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetMass - No actor found!");
				return;
			}

			actor->SetMass(mass);
		}

		RigidBodyComponent::Type RigidBodyComponent_GetBodyType(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetBodyType - Invalid entity!");
				return RigidBodyComponent::Type::None;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetBodyType - No actor found!");
				return RigidBodyComponent::Type::None;
			}

			return entity.GetComponent<RigidBodyComponent>().BodyType;
		}

		void RigidBodyComponent_SetBodyType(uint64_t entityID, RigidBodyComponent::Type type)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetBodyType - Invalid entity!");
				return;
			}

			auto& rigidbodyComponent = entity.GetComponent<RigidBodyComponent>();

			if (rigidbodyComponent.BodyType == type)
				return;

			auto physicsScene = Scene::GetScene(entity.GetSceneUUID())->GetPhysicsScene();
			physicsScene->RemoveActor(entity);
			rigidbodyComponent.BodyType = type;
			physicsScene->CreateActor(entity);
		}

		bool RigidBodyComponent_IsKinematic(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.IsKinematic - Invalid entity!");
				return false;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.IsKinematic - No actor found!");
				return false;
			}

			return entity.GetComponent<RigidBodyComponent>().IsKinematic;
		}

		void RigidBodyComponent_SetIsKinematic(uint64_t entityID, bool isKinematic)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetIsKinematic - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetIsKinematic - No actor found!");
				return;
			}

			actor->SetKinematic(isKinematic);
		}

		void RigidBodyComponent_GetKinematicTarget(uint64_t entityID, glm::vec3* outTargetPosition, glm::vec3* outTargetRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetKinematicTarget - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetKinematicTarget - No actor found!");
				return;
			}

			*outTargetPosition = actor->GetKinematicTargetPosition();
			*outTargetRotation = actor->GetKinematicTargetRotationEuler();
		}

		void RigidBodyComponent_SetKinematicTarget(uint64_t entityID, glm::vec3* inTargetPosition, glm::vec3* inTargetRotation)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetKinematicTarget - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetKinematicTarget - No actor found!");
				return;
			}

			if (inTargetPosition == nullptr || inTargetRotation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetKinematicTarget - targetPosition or targetRotation is null!");
				return;
			}

			actor->SetKinematicTarget(*inTargetPosition, *inTargetRotation);
		}

		void RigidBodyComponent_SetLockFlag(uint64_t entityID, ActorLockFlag flag, bool value, bool forceWake)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLockFlag - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetLockFlag - No actor found!");
				return;
			}

			actor->SetLockFlag(flag, value, forceWake);
		}

		bool RigidBodyComponent_IsLockFlagSet(uint64_t entityID, ActorLockFlag flag)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.IsLockFlagSet - Invalid entity!");
				return false;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.IsLockFlagSet - No actor found!");
				return false;
			}

			return actor->IsLockFlagSet(flag);
		}

		uint32_t RigidBodyComponent_GetLockFlags(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLockFlags - Invalid entity!");
				return 0;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.GetLockFlags - No actor found!");
				return 0;
			}

			return actor->GetLockFlags();
		}

		bool RigidBodyComponent_IsSleeping(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.IsSleeping - Invalid entity!");
				return false;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.IsSleeping - No actor found!");
				return false;
			}

			return actor->IsSleeping();
		}

		void RigidBodyComponent_SetIsSleeping(uint64_t entityID, bool isSleeping)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetIsSleeping - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("RigidBodyComponent.SetIsSleeping - No actor found!");
				return;
			}

			if (isSleeping)
				actor->PutToSleep();
			else
				actor->WakeUp();
		}

#pragma endregion

#pragma region CharacterControllerComponent

		static inline Ref<PhysicsController> GetPhysicsController(Entity entity)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No scene active!");
			Ref<PhysicsScene> physicsScene = scene->GetPhysicsScene();
			AS_CORE_ASSERT(physicsScene, "No physics scene active!");
			return physicsScene->GetController(entity);
		}

		float CharacterControllerComponent_GetSlopeLimit(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetSlopeLimit - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<CharacterControllerComponent>().SlopeLimitDeg;
		}

		void CharacterControllerComponent_SetSlopeLimit(uint64_t entityID, float slopeLimit)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.SetSlopeLimit - Invalid entity!");
				return;
			}

			auto controller = GetPhysicsController(entity);
			if (!controller)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.SetSlopeLimit - No PhysicsController found!");
				return;
			}

			controller->SetSlopeLimit(glm::clamp(slopeLimit, 0.0f, 90.0f));
		}

		float CharacterControllerComponent_GetStepOffset(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetStepOffset - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<CharacterControllerComponent>().StepOffset;
		}

		void CharacterControllerComponent_SetStepOffset(uint64_t entityID, float stepOffset)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.SetStepOffset - Invalid entity!");
				return;
			}

			auto controller = GetPhysicsController(entity);
			if (!controller)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.SetStepOffset - No PhysicsController found!");
				return;
			}

			controller->SetStepOffset(stepOffset);
		}

		void CharacterControllerComponent_Move(uint64_t entityID, glm::vec3* inDisplacement)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.Move - Invalid entity!");
				return;
			}

			auto controller = GetPhysicsController(entity);
			if (!controller)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.Move - No PhysicsController found!");
				return;
			}

			if (inDisplacement == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.Move - Cannot move CharacterControllerComponent by a null displacement!");
				return;
			}

			controller->Move(*inDisplacement);
		}

		void CharacterControllerComponent_Jump(uint64_t entityID, float jumpPower)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.Jump - Invalid entity!");
				return;
			}

			auto controller = GetPhysicsController(entity);
			if (!controller)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.Jump - No PhysicsController found!");
				return;
			}

			controller->Jump(jumpPower);
		}

		float CharacterControllerComponent_GetSpeedDown(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetSpeedDown - Invalid entity!");
				return 0.0f;
			}

			auto controller = GetPhysicsController(entity);
			if (!controller)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetSpeedDown - No PhysicsController found!");
				return 0.0f;
			}

			return controller->GetSpeedDown();
		}

		bool CharacterControllerComponent_IsGrounded(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetSpeedDown - Invalid entity!");
				return false;
			}

			auto controller = GetPhysicsController(entity);
			if (!controller)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetSpeedDown - No PhysicsController found!");
				return false;
			}

			return controller->IsGrounded();
		}

		CollisionFlags CharacterControllerComponent_GetCollisionFlags(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetSpeedDown - Invalid entity!");
				return CollisionFlags::None;
			}

			auto controller = GetPhysicsController(entity);
			if (!controller)
			{
				AS_CONSOLE_LOG_ERROR("CharacterControllerComponent.GetSpeedDown - No PhysicsController found!");
				return CollisionFlags::None;
			}

			return controller->GetCollisionFlags();
		}

#pragma endregion

#pragma region FixedJointComponent

		static inline Ref<JointBase> GetJoint(Entity entity)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No scene active!");
			Ref<PhysicsScene> physicsScene = scene->GetPhysicsScene();
			AS_CORE_ASSERT(physicsScene, "No physics scene active!");
			return physicsScene->GetJoint(entity);
		}

		uint64_t FixedJointComponent_GetConnectedEntity(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.GetConnectedEntity - Invalid entity!");
				return 0;
			}

			return entity.GetComponent<FixedJointComponent>().ConnectedEntity;
		}

		void FixedJointComponent_SetConnectedEntity(uint64_t entityID, uint64_t connectedEntityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetConnectedEntity - Invalid entity!");
				return;
			}

			auto connectedEntity = GetEntity(connectedEntityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetConnectedEntity - Invalid connectedEntity!");
				return;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetConnectedEntity - No Joint found!");
				return;
			}

			joint->SetConnectedEntity(connectedEntity);
		}

		bool FixedJointComponent_IsBreakable(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.IsBreakable - Invalid entity!");
				return false;
			}

			return entity.GetComponent<FixedJointComponent>().IsBreakable;
		}

		void FixedJointComponent_SetIsBreakable(uint64_t entityID, bool isBreakable)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetIsBreakable - Invalid entity!");
				return;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetIsBreakable - No Joint found!");
				return;
			}

			const auto& component = entity.GetComponent<FixedJointComponent>();

			if (isBreakable)
				joint->SetBreakForceAndTorque(component.BreakForce, component.BreakTorque);
			else
				joint->SetBreakForceAndTorque(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		}

		bool FixedJointComponent_IsBroken(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.IsBroken - Invalid entity!");
				return false;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.IsBroken - No Joint found!");
				return false;
			}

			return joint->IsBroken();
		}

		void FixedJointComponent_Break(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.Break - Invalid entity!");
				return;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.Break - No Joint found!");
				return;
			}

			joint->Break();
		}

		float FixedJointComponent_GetBreakForce(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.GetBreakForce - Invalid entity!");
				return 0.0f;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.GetBreakForce - No Joint found!");
				return 0.0f;
			}

			float breakForce, breakTorque;
			joint->GetBreakForceAndTorque(breakForce, breakTorque);
			return breakForce;
		}

		void FixedJointComponent_SetBreakForce(uint64_t entityID, float breakForce)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetBreakForce - Invalid entity!");
				return;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetBreakForce - No Joint found!");
				return;
			}

			float prevBreakForce, breakTorque;
			joint->GetBreakForceAndTorque(prevBreakForce, breakTorque);
			joint->SetBreakForceAndTorque(breakForce, breakTorque);
		}

		float FixedJointComponent_GetBreakTorque(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.GetBreakTorque - Invalid entity!");
				return 0.0f;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.GetBreakTorque - No Joint found!");
				return 0.0f;
			}

			float breakForce, breakTorque;
			joint->GetBreakForceAndTorque(breakForce, breakTorque);
			return breakTorque;
		}

		void FixedJointComponent_SetBreakTorque(uint64_t entityID, float breakTorque)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetBreakTorque - Invalid entity!");
				return;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetBreakTorque - No Joint found!");
				return;
			}

			float breakForce, prevBreakTorque;
			joint->GetBreakForceAndTorque(breakForce, prevBreakTorque);
			joint->SetBreakForceAndTorque(breakForce, breakTorque);
		}

		bool FixedJointComponent_IsCollisionEnabled(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.IsCollisionEnabled - Invalid entity!");
				return false;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.IsCollisionEnabled - No Joint found!");
				return false;
			}

			return joint->IsCollisionEnabled();
		}

		void FixedJointComponent_SetCollisionEnabled(uint64_t entityID, bool isCollisionEnabled)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetCollisionEnabled - Invalid entity!");
				return;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetCollisionEnabled - No Joint found!");
				return;
			}

			joint->SetCollisionEnabled(isCollisionEnabled);
		}

		bool FixedJointComponent_IsPreProcessingEnabled(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.IsPreProcessingEnabled - Invalid entity!");
				return false;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.IsPreProcessingEnabled - No Joint found!");
				return false;
			}

			return joint->IsPreProcessingEnabled();
		}

		void FixedJointComponent_SetPreProcessingEnabled(uint64_t entityID, bool isPreProcessingEnabled)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetPreProcessingEnabled - Invalid entity!");
				return;
			}

			auto joint = GetJoint(entity);
			if (!joint)
			{
				AS_CONSOLE_LOG_ERROR("FixedJointComponent.SetPreProcessingEnabled - No Joint found!");
				return;
			}

			joint->SetPreProcessingEnabled(isPreProcessingEnabled);
		}

#pragma endregion

#pragma region BoxColliderComponent

		void BoxColliderComponent_GetHalfSize(uint64_t entityID, glm::vec3* outSize)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.GetHalfSize - Invalid entity!");
				return;
			}

			*outSize = entity.GetComponent<BoxColliderComponent>().HalfSize;
		}

		void BoxColliderComponent_SetHalfSize(uint64_t entityID, glm::vec3* inSize)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetHalfSize - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetHalfSize - No PhysicsActor found!");
				return;
			}

			auto boxCollider = actor->GetCollider<BoxColliderShape>();
			if (!boxCollider)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetHalfSize - No BoxColliderShape found?");
				return;
			}

			boxCollider->SetHalfSize(*inSize);
			actor->WakeUp();
		}

		void BoxColliderComponent_GetOffset(uint64_t entityID, glm::vec3* outOffset)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.GetOffset - Invalid entity!");
				return;
			}

			*outOffset = entity.GetComponent<BoxColliderComponent>().Offset;
		}

		void BoxColliderComponent_SetOffset(uint64_t entityID, glm::vec3* inOffset)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetOffset - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetOffset - No PhysicsActor found!");
				return;
			}

			auto boxCollider = actor->GetCollider<BoxColliderShape>();
			if (!boxCollider)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetOffset - No BoxColliderShape found?");
				return;
			}

			boxCollider->SetOffset(*inOffset);
			actor->WakeUp();
		}

		bool BoxColliderComponent_IsTrigger(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.IsTrigger - Invalid entity!");
				return false;
			}

			return entity.GetComponent<BoxColliderComponent>().IsTrigger;
		}

		void BoxColliderComponent_SetTrigger(uint64_t entityID, bool isTrigger)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetTrigger - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetTrigger - No PhysicsActor found!");
				return;
			}

			auto boxCollider = actor->GetCollider<BoxColliderShape>();
			if (!boxCollider)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.SetTrigger - No BoxColliderShape found?");
				return;
			}

			boxCollider->SetTrigger(isTrigger);
			actor->WakeUp();
		}

		bool BoxColliderComponent_GetMaterialHandle(uint64_t entityID, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("BoxColliderComponent.GetMaterialHandle - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = entity.GetComponent<BoxColliderComponent>().Material;
			return true;
		}

#pragma endregion

#pragma region SphereColliderComponent

		float SphereColliderComponent_GetRadius(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.GetRadius - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<SphereColliderComponent>().Radius;
		}

		void SphereColliderComponent_SetRadius(uint64_t entityID, float radius)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetRadius - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetRadius - No PhysicsActor found!");
				return;
			}

			auto sphereCollider = actor->GetCollider<SphereColliderShape>();
			if (!sphereCollider)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetRadius - No SphereColliderShape found?");
				return;
			}

			sphereCollider->SetRadius(radius);
			actor->WakeUp();
		}

		void SphereColliderComponent_GetOffset(uint64_t entityID, glm::vec3* outOffset)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.GetOffset - Invalid entity!");
				return;
			}

			*outOffset = entity.GetComponent<SphereColliderComponent>().Offset;
		}

		void SphereColliderComponent_SetOffset(uint64_t entityID, glm::vec3* inOffset)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetOffset - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetOffset - No PhysicsActor found!");
				return;
			}

			auto sphereCollider = actor->GetCollider<SphereColliderShape>();
			if (!sphereCollider)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetOffset - No SphereColliderShape found?");
				return;
			}

			sphereCollider->SetOffset(*inOffset);
			actor->WakeUp();
		}

		bool SphereColliderComponent_IsTrigger(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.IsTrigger - Invalid entity!");
				return false;
			}

			return entity.GetComponent<BoxColliderComponent>().IsTrigger;
		}

		void SphereColliderComponent_SetTrigger(uint64_t entityID, bool isTrigger)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetTrigger - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetTrigger - No PhysicsActor found!");
				return;
			}

			auto sphereCollider = actor->GetCollider<SphereColliderShape>();
			if (!sphereCollider)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.SetTrigger - No SphereColliderShape found?");
				return;
			}

			sphereCollider->SetTrigger(isTrigger);
			actor->WakeUp();
		}

		bool SphereColliderComponent_GetMaterialHandle(uint64_t entityID, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("SphereColliderComponent.GetMaterialHandle - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = entity.GetComponent<SphereColliderComponent>().Material;
			return true;
		}

#pragma endregion

#pragma region CapsuleColliderComponent

		float CapsuleColliderComponent_GetRadius(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.GetRadius - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<CapsuleColliderComponent>().Radius;
		}

		void CapsuleColliderComponent_SetRadius(uint64_t entityID, float radius)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetRadius - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetRadius - No PhysicsActor found!");
				return;
			}

			auto capsuleCollider = actor->GetCollider<CapsuleColliderShape>();
			if (!capsuleCollider)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetRadius - No CapsuleColliderShape found?");
				return;
			}

			capsuleCollider->SetRadius(radius);
			actor->WakeUp();
		}

		float CapsuleColliderComponent_GetHeight(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.GetHeight - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<CapsuleColliderComponent>().Height;
		}

		void CapsuleColliderComponent_SetHeight(uint64_t entityID, float height)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetHeight - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetHeight - No PhysicsActor found!");
				return;
			}

			auto capsuleCollider = actor->GetCollider<CapsuleColliderShape>();
			if (!capsuleCollider)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetHeight - No CapsuleColliderShape found?");
				return;
			}

			capsuleCollider->SetHeight(height);
			actor->WakeUp();
		}

		void CapsuleColliderComponent_GetOffset(uint64_t entityID, glm::vec3* outOffset)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.GetOffset - Invalid entity!");
				return;
			}

			*outOffset = entity.GetComponent<CapsuleColliderComponent>().Offset;
		}

		void CapsuleColliderComponent_SetOffset(uint64_t entityID, glm::vec3* inOffset)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetOffset - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetOffset - No PhysicsActor found!");
				return;
			}

			auto capsuleCollider = actor->GetCollider<CapsuleColliderShape>();
			if (!capsuleCollider)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetOffset - No CapsuleColliderShape found?");
				return;
			}

			capsuleCollider->SetOffset(*inOffset);
			actor->WakeUp();
		}

		bool CapsuleColliderComponent_IsTrigger(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.IsTrigger - Invalid entity!");
				return false;
			}

			return entity.GetComponent<CapsuleColliderComponent>().IsTrigger;
		}

		void CapsuleColliderComponent_SetTrigger(uint64_t entityID, bool isTrigger)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetTrigger - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetTrigger - No PhysicsActor found!");
				return;
			}

			auto capsuleCollider = actor->GetCollider<CapsuleColliderShape>();
			if (!capsuleCollider)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.SetTrigger - No CapsuleColliderShape found?");
				return;
			}

			capsuleCollider->SetTrigger(isTrigger);
			actor->WakeUp();
		}

		bool CapsuleColliderComponent_GetMaterialHandle(uint64_t entityID, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("CapsuleColliderComponent.GetMaterialHandle - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = entity.GetComponent<CapsuleColliderComponent>().Material;
			return true;
		}

#pragma endregion

#pragma region MeshColliderComponent

		bool MeshColliderComponent_IsMeshStatic(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.IsMeshStatic - Invalid entity!");
				return false;
			}

			const auto& component = entity.GetComponent<MeshColliderComponent>();
			Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(component.ColliderAsset);

			if (!colliderAsset)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.IsMeshStatic - Invalid collider asset!");
				return false;
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(colliderAsset->ColliderMesh);
			if (!metadata.IsValid())
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.IsMeshStatic - Invalid mesh!");
				return false;
			}

			return metadata.Type == AssetType::StaticMesh;
		}

		bool MeshColliderComponent_IsColliderMeshValid(uint64_t entityID, AssetHandle* meshHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.IsColliderMeshValid - Invalid entity!");
				return false;
			}

			const auto& component = entity.GetComponent<MeshColliderComponent>();
			Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(component.ColliderAsset);

			if (!colliderAsset)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.IsColliderMeshValid - Invalid collider asset!");
				return false;
			}

			return *meshHandle == colliderAsset->ColliderMesh;
		}

		bool MeshColliderComponent_GetColliderMesh(uint64_t entityID, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.GetColliderMesh - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			const auto& component = entity.GetComponent<MeshColliderComponent>();
			Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(component.ColliderAsset);

			if (!colliderAsset)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.GetColliderMesh - Invalid collider asset!");
				return false;
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(colliderAsset->ColliderMesh);
			if (!metadata.IsValid())
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.GetColliderMesh - This component doesn't have a valid collider mesh!");
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = colliderAsset->ColliderMesh;
			return true;
		}

		void MeshColliderComponent_SetColliderMesh(uint64_t entityID, AssetHandle* meshHandle)
		{

		}

		bool MeshColliderComponent_IsTrigger(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.IsTrigger - Invalid entity!");
				return false;
			}

			const auto& component = entity.GetComponent<MeshColliderComponent>();
			return component.IsTrigger;
		}

		void MeshColliderComponent_SetTrigger(uint64_t entityID, bool isTrigger)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.SetTrigger - Invalid entity!");
				return;
			}

			auto actor = GetPhysicsActor(entity);
			if (!actor)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.SetTrigger - No PhysicsActor found!");
				return;
			}

			const auto& component = entity.GetComponent<MeshColliderComponent>();
			Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(component.ColliderAsset);

			if (!colliderAsset)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.SetTrigger - Invalid collider asset!");
				return;
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(colliderAsset->ColliderMesh);
			if (!metadata.IsValid())
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.SetTrigger - This component doesn't have a valid collider mesh!");
				return;
			}

			auto convexMeshCollider = actor->GetCollider<ConvexMeshShape>();
			if (!convexMeshCollider)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.SetTrigger - Failed to find Simple Shape! Only simple shapes can be trigger shapes.");
				return;
			}

			convexMeshCollider->SetTrigger(isTrigger);
		}

		bool MeshColliderComponent_GetMaterialHandle(uint64_t entityID, AssetHandle* outHandle)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.GetMaterialHandle - Invalid entity!");
				*outHandle = AssetHandle(0);
				return false;
			}

			const auto& component = entity.GetComponent<MeshColliderComponent>();
			Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(component.ColliderAsset);

			if (!colliderAsset)
			{
				AS_CONSOLE_LOG_ERROR("MeshColliderComponent.GetMaterialHandle - Invalid collider asset!");
				return false;
			}

			*outHandle = colliderAsset->Material;
			if (AssetManager::IsAssetHandleValid(component.OverrideMaterial))
				*outHandle = component.OverrideMaterial;

			return true;
		}

#pragma endregion

#pragma region MeshCollider

		bool MeshCollider_IsStaticMesh(AssetHandle* meshHandle)
		{
			if (!AssetManager::IsAssetHandleValid(*meshHandle))
				return false;

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(*meshHandle);

			if (metadata.Type != AssetType::StaticMesh && metadata.Type != AssetType::Mesh)
			{
				AS_CONSOLE_LOG_WARN("MeshCollider recieved AssetHandle to a non-mesh asset?");
				return false;
			}

			return metadata.Type == AssetType::StaticMesh;
		}

#pragma endregion

#pragma region PhysicsMaterial

		float PhysicsMaterial_GetStaticFriction(AssetHandle* handle)
		{
			if (!AssetManager::IsAssetHandleValid(*handle))
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetStaticFriction called on an invalid PhysicsMaterial instance!");
				return std::numeric_limits<float>::max();
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(*handle);

			if (metadata.Type != AssetType::PhysicsMat)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetStaticFriction called with an AssetHandle that doesn't represent a PhysicsMaterial!");
				return std::numeric_limits<float>::max();
			}

			Ref<PhysicsMaterial> material = AssetManager::GetAsset<PhysicsMaterial>(*handle);
			if (!material)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetStaticFriction called on an invalid PhysicsMaterial!");
				return std::numeric_limits<float>::max();
			}

			return material->StaticFriction;
		}

		// TODO(Peter): Make these set functions actually take effect...

		void PhysicsMaterial_SetStaticFriction(AssetHandle* handle, float value)
		{
			if (!AssetManager::IsAssetHandleValid(*handle))
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetStaticFriction called on an invalid PhysicsMaterial instance!");
				return;
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(*handle);

			if (metadata.Type != AssetType::PhysicsMat)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetStaticFriction called with an AssetHandle that doesn't represent a PhysicsMaterial!");
				return;
			}

			Ref<PhysicsMaterial> material = AssetManager::GetAsset<PhysicsMaterial>(*handle);
			if (!material)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetStaticFriction called on an invalid PhysicsMaterial!");
				return;
			}

			material->StaticFriction = value;
		}

		float PhysicsMaterial_GetDynamicFriction(AssetHandle* handle)
		{
			if (!AssetManager::IsAssetHandleValid(*handle))
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetDynamicFriction called on an invalid PhysicsMaterial instance!");
				return std::numeric_limits<float>::max();
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(*handle);

			if (metadata.Type != AssetType::PhysicsMat)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetDynamicFriction called with an AssetHandle that doesn't represent a PhysicsMaterial!");
				return std::numeric_limits<float>::max();
			}

			Ref<PhysicsMaterial> material = AssetManager::GetAsset<PhysicsMaterial>(*handle);
			if (!material)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetDynamicFriction called on an invalid PhysicsMaterial!");
				return std::numeric_limits<float>::max();
			}

			return material->DynamicFriction;
		}

		void PhysicsMaterial_SetDynamicFriction(AssetHandle* handle, float value)
		{
			if (!AssetManager::IsAssetHandleValid(*handle))
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetDynamicFriction called on an invalid PhysicsMaterial instance!");
				return;
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(*handle);

			if (metadata.Type != AssetType::PhysicsMat)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetDynamicFriction called with an AssetHandle that doesn't represent a PhysicsMaterial!");
				return;
			}

			Ref<PhysicsMaterial> material = AssetManager::GetAsset<PhysicsMaterial>(*handle);
			if (!material)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetDynamicFriction called on an invalid PhysicsMaterial!");
				return;
			}

			material->DynamicFriction = value;
		}

		float PhysicsMaterial_GetRestitution(AssetHandle* handle)
		{
			if (!AssetManager::IsAssetHandleValid(*handle))
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetRestitution called on an invalid PhysicsMaterial instance!");
				return std::numeric_limits<float>::max();
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(*handle);

			if (metadata.Type != AssetType::PhysicsMat)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetRestitution called with an AssetHandle that doesn't represent a PhysicsMaterial!");
				return std::numeric_limits<float>::max();
			}

			Ref<PhysicsMaterial> material = AssetManager::GetAsset<PhysicsMaterial>(*handle);
			if (!material)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.GetRestitution called on an invalid PhysicsMaterial!");
				return std::numeric_limits<float>::max();
			}

			return material->Bounciness;
		}

		void PhysicsMaterial_SetRestitution(AssetHandle* handle, float value)
		{
			if (!AssetManager::IsAssetHandleValid(*handle))
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetRestitution called on an invalid PhysicsMaterial instance!");
				return;
			}

			const auto& metadata = Project::GetEditorAssetManager()->GetMetadata(*handle);

			if (metadata.Type != AssetType::PhysicsMat)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetRestitution called with an AssetHandle that doesn't represent a PhysicsMaterial!");
				return;
			}

			Ref<PhysicsMaterial> material = AssetManager::GetAsset<PhysicsMaterial>(*handle);
			if (!material)
			{
				AS_CONSOLE_LOG_ERROR("PhysicsMaterial.SetRestitution called on an invalid PhysicsMaterial!");
				return;
			}

			material->Bounciness = value;
		}

#pragma endregion

#pragma region AudioComponent

		bool AudioComponent_IsPlaying(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.IsPlaying - Invalid entity!");
				return false;
			}

			return AudioPlayback::IsPlaying(entityID);
		}

		bool AudioComponent_Play(uint64_t entityID, float startTime)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.Play - Invalid entity!");
				return false;
			}

			return AudioPlayback::Play(entityID, startTime);
		}

		bool AudioComponent_Stop(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.Stop - Invalid entity!");
				return false;
			}

			return AudioPlayback::StopActiveSound(entityID);
		}

		bool AudioComponent_Pause(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.Pause - Invalid entity!");
				return false;
			}

			return AudioPlayback::PauseActiveSound(entityID);
		}

		bool AudioComponent_Resume(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.Resume - Invalid entity!");
				return false;
			}

			return AudioPlayback::Resume(entityID);
		}

		float AudioComponent_GetVolumeMult(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.GetVolumeMultiplier - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<AudioComponent>().VolumeMultiplier;
		}

		void AudioComponent_SetVolumeMult(uint64_t entityID, float volumeMultiplier)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.SetVolumeMultiplier - Invalid entity!");
				return;
			}

			entity.GetComponent<AudioComponent>().VolumeMultiplier = volumeMultiplier;
		}

		float AudioComponent_GetPitchMult(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.GetPitchMultiplier - Invalid entity!");
				return 0.0f;
			}

			return entity.GetComponent<AudioComponent>().PitchMultiplier;
		}

		void AudioComponent_SetPitchMult(uint64_t entityID, float pitchMultiplier)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.SetPitchMultiplier - Invalid entity!");
				return;
			}

			entity.GetComponent<AudioComponent>().VolumeMultiplier = pitchMultiplier;
		}

		void AudioComponent_SetEvent(uint64_t entityID, Audio::CommandID eventID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.SetEvent - Invalid entity!");
				return;
			}

			if (!AudioCommandRegistry::DoesCommandExist<Audio::TriggerCommand>(eventID))
			{
				AS_CONSOLE_LOG_ERROR("AudioComponent.SetEvent - TriggerCommand with ID {0} does not exist!", eventID);
				return;
			}

			auto& component = entity.GetComponent<AudioComponent>();
			component.StartCommandID = eventID;
			component.StartEvent = AudioCommandRegistry::GetCommand<Audio::TriggerCommand>(eventID).DebugName;
		}

#pragma endregion

#pragma region TextComponent

		size_t TextComponent_GetHash(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TextComponent.GetHash - Invalid entity!");
				return 0;
			}

			return entity.GetComponent<TextComponent>().TextHash;
		}

		MonoString* TextComponent_GetText(uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TextComponent.GetText - Invalid entity!");
				return ScriptUtils::UTF8StringToMono("");
			}

			const auto& component = entity.GetComponent<TextComponent>();
			return ScriptUtils::UTF8StringToMono(component.TextString);
		}

		void TextComponent_SetText(uint64_t entityID, MonoString* text)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TextComponent.SetText - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<TextComponent>();
			component.TextString = ScriptUtils::MonoStringToUTF8(text);
			component.TextHash = std::hash<std::string>()(component.TextString);
		}

		void TextComponent_GetColor(uint64_t entityID, glm::vec4* outColor)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TextComponent.GetColor - Invalid entity!");
				*outColor = glm::vec4(0.0f);
				return;
			}

			const auto& component = entity.GetComponent<TextComponent>();
			*outColor = component.Color;
		}

		void TextComponent_SetColor(uint64_t entityID, glm::vec4* inColor)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("TextComponent.SetColor - Invalid entity!");
				return;
			}

			auto& component = entity.GetComponent<TextComponent>();
			component.Color = *inColor;
		}

#pragma endregion

#pragma region Audio

		uint32_t Audio_PostEvent(Audio::CommandID eventID, uint64_t entityID)
		{
			if (!AudioCommandRegistry::DoesCommandExist<Audio::TriggerCommand>(eventID))
				return 0;

			return AudioPlayback::PostTrigger(eventID, entityID);
		}

		uint32_t Audio_PostEventFromAC(Audio::CommandID eventID, uint64_t entityID)
		{
			auto entity = GetEntity(entityID);
			if (!entity)
			{
				AS_CONSOLE_LOG_ERROR("Audio.PostEventFromAC - Invalid entity!");
				return 0;
			}

			if (!AudioCommandRegistry::DoesCommandExist<Audio::TriggerCommand>(eventID))
			{
				AS_CONSOLE_LOG_ERROR("Audio.PostEventFromAC - Unable to find TriggerCommand with ID {0}", eventID);
				return 0;
			}

			return AudioPlayback::PostTriggerFromAC(eventID, entityID);
		}

		uint32_t Audio_PostEventAtLocation(Audio::CommandID eventID, Transform* inLocation)
		{
			if (!AudioCommandRegistry::DoesCommandExist<Audio::TriggerCommand>(eventID))
			{
				AS_CONSOLE_LOG_ERROR("Audio.PostEventAtLocation - Unable to find TriggerCommand with ID {0}", eventID);
				return 0;
			}

			if (inLocation == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("Audio.PostEventAtLocation - Cannot post audio event at a location of 'null'!");
				return 0;
			}

			return AudioPlayback::PostTriggerAtLocation(eventID, inLocation->Translation, inLocation->Rotation, inLocation->Scale);
		}

		bool Audio_StopEventID(uint32_t playingEventID)
		{
			return AudioPlayback::StopEventID(playingEventID);
		}

		bool Audio_PauseEventID(uint32_t playingEventID)
		{
			return AudioPlayback::PauseEventID(playingEventID);
		}

		bool Audio_ResumeEventID(uint32_t playingEventID)
		{
			return AudioPlayback::ResumeEventID(playingEventID);
		}

		uint64_t Audio_CreateAudioEntity(Audio::CommandID eventID, Transform* inLocation, float volume, float pitch)
		{
			if (!AudioCommandRegistry::DoesCommandExist<Audio::TriggerCommand>(eventID))
			{
				AS_CONSOLE_LOG_ERROR("Audio.CreateSound - Unable to find TriggerCommand with ID {0}", eventID);
				return 0;
			}

			Ref<Scene> scene = ScriptEngine::GetSceneContext();

			Entity entity = scene->CreateEntity("AudioEntity");
			auto& component = entity.AddComponent<AudioComponent>();

			component.StartCommandID = eventID;
			component.StartEvent = AudioCommandRegistry::GetCommand<Audio::TriggerCommand>(eventID).DebugName;
			component.VolumeMultiplier = volume;
			component.PitchMultiplier = pitch;

			return entity.GetUUID();
		}

#pragma endregion

#pragma region AudioCommandID

		uint32_t AudioCommandID_Constructor(MonoString* inCommandName)
		{
			std::string commandName = ScriptUtils::MonoStringToUTF8(inCommandName);
			return Audio::CommandID(commandName.c_str());
		}

#pragma endregion

#pragma region AudioParameters
		//============================================================================================
		/// Audio Parameters Interface
		void Audio_SetParameterFloat(Audio::CommandID parameterID, uint64_t objectID, float value)
		{
			return AudioPlayback::SetParameterFloat(parameterID, objectID, value);
		}

		void Audio_SetParameterInt(Audio::CommandID parameterID, uint64_t objectID, int value)
		{
			return AudioPlayback::SetParameterInt(parameterID, objectID, value);
		}

		void Audio_SetParameterBool(Audio::CommandID parameterID, uint64_t objectID, bool value)
		{
			return AudioPlayback::SetParameterBool(parameterID, objectID, value);

		}

		void Audio_SetParameterFloatForAC(Audio::CommandID parameterID, uint64_t entityID, float value)
		{
			return AudioPlayback::SetParameterFloatForAC(parameterID, entityID, value);
		}

		void Audio_SetParameterIntForAC(Audio::CommandID parameterID, uint64_t entityID, int value)
		{
			return AudioPlayback::SetParameterIntForAC(parameterID, entityID, value);
		}

		void Audio_SetParameterBoolForAC(Audio::CommandID parameterID, uint64_t entityID, bool value)
		{
			return AudioPlayback::SetParameterBoolForAC(parameterID, entityID, value);
		}

		void Audio_SetParameterFloatForEvent(Audio::CommandID parameterID, uint32_t eventID, float value)
		{
			AudioPlayback::SetParameterFloat(parameterID, eventID, value);
		}

		void Audio_SetParameterIntForEvent(Audio::CommandID parameterID, uint32_t eventID, int value)
		{
			AudioPlayback::SetParameterInt(parameterID, eventID, value);
		}

		void Audio_SetParameterBoolForEvent(Audio::CommandID parameterID, uint32_t eventID, bool value)
		{
			AudioPlayback::SetParameterBool(parameterID, eventID, value);
		}

		//============================================================================================
		void Audio_PreloadEventSources(Audio::CommandID eventID)
		{
			AudioPlayback::PreloadEventSources(eventID);
		}

		void Audio_UnloadEventSources(Audio::CommandID eventID)
		{
			AudioPlayback::UnloadEventSources(eventID);
		}

		void Audio_SetLowPassFilterValue(uint64_t objectID, float value)
		{
			AudioPlayback::SetLowPassFilterValueObj(objectID, value);
		}

		void Audio_SetHighPassFilterValue(uint64_t objectID, float value)
		{
			AudioPlayback::SetHighPassFilterValueObj(objectID, value);
		}

		void Audio_SetLowPassFilterValue_Event(Audio::CommandID eventID, float value)
		{
			AudioPlayback::SetLowPassFilterValue(eventID, value);
		}

		void Audio_SetHighPassFilterValue_Event(Audio::CommandID eventID, float value)
		{
			AudioPlayback::SetHighPassFilterValue(eventID, value);
		}

		void Audio_SetLowPassFilterValue_AC(uint64_t entityID, float value)
		{
			AudioPlayback::SetLowPassFilterValueAC(entityID, value);
		}

		void Audio_SetHighPassFilterValue_AC(uint64_t entityID, float value)
		{
			AudioPlayback::SetHighPassFilterValueAC(entityID, value);
		}

#pragma endregion

#pragma region Texture2D

		bool Texture2D_Create(uint32_t width, uint32_t height, TextureWrap wrapMode, TextureFilter filterMode, AssetHandle* outHandle)
		{
			TextureSpecification spec;
			spec.Width = width;
			spec.Height = height;
			spec.SamplerWrap = wrapMode;
			spec.SamplerFilter = filterMode;

			auto result = Texture2D::Create(spec);
			*outHandle = AssetManager::AddMemoryOnlyAsset<Texture2D>(result);
			return true;
		}

		void Texture2D_GetSize(AssetHandle* inHandle, uint32_t* outWidth, uint32_t* outHeight)
		{
			Ref<Texture2D> instance = AssetManager::GetAsset<Texture2D>(*inHandle);
			if (!instance)
			{
				AS_CONSOLE_LOG_ERROR("Tried to get texture size using an invalid handle!");
				return;
			}

			*outWidth = instance->GetWidth();
			*outHeight = instance->GetHeight();
		}

		void Texture2D_SetData(AssetHandle* inHandle, MonoArray* inData)
		{
			Ref<Texture2D> instance = AssetManager::GetAsset<Texture2D>(*inHandle);

			if (!instance)
			{
				AS_CONSOLE_LOG_ERROR("Tried to set texture data in an invalid texture!");
				return;
			}

			uintptr_t count = mono_array_length(inData);
			uint32_t dataSize = (uint32_t)(count * sizeof(glm::vec4) / 4);

			instance->Lock();
			Buffer buffer = instance->GetWriteableBuffer();
			AS_CORE_ASSERT(dataSize <= buffer.Size);
			// Convert RGBA32F color to RGBA8
			uint8_t* pixels = (uint8_t*)buffer.Data;
			uint32_t index = 0;
			for (uint32_t i = 0; i < instance->GetWidth() * instance->GetHeight(); i++)
			{
				glm::vec4& value = mono_array_get(inData, glm::vec4, i);
				*pixels++ = (uint32_t)(value.x * 255.0f);
				*pixels++ = (uint32_t)(value.y * 255.0f);
				*pixels++ = (uint32_t)(value.z * 255.0f);
				*pixels++ = (uint32_t)(value.w * 255.0f);
			}

			instance->Unlock();
		}

		// TODO(Peter): Uncomment when Aspect can actually read texture data from the CPU or when image data is persistently stored in RAM
		/*MonoArray* Texture2D_GetData(AssetHandle* inHandle)
		{
			Ref<Texture2D> instance = AssetManager::GetAsset<Texture2D>(*inHandle);

			if (!instance)
			{
				AS_CONSOLE_LOG_ERROR("Tried to get texture data for an invalid texture!");
				return nullptr;
			}

			uint32_t width = instance->GetWidth();
			uint32_t height = instance->GetHeight();
			ManagedArray result = ManagedArray::Create<glm::vec4>(width * height);

			instance->Lock();
			Buffer buffer = instance->GetImage()->GetBuffer();
			uint8_t* pixels = (uint8_t*)buffer.Data;

			for (uint32_t i = 0; i < width * height; i++)
			{
				glm::vec4 value;
				value.r = (float)*pixels++ / 255.0f;
				value.g = (float)*pixels++ / 255.0f;
				value.b = (float)*pixels++ / 255.0f;
				value.a = (float)*pixels++ / 255.0f;

				result.Set(i, value);
			}

			instance->Unlock();
			return result;
		}

#pragma endregion

#pragma region Mesh

		bool Mesh_GetMaterialByIndex(AssetHandle* meshHandle, int index, AssetHandle* outHandle)
		{
			if (!AssetManager::IsAssetHandleValid(*meshHandle))
			{
				AS_CONSOLE_LOG_ERROR("Mesh.GetMaterialByIndex called on an invalid Mesh instance!");
				*outHandle = AssetHandle(0);
				return false;
			}

			Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(*meshHandle);
			if (!mesh || !mesh->IsValid())
			{
				AS_CONSOLE_LOG_ERROR("Mesh.GetMaterialByIndex called on an invalid Mesh instance!");
				*outHandle = AssetHandle(0);
				return false;
			}

			Ref<MaterialTable> materialTable = mesh->GetMaterials();
			if (materialTable == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("Mesh.GetMaterialByIndex - Mesh has no materials!");
				*outHandle = AssetHandle(0);
				return false;
			}

			if (materialTable->GetMaterialCount() == 0)
			{
				*outHandle = AssetHandle(0);
				return false;
			}

			if ((uint32_t)index >= materialTable->GetMaterialCount())
			{
				AS_CONSOLE_LOG_ERROR("Mesh.GetMaterialByIndex - Material index out of range. Index: {0}, MaxIndex: {1}", index, materialTable->GetMaterialCount() - 1);
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = materialTable->GetMaterial(index);
			return true;
		}

		int Mesh_GetMaterialCount(AssetHandle* meshHandle)
		{
			if (!AssetManager::IsAssetHandleValid(*meshHandle))
			{
				AS_CONSOLE_LOG_ERROR("Mesh.GetMaterialCount called on an invalid Mesh instance!");
				return 0;
			}

			Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(*meshHandle);
			if (!mesh || !mesh->IsValid())
			{
				AS_CONSOLE_LOG_ERROR("Mesh.GetMaterialCount called on an invalid Mesh instance!");
				return 0;
			}

			Ref<MaterialTable> materialTable = mesh->GetMaterials();
			if (materialTable == nullptr)
				return 0;

			return materialTable->GetMaterialCount();
		}

#pragma endregion

#pragma region StaticMesh

		bool StaticMesh_GetMaterialByIndex(AssetHandle* meshHandle, int index, AssetHandle* outHandle)
		{
			if (!AssetManager::IsAssetHandleValid(*meshHandle))
			{
				AS_CONSOLE_LOG_ERROR("StaticMesh.GetMaterialByIndex called on an invalid Mesh instance!");
				*outHandle = AssetHandle(0);
				return false;
			}

			Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(*meshHandle);
			if (!mesh || !mesh->IsValid())
			{
				AS_CONSOLE_LOG_ERROR("StaticMesh.GetMaterialByIndex called on an invalid Mesh instance!");
				*outHandle = AssetHandle(0);
				return false;
			}

			Ref<MaterialTable> materialTable = mesh->GetMaterials();
			if (materialTable == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("StaticMesh.GetMaterialByIndex - Mesh has no materials!");
				*outHandle = AssetHandle(0);
				return false;
			}

			if (materialTable->GetMaterialCount() == 0)
			{
				*outHandle = AssetHandle(0);
				return false;
			}

			if ((uint32_t)index >= materialTable->GetMaterialCount())
			{
				AS_CONSOLE_LOG_ERROR("StaticMesh.GetMaterialByIndex - Material index out of range. Index: {0}, MaxIndex: {1}", index, materialTable->GetMaterialCount() - 1);
				*outHandle = AssetHandle(0);
				return false;
			}

			*outHandle = materialTable->GetMaterial(index);
			return true;
		}

		int StaticMesh_GetMaterialCount(AssetHandle* meshHandle)
		{
			if (!AssetManager::IsAssetHandleValid(*meshHandle))
			{
				AS_CONSOLE_LOG_ERROR("StaticMesh.GetMaterialCount called on an invalid Mesh instance!");
				return 0;
			}

			Ref<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>(*meshHandle);
			if (!mesh || !mesh->IsValid())
			{
				AS_CONSOLE_LOG_ERROR("StaticMesh.GetMaterialCount called on an invalid Mesh instance!");
				return 0;
			}

			Ref<MaterialTable> materialTable = mesh->GetMaterials();
			if (materialTable == nullptr)
				return 0;

			return materialTable->GetMaterialCount();
		}

#pragma endregion

#pragma region Material

		static Ref<MaterialAsset> Material_GetMaterialAsset(const char* functionName, uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle)
		{
			if (!AssetManager::IsAssetHandleValid(*meshHandle))
			{
				// NOTE(Peter): This means the material is expected to be an actual asset, referenced directly
				Ref<MaterialAsset> material = AssetManager::GetAsset<MaterialAsset>(*materialHandle);

				if (!material)
				{
					AS_CONSOLE_LOG_ERROR("{} - Failed to get a material asset with handle {}, no such asset exists!", functionName, *materialHandle);
					return nullptr;
				}

				return material;
			}

			Ref<MaterialTable> materialTable = nullptr;
			if (entityID == 0)
			{
				const auto& meshMetadata = Project::GetEditorAssetManager()->GetMetadata(*meshHandle);

				if (meshMetadata.Type == AssetType::Mesh)
				{
					auto& mesh = AssetManager::GetAsset<Mesh>(*meshHandle);

					if (!mesh || !mesh->IsValid())
					{
						AS_CONSOLE_LOG_ERROR("{0} called with an invalid mesh instance!", functionName);
						return nullptr;
					}

					materialTable = mesh->GetMaterials();
				}
				else if (meshMetadata.Type == AssetType::StaticMesh)
				{
					auto& staticMesh = AssetManager::GetAsset<StaticMesh>(*meshHandle);

					if (!staticMesh || !staticMesh->IsValid())
					{
						AS_CONSOLE_LOG_ERROR("{0} called with an invalid mesh instance!", functionName);
						return nullptr;
					}

					materialTable = staticMesh->GetMaterials();
				}
				else
				{
					AS_CONSOLE_LOG_ERROR("{0} - meshHandle doesn't correspond with a Mesh? AssetType: {1}", functionName, Aspect::Utils::AssetTypeToString(meshMetadata.Type));
					return nullptr;
				}
			}
			else
			{
				// This material is expected to be on a component
				auto entity = GetEntity(entityID);

				if (!entity)
				{
					AS_CONSOLE_LOG_ERROR("{0} - Invalid entity!", functionName);
					return nullptr;
				}

				if (entity.HasComponent<MeshComponent>())
				{
					materialTable = entity.GetComponent<MeshComponent>().MaterialTable;
				}
				else if (entity.HasComponent<StaticMeshComponent>())
				{
					materialTable = entity.GetComponent<StaticMeshComponent>().MaterialTable;
				}
				else
				{
					AS_CONSOLE_LOG_ERROR("{0} - Invalid component!");
					return nullptr;
				}
			}

			if (materialTable == nullptr || materialTable->GetMaterialCount() == 0)
			{
				AS_CONSOLE_LOG_ERROR("{0} - Mesh has no materials!", functionName);
				return nullptr;
			}

			Ref<MaterialAsset> materialInstance = nullptr;

			for (const auto& [materialIndex, material] : materialTable->GetMaterials())
			{
				if (material == *materialHandle)
				{
					materialInstance = AssetManager::GetAsset<MaterialAsset>(material);
					break;
				}
			}

			if (materialInstance == nullptr)
			{
				AS_CONSOLE_LOG_ERROR("{0} - This appears to be an invalid Material!", functionName);
			}

			return materialInstance;
		}

		void Material_GetAlbedoColor(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, glm::vec3* outAlbedoColor)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.GetAlbedoColor", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
			{
				*outAlbedoColor = glm::vec3(1.0f, 0.0f, 1.0f);
				return;
			}

			*outAlbedoColor = materialInstance->GetAlbedoColor();
		}

		void Material_SetAlbedoColor(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, glm::vec3* inAlbedoColor)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.SetAlbedoColor", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			materialInstance->SetAlbedoColor(*inAlbedoColor);
		}

		float Material_GetMetalness(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.GetMetalness", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return 0.0f;

			return materialInstance->GetMetalness();
		}

		void Material_SetMetalness(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, float inMetalness)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.SetMetalness", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			materialInstance->SetMetalness(inMetalness);
		}

		float Material_GetRoughness(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.GetRoughness", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return 0.0f;

			return materialInstance->GetRoughness();
		}

		void Material_SetRoughness(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, float inRoughness)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.SetRoughness", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			materialInstance->SetRoughness(inRoughness);
		}

		float Material_GetEmission(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.GetEmission", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return 0.0f;

			return materialInstance->GetEmission();
		}

		void Material_SetEmission(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, float inEmission)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.SetEmission", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			materialInstance->SetEmission(inEmission);
		}

		void Material_SetFloat(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, MonoString* inUniform, float value)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.Set", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			std::string uniformName = ScriptUtils::MonoStringToUTF8(inUniform);
			if (uniformName.empty())
			{
				AS_CONSOLE_LOG_WARN("Material.Set - Cannot set uniform with empty name!");
				return;
			}

			materialInstance->GetMaterial()->Set(uniformName, value);
		}

		void Material_SetVector3(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, MonoString* inUniform, glm::vec3* inValue)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.Set", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			std::string uniformName = ScriptUtils::MonoStringToUTF8(inUniform);
			if (uniformName.empty())
			{
				AS_CONSOLE_LOG_WARN("Material.Set - Cannot set uniform with empty name!");
				return;
			}

			materialInstance->GetMaterial()->Set(uniformName, *inValue);
		}

		void Material_SetVector4(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, MonoString* inUniform, glm::vec3* inValue)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.Set", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			std::string uniformName = ScriptUtils::MonoStringToUTF8(inUniform);
			if (uniformName.empty())
			{
				AS_CONSOLE_LOG_WARN("Material.Set - Cannot set uniform with empty name!");
				return;
			}

			materialInstance->GetMaterial()->Set(uniformName, *inValue);
		}

		void Material_SetTexture(uint64_t entityID, AssetHandle* meshHandle, AssetHandle* materialHandle, MonoString* inUniform, AssetHandle* inTexture)
		{
			Ref<MaterialAsset> materialInstance = Material_GetMaterialAsset("Material.Set", entityID, meshHandle, materialHandle);

			if (materialInstance == nullptr)
				return;

			std::string uniformName = ScriptUtils::MonoStringToUTF8(inUniform);
			if (uniformName.empty())
			{
				AS_CONSOLE_LOG_WARN("Material.Set - Cannot set uniform with empty name!");
				return;
			}

			Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(*inTexture);

			if (!texture)
			{
				AS_CONSOLE_LOG_ERROR("Material.Set - Tried to set an invalid texture instance");
				return;
			}

			materialInstance->GetMaterial()->Set(uniformName, texture);
		}

#pragma endregion

#pragma region MeshFactory

		void* MeshFactory_CreatePlane(float width, float height)
		{
			// TODO: MeshFactory::CreatePlane(width, height, subdivisions)!
			return nullptr;
		}

#pragma endregion

#pragma region Physics

		bool Physics_Raycast(RaycastData* inRaycastData, ScriptRaycastHit* outHit)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.Raycast can only be called in Play mode!");
				return false;
			}

			AS_CORE_ASSERT(scene->GetPhysicsScene()->IsValid());

			RaycastHit tempHit;
			bool success = false;

			if (inRaycastData->ExcludeEntities)
			{
				size_t excludeEntitiesCount = mono_array_length(inRaycastData->ExcludeEntities);
				std::unordered_set<UUID> entityIDs(excludeEntitiesCount);
				for (size_t i = 0; i < excludeEntitiesCount; i++)
				{
					uint64_t entityID = mono_array_get(inRaycastData->ExcludeEntities, uint64_t, i);
					entityIDs.insert(entityID);
				}
				success = scene->GetPhysicsScene()->RaycastExcludeEntities(inRaycastData->Origin, inRaycastData->Direction, inRaycastData->MaxDistance, &tempHit, entityIDs);
			}
			else
			{
				success = scene->GetPhysicsScene()->Raycast(inRaycastData->Origin, inRaycastData->Direction, inRaycastData->MaxDistance, &tempHit);
			}

			if (success && inRaycastData->RequiredComponentTypes != nullptr)
			{
				Entity entity = scene->GetEntityWithUUID(tempHit.HitEntity);
				size_t requiredComponentsCount = mono_array_length(inRaycastData->RequiredComponentTypes);

				for (size_t i = 0; i < requiredComponentsCount; i++)
				{
					void* reflectionType = mono_array_get(inRaycastData->RequiredComponentTypes, void*, i);
					if (reflectionType == nullptr)
					{
						AS_CONSOLE_LOG_ERROR("Physics.Raycast - Why did you feel the need to pass a \"null\" as a required component?");
						success = false;
						break;
					}

					MonoType* componentType = mono_reflection_type_get_type((MonoReflectionType*)reflectionType);

#ifdef AS_DEBUG
					MonoClass* typeClass = mono_type_get_class(componentType);
					MonoClass* parentClass = mono_class_get_parent(typeClass);

					bool validComponentFilter = parentClass != nullptr;
					if (validComponentFilter)
					{
						const char* parentClassName = mono_class_get_name(parentClass);
						const char* parentNameSpace = mono_class_get_namespace(parentClass);
						validComponentFilter = strstr(parentClassName, "Component") != nullptr && strstr(parentNameSpace, "Aspect") != nullptr;
					}

					if (!validComponentFilter)
					{
						AS_CONSOLE_LOG_ERROR("Physics.Raycast - {0} does not inherit from Aspect.Component!", mono_class_get_name(typeClass));
						success = false;
						break;
					}
#endif

					if (!s_HasComponentFuncs[componentType](entity))
					{
						success = false;
						break;
					}
				}
			}

			if (success)
			{
				outHit->HitEntity = tempHit.HitEntity;
				outHit->Position = tempHit.Position;
				outHit->Normal = tempHit.Normal;
				outHit->Distance = tempHit.Distance;

				if (tempHit.HitCollider)
				{
					switch (tempHit.HitCollider->GetType())
					{
						case ColliderType::Box:
						{
							Ref<BoxColliderShape> shape = tempHit.HitCollider.As<BoxColliderShape>();
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.BoxCollider", outHit->HitEntity, shape->IsTrigger(), shape->GetHalfSize(), shape->GetOffset());
							break;
						}
						case ColliderType::Sphere:
						{
							Ref<SphereColliderShape> shape = tempHit.HitCollider.As<SphereColliderShape>();
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.SphereCollider", outHit->HitEntity, shape->IsTrigger(), shape->GetRadius());
							break;
						}
						case ColliderType::Capsule:
						{
							Ref<CapsuleColliderShape> shape = tempHit.HitCollider.As<CapsuleColliderShape>();
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.CapsuleCollider", outHit->HitEntity, shape->IsTrigger(), shape->GetRadius(), shape->GetHeight());
							break;
						}
						case ColliderType::ConvexMesh:
						{
							Ref<ConvexMeshShape> shape = tempHit.HitCollider.As<ConvexMeshShape>();
							Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(shape->GetColliderHandle());
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.MeshCollider", outHit->HitEntity, shape->IsTrigger(), colliderAsset->ColliderMesh);
							break;
						}
						case ColliderType::TriangleMesh:
						{
							Ref<TriangleMeshShape> shape = tempHit.HitCollider.As<TriangleMeshShape>();
							Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(shape->GetColliderHandle());
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.MeshCollider", outHit->HitEntity, shape->IsTrigger(), colliderAsset->ColliderMesh);
							break;
						}
					}
				}
			}
			else
			{
				*outHit = ScriptRaycastHit();
			}

			return success;
		}

		bool Physics_SphereCast(SphereCastData* inSphereCastData, ScriptRaycastHit* outHit)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.Raycast can only be called in Play mode!");
				return false;
			}

			AS_CORE_ASSERT(scene->GetPhysicsScene()->IsValid());

			RaycastHit tempHit;
			bool success = false;

			if (inSphereCastData->ExcludeEntities)
			{
				size_t excludeEntitiesCount = mono_array_length(inSphereCastData->ExcludeEntities);
				std::unordered_set<UUID> entityIDs(excludeEntitiesCount);
				for (size_t i = 0; i < excludeEntitiesCount; i++)
				{
					uint64_t entityID = mono_array_get(inSphereCastData->ExcludeEntities, uint64_t, i);
					entityIDs.insert(entityID);
				}
				AS_CORE_VERIFY(false);
				success = scene->GetPhysicsScene()->RaycastExcludeEntities(inSphereCastData->Origin, inSphereCastData->Direction, inSphereCastData->MaxDistance, &tempHit, entityIDs);
			}
			else
			{
				success = scene->GetPhysicsScene()->SphereCast(inSphereCastData->Origin, inSphereCastData->Direction, inSphereCastData->Radius, inSphereCastData->MaxDistance, &tempHit);
			}

			if (success && inSphereCastData->RequiredComponentTypes != nullptr)
			{
				Entity entity = scene->GetEntityWithUUID(tempHit.HitEntity);
				size_t requiredComponentsCount = mono_array_length(inSphereCastData->RequiredComponentTypes);

				for (size_t i = 0; i < requiredComponentsCount; i++)
				{
					void* reflectionType = mono_array_get(inSphereCastData->RequiredComponentTypes, void*, i);
					if (reflectionType == nullptr)
					{
						AS_CONSOLE_LOG_ERROR("Physics.Raycast - Why did you feel the need to pass a \"null\" as a required component?");
						success = false;
						break;
					}

					MonoType* componentType = mono_reflection_type_get_type((MonoReflectionType*)reflectionType);

#ifdef AS_DEBUG
					MonoClass* typeClass = mono_type_get_class(componentType);
					MonoClass* parentClass = mono_class_get_parent(typeClass);

					bool validComponentFilter = parentClass != nullptr;
					if (validComponentFilter)
					{
						const char* parentClassName = mono_class_get_name(parentClass);
						const char* parentNameSpace = mono_class_get_namespace(parentClass);
						validComponentFilter = strstr(parentClassName, "Component") != nullptr && strstr(parentNameSpace, "Aspect") != nullptr;
					}

					if (!validComponentFilter)
					{
						AS_CONSOLE_LOG_ERROR("Physics.Raycast - {0} does not inherit from Aspect.Component!", mono_class_get_name(typeClass));
						success = false;
						break;
					}
#endif

					if (!s_HasComponentFuncs[componentType](entity))
					{
						success = false;
						break;
					}
				}
			}

			if (success)
			{
				outHit->HitEntity = tempHit.HitEntity;
				outHit->Position = tempHit.Position;
				outHit->Normal = tempHit.Normal;
				outHit->Distance = tempHit.Distance;

				if (tempHit.HitCollider)
				{
					switch (tempHit.HitCollider->GetType())
					{
						case ColliderType::Box:
						{
							Ref<BoxColliderShape> shape = tempHit.HitCollider.As<BoxColliderShape>();
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.BoxCollider", outHit->HitEntity, shape->IsTrigger(), shape->GetHalfSize(), shape->GetOffset());
							break;
						}
						case ColliderType::Sphere:
						{
							Ref<SphereColliderShape> shape = tempHit.HitCollider.As<SphereColliderShape>();
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.SphereCollider", outHit->HitEntity, shape->IsTrigger(), shape->GetRadius());
							break;
						}
						case ColliderType::Capsule:
						{
							Ref<CapsuleColliderShape> shape = tempHit.HitCollider.As<CapsuleColliderShape>();
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.CapsuleCollider", outHit->HitEntity, shape->IsTrigger(), shape->GetRadius(), shape->GetHeight());
							break;
						}
						case ColliderType::ConvexMesh:
						{
							Ref<ConvexMeshShape> shape = tempHit.HitCollider.As<ConvexMeshShape>();
							Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(shape->GetColliderHandle());
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.MeshCollider", outHit->HitEntity, shape->IsTrigger(), colliderAsset->ColliderMesh);
							break;
						}
						case ColliderType::TriangleMesh:
						{
							Ref<TriangleMeshShape> shape = tempHit.HitCollider.As<TriangleMeshShape>();
							Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(shape->GetColliderHandle());
							outHit->HitCollider = ScriptEngine::CreateManagedObject("Aspect.MeshCollider", outHit->HitEntity, shape->IsTrigger(), colliderAsset->ColliderMesh);
							break;
						}
					}
				}
			}
			else
			{
				*outHit = ScriptRaycastHit();
			}

			return success;
		}

		MonoArray* Physics_Raycast2D(RaycastData2D* inRaycastData)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.Raycast2D can only be called in Play mode!");
				return nullptr;
			}

			std::vector<Raycast2DResult> raycastResults = Physics2D::Raycast(scene, inRaycastData->Origin, inRaycastData->Origin + inRaycastData->Direction * inRaycastData->MaxDistance);

			MonoArray* result = ManagedArrayUtils::Create("Aspect.RaycastHit2D", raycastResults.size());
			for (size_t i = 0; i < raycastResults.size(); i++)
			{
				UUID entityID = raycastResults[i].HitEntity.GetUUID();
				MonoObject* entityObject = nullptr;

				GCHandle entityInstance = ScriptEngine::GetEntityInstance(entityID);
				if (entityInstance != nullptr)
					entityObject = GCManager::GetReferencedObject(entityInstance);
				else
					entityObject = ScriptEngine::CreateManagedObject("Aspect.Entity", entityID);

				MonoObject* raycastHit2D = ScriptEngine::CreateManagedObject("Aspect.RaycastHit2D", entityObject, raycastResults[i].Point, raycastResults[i].Normal, raycastResults[i].Distance);
				ManagedArrayUtils::SetValue(result, i, raycastHit2D);
			}

			return result;
		}

		static void AddCollidersToArray(MonoArray* arr, const std::array<OverlapHit, OVERLAP_MAX_COLLIDERS>& hits, uint32_t count)
		{
			uint32_t arrayIndex = 0;
			uintptr_t arrayLength = ManagedArrayUtils::Length(arr);

			for (uint32_t i = 0; i < count; i++)
			{
				Entity entity = hits[i].Actor->GetEntity();

				if (entity.HasComponent<BoxColliderComponent>() && arrayIndex < arrayLength)
				{
					auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
					MonoObject* instance = ScriptEngine::CreateManagedObject("Aspect.BoxCollider", entity.GetUUID(), boxCollider.IsTrigger, boxCollider.HalfSize, boxCollider.Offset);
					ManagedArrayUtils::SetValue(arr, arrayIndex++, instance);
				}

				if (entity.HasComponent<SphereColliderComponent>() && arrayIndex < arrayLength)
				{
					auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();
					MonoObject* instance = ScriptEngine::CreateManagedObject("Aspect.SphereCollider", entity.GetUUID(), sphereCollider.IsTrigger, sphereCollider.Radius);
					ManagedArrayUtils::SetValue(arr, arrayIndex++, instance);
				}

				if (entity.HasComponent<CapsuleColliderComponent>() && arrayIndex < arrayLength)
				{
					auto& capsuleCollider = entity.GetComponent<CapsuleColliderComponent>();
					MonoObject* instance = ScriptEngine::CreateManagedObject("Aspect.CapsuleCollider", entity.GetUUID(), capsuleCollider.IsTrigger, capsuleCollider.Radius, capsuleCollider.Height);
					ManagedArrayUtils::SetValue(arr, arrayIndex++, instance);
				}

				if (entity.HasComponent<MeshColliderComponent>() && arrayIndex < arrayLength)
				{
					auto& meshCollider = entity.GetComponent<MeshColliderComponent>();
					Ref<MeshColliderAsset> colliderAsset = AssetManager::GetAsset<MeshColliderAsset>(meshCollider.ColliderAsset);
					MonoObject* instance = ScriptEngine::CreateManagedObject("Aspect.MeshCollider", entity.GetUUID(), meshCollider.IsTrigger, colliderAsset->ColliderMesh);
					ManagedArrayUtils::SetValue(arr, arrayIndex++, instance);
				}
			}
		}

		static std::array<OverlapHit, OVERLAP_MAX_COLLIDERS> s_OverlapBuffer;
		MonoArray* Physics_OverlapBox(glm::vec3* inOrigin, glm::vec3* inHalfSize)
		{
			AS_PROFILE_FUNC();
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.OverlapBox can only be called in Play mode!");
				return nullptr;
			}

			AS_CORE_ASSERT(scene->GetPhysicsScene()->IsValid());

			memset(s_OverlapBuffer.data(), 0, OVERLAP_MAX_COLLIDERS * sizeof(OverlapHit));

			MonoArray* outColliders = nullptr;

			uint32_t count;
			if (scene->GetPhysicsScene()->OverlapBox(*inOrigin, *inHalfSize, s_OverlapBuffer, count))
			{
				outColliders = ManagedArrayUtils::Create("Aspect.Collider", count);
				AddCollidersToArray(outColliders, s_OverlapBuffer, count);
			}

			return outColliders;
		}

		MonoArray* Physics_OverlapCapsule(glm::vec3* inOrigin, float radius, float halfHeight)
		{
			AS_PROFILE_FUNC();
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.OverlapCapsule can only be called in Play mode!");
				return nullptr;
			}

			MonoArray* outColliders = nullptr;
			memset(s_OverlapBuffer.data(), 0, OVERLAP_MAX_COLLIDERS * sizeof(OverlapHit));

			uint32_t count;
			if (scene->GetPhysicsScene()->OverlapCapsule(*inOrigin, radius, halfHeight, s_OverlapBuffer, count))
			{
				outColliders = ManagedArrayUtils::Create("Aspect.Collider", count);
				AddCollidersToArray(outColliders, s_OverlapBuffer, count);
			}

			return outColliders;
		}

		MonoArray* Physics_OverlapSphere(glm::vec3* inOrigin, float radius)
		{
			AS_PROFILE_FUNC();
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.OverlapSphere can only be called in Play mode!");
				return nullptr;
			}

			MonoArray* outColliders = nullptr;
			memset(s_OverlapBuffer.data(), 0, OVERLAP_MAX_COLLIDERS * sizeof(OverlapHit));

			uint32_t count;
			if (scene->GetPhysicsScene()->OverlapSphere(*inOrigin, radius, s_OverlapBuffer, count))
			{
				outColliders = ManagedArrayUtils::Create("Aspect.Collider", count);
				AddCollidersToArray(outColliders, s_OverlapBuffer, count);
			}

			return outColliders;
		}

		int Physics_OverlapBoxNonAlloc(glm::vec3* inOrigin, glm::vec3* inHalfSize, MonoArray* outColliders)
		{
			AS_PROFILE_FUNC();
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.OverlapBoxNonAlloc can only be called in Play mode!");
				return 0;
			}

			memset(s_OverlapBuffer.data(), 0, OVERLAP_MAX_COLLIDERS * sizeof(OverlapHit));

			uint32_t count;
			if (scene->GetPhysicsScene()->OverlapBox(*inOrigin, *inHalfSize, s_OverlapBuffer, count))
			{
				count = count > (uint32_t)ManagedArrayUtils::Length(outColliders) ? (uint32_t)ManagedArrayUtils::Length(outColliders) : count;
				AddCollidersToArray(outColliders, s_OverlapBuffer, count);
			}

			return count;
		}

		int Physics_OverlapCapsuleNonAlloc(glm::vec3* inOrigin, float radius, float halfHeight, MonoArray* outColliders)
		{
			AS_PROFILE_FUNC();
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.OverlapCapsuleNonAlloc can only be called in Play mode!");
				return 0;
			}

			memset(s_OverlapBuffer.data(), 0, OVERLAP_MAX_COLLIDERS * sizeof(OverlapHit));

			uint32_t count;
			if (scene->GetPhysicsScene()->OverlapCapsule(*inOrigin, radius, halfHeight, s_OverlapBuffer, count))
			{
				count = count > (uint32_t)ManagedArrayUtils::Length(outColliders) ? (uint32_t)ManagedArrayUtils::Length(outColliders) : count;
				AddCollidersToArray(outColliders, s_OverlapBuffer, count);
			}

			return count;
		}

		int Physics_OverlapSphereNonAlloc(glm::vec3* inOrigin, float radius, MonoArray* outColliders)
		{
			AS_PROFILE_FUNC();
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");

			if (scene->IsEditorScene())
			{
				AS_THROW_INVALID_OPERATION("Physics.OverlapSphereNonAlloc can only be called in Play mode!");
				return 0;
			}

			memset(s_OverlapBuffer.data(), 0, OVERLAP_MAX_COLLIDERS * sizeof(OverlapHit));

			uint32_t count;
			if (scene->GetPhysicsScene()->OverlapSphere(*inOrigin, radius, s_OverlapBuffer, count))
			{
				count = count > (uint32_t)ManagedArrayUtils::Length(outColliders) ? (uint32_t)ManagedArrayUtils::Length(outColliders) : count;
				AddCollidersToArray(outColliders, s_OverlapBuffer, count);
			}

			return count;
		}

		void Physics_GetGravity(glm::vec3* outGravity)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			*outGravity = scene->GetPhysicsScene()->GetGravity();
		}

		void Physics_SetGravity(glm::vec3* inGravity)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			scene->GetPhysicsScene()->SetGravity(*inGravity);
		}

		void Physics_AddRadialImpulse(glm::vec3* inOrigin, float radius, float strength, EFalloffMode falloff, bool velocityChange)
		{
			Ref<Scene> scene = ScriptEngine::GetSceneContext();
			AS_CORE_ASSERT(scene, "No active scene!");
			scene->GetPhysicsScene()->AddRadialImpulse(*inOrigin, radius, strength, falloff, velocityChange);
		}

#pragma endregion

#pragma region Matrix4
		void Matrix4_LookAt(glm::vec3* eye, glm::vec3* center, glm::vec3* up, glm::mat4* outMatrix)
		{
			*outMatrix = glm::lookAt(*eye, *center, *up);
		}

#pragma endregion

#pragma region Noise
		
		Noise* Noise_Constructor(int seed)
		{
			return hnew Noise(seed);
		}

		void Noise_Destructor(Noise* _this)
		{
			hdelete _this;
		}

		float Noise_GetFrequency(Noise* _this) { return _this->GetFrequency(); }
		void Noise_SetFrequency(Noise* _this, float frequency) { _this->SetFrequency(frequency); }

		int Noise_GetFractalOctaves(Noise* _this) { return _this->GetFractalOctaves(); }
		void Noise_SetFractalOctaves(Noise* _this, int octaves) { _this->SetFractalOctaves(octaves); }

		float Noise_GetFractalLacunarity(Noise* _this) { return _this->GetFractalLacunarity(); }
		void Noise_SetFractalLacunarity(Noise* _this, float lacunarity) { _this->SetFractalLacunarity(lacunarity); }

		float Noise_GetFractalGain(Noise* _this) { return _this->GetFractalGain(); }
		void Noise_SetFractalGain(Noise* _this, float gain) { _this->SetFractalGain(gain); }

		float Noise_Get(Noise* _this, float x, float y) { return _this->Get(x, y); }

		void Noise_SetSeed(int seed) { Noise::SetSeed(seed); }
		float Noise_Perlin(float x, float y) { return Noise::PerlinNoise(x, y); }

#pragma endregion
		*/
#pragma region Log

		void Log_LogMessage(LogLevel level, MonoString* inFormattedMessage)
		{
			AS_PROFILE_FUNC();

			std::string message = ScriptUtils::MonoStringToUTF8(inFormattedMessage);
			switch (level)
			{
				case LogLevel::Trace:
					AS_CONSOLE_LOG_TRACE(message);
					break;
				case LogLevel::Debug:
					AS_CONSOLE_LOG_INFO(message);
					break;
				case LogLevel::Info:
					AS_CONSOLE_LOG_INFO(message);
					break;
				case LogLevel::Warn:
					AS_CONSOLE_LOG_WARN(message);
					break;
				case LogLevel::Error:
					AS_CONSOLE_LOG_ERROR(message);
					break;
				case LogLevel::Critical:
					AS_CONSOLE_LOG_FATAL(message);
					break;
			}
		}

#pragma endregion
		/*
#pragma region Input

		bool Input_IsKeyPressed(KeyCode keycode) { return Input::IsKeyPressed(keycode); }
		bool Input_IsKeyHeld(KeyCode keycode) { return Input::IsKeyHeld(keycode); }
		bool Input_IsKeyDown(KeyCode keycode) { return Input::IsKeyDown(keycode); }
		bool Input_IsKeyReleased(KeyCode keycode) { return Input::IsKeyReleased(keycode); }

		bool Input_IsMouseButtonPressed(Key::MouseButton button)
		{
			bool isPressed = Input::IsMouseButtonPressed((int)button);//TODO: delete int

			bool enableImGui = Application::Get().GetSpecification().EnableImGui;
			if (isPressed && enableImGui && GImGui->HoveredWindow != nullptr)
			{
				// Make sure we're in the viewport panel
				ImGuiWindow* viewportWindow = ImGui::FindWindowByName("Viewport");
				if (viewportWindow != nullptr)
					isPressed = GImGui->HoveredWindow->ID == viewportWindow->ID;
			}

			return isPressed;
		}
		bool Input_IsMouseButtonHeld(Key::MouseButton button)
		{
			bool isHeld = Input::IsMouseButtonHeld(button);

			bool enableImGui = Application::Get().GetSpecification().EnableImGui;
			if (isHeld && enableImGui && GImGui->HoveredWindow != nullptr)
			{
				// Make sure we're in the viewport panel
				ImGuiWindow* viewportWindow = ImGui::FindWindowByName("Viewport");
				if (viewportWindow != nullptr)
					isHeld = GImGui->HoveredWindow->ID == viewportWindow->ID;
			}

			return isHeld;
		}
		bool Input_IsMouseButtonDown(Key::MouseButton button)
		{
			bool isDown = Input::IsMouseButtonDown(button);

			bool enableImGui = Application::Get().GetSpecification().EnableImGui;
			if (isDown && enableImGui && GImGui->HoveredWindow != nullptr)
			{
				// Make sure we're in the viewport panel
				ImGuiWindow* viewportWindow = ImGui::FindWindowByName("Viewport");
				if (viewportWindow != nullptr)
					isDown = GImGui->HoveredWindow->ID == viewportWindow->ID;
			}

			return isDown;
		}
		bool Input_IsMouseButtonReleased(Key::MouseButton button)
		{
			bool released = Input::IsMouseButtonReleased(button);

			bool enableImGui = Application::Get().GetSpecification().EnableImGui;
			if (released && enableImGui && GImGui->HoveredWindow != nullptr)
			{
				// Make sure we're in the viewport panel
				ImGuiWindow* viewportWindow = ImGui::FindWindowByName("Viewport");
				if (viewportWindow != nullptr)
					released = GImGui->HoveredWindow->ID == viewportWindow->ID;
			}

			return released;
		}

		void Input_GetMousePosition(glm::vec2* outPosition)
		{
			auto [x, y] = Input::GetMousePosition();
			*outPosition = { x, y };
		}
		
		void Input_SetCursorMode(Key::CursorMode mode) { Input::SetCursorMode(mode); }
		Key::CursorMode Input_GetCursorMode() { return Input::GetCursorMode(); }
		bool Input_IsControllerPresent(int id) { return Input::IsControllerPresent(id); }

		MonoArray* Input_GetConnectedControllerIDs()
		{
			return ManagedArrayUtils::FromVector<int32_t>(Input::GetConnectedControllerIDs());
		}

		MonoString* Input_GetControllerName(int id)
		{
			auto name = Input::GetControllerName(id);
			if (name.empty())
				return ScriptUtils::EmptyMonoString();
			return ScriptUtils::UTF8StringToMono(&name.front());
		}

		bool Input_IsControllerButtonPressed(int id, int button) { return Input::IsControllerButtonPressed(id, button); }
		bool Input_IsControllerButtonHeld(int id, int button) { return Input::IsControllerButtonHeld(id, button); }
		bool Input_IsControllerButtonDown(int id, int button) { return Input::IsControllerButtonDown(id, button); }
		bool Input_IsControllerButtonReleased(int id, int button) { return Input::IsControllerButtonReleased(id, button); }


		float Input_GetControllerAxis(int id, int axis) { return Input::GetControllerAxis(id, axis); }
		uint8_t Input_GetControllerHat(int id, int hat) { return Input::GetControllerHat(id, hat); }

		float Input_GetControllerDeadzone(int id, int axis) { return Input::GetControllerDeadzone(id, axis); }
		void Input_SetControllerDeadzone(int id, int axis, float deadzone) { return Input::SetControllerDeadzone(id, axis, deadzone); }
		*/
#pragma endregion
	/*
#pragma region EditorUI

#ifndef AS_DIST

		void EditorUI_Text(MonoString* inText)
		{
			std::string text = ScriptUtils::MonoStringToUTF8(inText);
			ImGui::TextUnformatted(text.c_str());
		}

		bool EditorUI_Button(MonoString* inLabel, glm::vec2* inSize)
		{
			std::string label = ScriptUtils::MonoStringToUTF8(inLabel);
			return ImGui::Button(label.c_str(), *((const ImVec2*)inSize));
		}

		bool EditorUI_BeginPropertyHeader(MonoString* label, bool openByDefault)
		{
			return UI::PropertyGridHeader(ScriptUtils::MonoStringToUTF8(label), openByDefault);
		}

		void EditorUI_EndPropertyHeader()
		{
			UI::EndTreeNode();
		}

		void EditorUI_PropertyGrid(bool inBegin)
		{
			if (inBegin)
				UI::BeginPropertyGrid();
			else
				UI::EndPropertyGrid();
		}

		bool EditorUI_PropertyFloat(MonoString* inLabel, float* outValue)
		{
			std::string label = ScriptUtils::MonoStringToUTF8(inLabel);
			return UI::Property(label.c_str(), *outValue);
		}

		bool EditorUI_PropertyVec2(MonoString* inLabel, glm::vec2* outValue)
		{
			std::string label = ScriptUtils::MonoStringToUTF8(inLabel);
			return UI::Property(label.c_str(), *outValue);
		}

		bool EditorUI_PropertyVec3(MonoString* inLabel, glm::vec3* outValue)
		{
			std::string label = ScriptUtils::MonoStringToUTF8(inLabel);
			return UI::Property(label.c_str(), *outValue);
		}

		bool EditorUI_PropertyVec4(MonoString* inLabel, glm::vec4* outValue)
		{
			std::string label = ScriptUtils::MonoStringToUTF8(inLabel);
			return UI::Property(label.c_str(), *outValue);
		}

#else
		void EditorUI_Text(MonoString* inText)
		{
		}

		bool EditorUI_Button(MonoString* inLabel, glm::vec2* inSize)
		{
			return false;
		}

		void EditorUI_PropertyGrid(bool inBegin)
		{
		}

		bool EditorUI_PropertyFloat(MonoString* inLabel, float* outValue)
		{
			return false;
		}

		bool EditorUI_PropertyVec2(MonoString* inLabel, glm::vec2* outValue)
		{
			return false;
		}

		bool EditorUI_PropertyVec3(MonoString* inLabel, glm::vec3* outValue)
		{
			return false;
		}

		bool EditorUI_PropertyVec4(MonoString* inLabel, glm::vec4* outValue)
		{
			return false;
		}
#endif
#pragma endregion

#pragma region SceneRenderer

		float SceneRenderer_GetOpacity()
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			return sceneRenderer->GetOpacity();
		}

		void SceneRenderer_SetOpacity(float opacity)
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			sceneRenderer->SetOpacity(opacity);
		}

		bool SceneRenderer_DepthOfField_IsEnabled()
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			return sceneRenderer->GetDOFSettings().Enabled;
		}

		void SceneRenderer_DepthOfField_SetEnabled(bool enabled)
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			sceneRenderer->GetDOFSettings().Enabled = enabled;
		}

		float SceneRenderer_DepthOfField_GetFocusDistance()
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			return sceneRenderer->GetDOFSettings().FocusDistance;
		}

		void SceneRenderer_DepthOfField_SetFocusDistance(float focusDistance)
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			sceneRenderer->GetDOFSettings().FocusDistance = focusDistance;
		}

		float SceneRenderer_DepthOfField_GetBlurSize()
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			return sceneRenderer->GetDOFSettings().BlurSize;
		}

		void SceneRenderer_DepthOfField_SetBlurSize(float blurSize)
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			AS_CORE_VERIFY(sceneRenderer);
			sceneRenderer->GetDOFSettings().BlurSize = blurSize;
		}

#pragma endregion

#pragma region DebugRenderer

		void DebugRenderer_DrawLine(glm::vec3* p0, glm::vec3* p1, glm::vec4* color)
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			Ref<DebugRenderer> debugRenderer = sceneRenderer->GetDebugRenderer();
			
			debugRenderer->DrawLine(*p0, *p1, *color);
		}

		void DebugRenderer_DrawQuadBillboard(glm::vec3* translation, glm::vec2* size, glm::vec4* color)
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			Ref<DebugRenderer> debugRenderer = sceneRenderer->GetDebugRenderer();

			debugRenderer->DrawQuadBillboard(*translation, *size, *color);
		}

		void DebugRenderer_SetLineWidth(float width)
		{
			Ref<SceneRenderer> sceneRenderer = ScriptEngine::GetSceneRenderer();
			Ref<DebugRenderer> debugRenderer = sceneRenderer->GetDebugRenderer();

			debugRenderer->SetLineWidth(width);
		}

#pragma endregion
		*/
	}

}
