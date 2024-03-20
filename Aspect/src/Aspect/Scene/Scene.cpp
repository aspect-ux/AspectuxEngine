#include "aspch.h"
#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "Aspect/Script/ScriptEngine.h"
#include "Aspect/Renderer/Renderer2D.h"
#include "Aspect/Physics/Physics2D.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Entity.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

//temp
#include "Aspect/Asset/ModeManager/ModeManager.h"
#include "Aspect/Asset/ConfigManager/ConfigManager.h"
#include "Aspect/Renderer/UniformBuffer.h"
#include "Aspect/Renderer/RenderCommand.h"
#include "Aspect/Renderer/FrameBuffer.h" // pointer to incomplete class type is not allowed  使用类中的变量指针报错，cpp只有给声明没有定义//参考https://www.zhihu.com/question/362445246
#include "Aspect/Library/UniformBufferLibrary.h"
#include "Aspect/Renderer/Renderer3D.h"
#include "Aspect/Debug/Profiler.h"
#include "Aspect/Events/SceneEvents.h"


//TODO: Physics
#include "Aspect/Math/HEMathUtils/MathUtils.h"//TODO:
#include "Aspect/Physics/BulletPhysics/3D/PhysicsSystem3D.h"



namespace Aspect {
	//TODO: To be removed
	Utils::BulletDrawer Scene::mDebugDrawer;

	// TODO: set scene to (*Scene) other than (Ref<>)
	std::unordered_map<UUID, AspectRef<Scene>> s_ActiveScenes;

	//temp
	static uint32_t id = 0;
	static uint32_t oldId = 0;
	EnvironmentHdrSettings Scene::environmentSettings;

	//TODO: from HEgine
	namespace Utils
	{
		static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
		{
			const auto inv = glm::inverse(projview);

			std::vector<glm::vec4> frustumCorners;
			for (unsigned int x = 0; x < 2; ++x)
			{
				for (unsigned int y = 0; y < 2; ++y)
				{
					for (unsigned int z = 0; z < 2; ++z)
					{
						const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
						frustumCorners.push_back(pt / pt.w);
					}
				}
			}

			return frustumCorners;
		}

		static glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, float cameraFOV, float aspect, const glm::vec3& lightDir, const glm::mat4& view)
		{
			const auto proj = glm::perspective(
				glm::radians(cameraFOV), aspect, nearPlane,
				farPlane);
			const auto corners = getFrustumCornersWorldSpace(proj * view);

			glm::vec3 center = glm::vec3(0, 0, 0);
			for (const auto& v : corners)
			{
				center += glm::vec3(v);
			}
			center /= corners.size();

			const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();
			for (const auto& v : corners)
			{
				const auto trf = lightView * v;
				minX = std::min(minX, trf.x);
				maxX = std::max(maxX, trf.x);
				minY = std::min(minY, trf.y);
				maxY = std::max(maxY, trf.y);
				minZ = std::min(minZ, trf.z);
				maxZ = std::max(maxZ, trf.z);
			}

			// Tune this parameter according to the scene
			constexpr float zMult = 10.0f;
			if (minZ < 0)
			{
				minZ *= zMult;
			}
			else
			{
				minZ /= zMult;
			}
			if (maxZ < 0)
			{
				maxZ /= zMult;
			}
			else
			{
				maxZ *= zMult;
			}

			const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

			return lightProjection * lightView;
		}

		static std::vector<glm::mat4> getLightSpaceMatrices(float cameraNearPlane, float cameraFarPlane, float cameraFOV, float aspect, const glm::vec3& lightDir, const glm::mat4& projview, const std::vector<float>& shadowCascadeLevels)
		{
			std::vector<glm::mat4> ret;
			for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
			{
				if (i == 0)
				{
					ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i], cameraFOV, aspect, lightDir, projview));
				}
				else if (i < shadowCascadeLevels.size())
				{
					ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i], cameraFOV, aspect, lightDir, projview));
				}
				else
				{
					ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane, cameraFOV, aspect, lightDir, projview));
				}
			}
			return ret;
		}
	}

	Scene::Scene()
	{
		// TODO:反序列化添加Components后，还要初始化场景自带Components(例如grid的MeshCompo)
		//Entity gridEntity = this->CreateEntity("SceneGrid");
		//gridEntity.AddComponent<MeshComponent>();
	}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}
	/*TODO:
	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}*/

	void Scene::ParentEntity(Entity entity, Entity parent)
	{
		AS_PROFILE_FUNC();

		if (parent.IsDescendantOf(entity))
		{
			UnparentEntity(parent);

			Entity newParent = TryGetEntityWithUUID(entity.GetParentUUID());
			if (newParent)
			{
				UnparentEntity(entity);
				ParentEntity(parent, newParent);
			}
		}
		else
		{
			Entity previousParent = TryGetEntityWithUUID(entity.GetParentUUID());

			if (previousParent)
				UnparentEntity(entity);
		}

		entity.SetParentUUID(parent.GetUUID());
		parent.Children().push_back(entity.GetUUID());

		ConvertToLocalSpace(entity);
	}

	void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
	{
		AS_PROFILE_FUNC();

		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
		if (!parent)
			return;

		auto& parentChildren = parent.Children();
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()), parentChildren.end());

		if (convertToWorldSpace)
			ConvertToWorldSpace(entity);

		entity.SetParentUUID(0);
	}

	AspectRef<Scene> Scene::Copy(AspectRef<Scene> other)
	{
		AspectRef<Scene> newScene = AspectRef<Scene>::Create();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	// Copy to runtime
	void Scene::CopyTo(AspectRef<Scene>& target)
	{
		AS_PROFILE_FUNC();

		/*TODO:
		// TODO(Yan): hack to prevent Box2D bodies from being created on copy via entt signals
		target->m_IsEditorScene = true;
		target->m_Name = m_Name;

		// Environment

		target->m_Light = m_Light;
		target->m_LightMultiplier = m_LightMultiplier;

		target->m_Environment = m_Environment;
		target->m_SkyboxLod = m_SkyboxLod;*/

		std::unordered_map<UUID, entt::entity> enttMap;
		auto idComponents = m_Registry.view<IDComponent>();
		for (auto entity : idComponents)
		{
			auto uuid = m_Registry.get<IDComponent>(entity).ID;
			auto name = m_Registry.get<TagComponent>(entity).Tag;
			Entity e = target->CreateEntityWithID(uuid, name);
			enttMap[uuid] = e.m_EntityHandle;
		}

		//CopyComponent<PrefabComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<TagComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<TransformComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<RelationshipComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<MeshComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<StaticMeshComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<AnimationComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<DirectionalLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<PointLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SpotLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SkyLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<ScriptComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<CameraComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SpriteRendererComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<TextComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<RigidBody2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<BoxCollider2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<CircleCollider2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<RigidBodyComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<Rigidbody3DComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<CharacterControllerComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<FixedJointComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<BoxColliderComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SphereColliderComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<CapsuleColliderComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<MeshColliderComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<AudioComponent>(target->m_Registry, m_Registry, enttMap);
		//CopyComponent<AudioListenerComponent>(target->m_Registry, m_Registry, enttMap);

		//target->SetPhysics2DGravity(GetPhysics2DGravity());

		// Sort IdComponent by by entity handle (which is essentially the order in which they were created)
		// This ensures a consistent ordering when iterating IdComponent (for example: when rendering scene hierarchy panel)
		target->SortEntities();

		target->m_ViewportWidth = m_ViewportWidth;
		target->m_ViewportHeight = m_ViewportHeight;

		//target->m_IsEditorScene = false;
	}

	AspectRef<Scene> Scene::GetScene(UUID uuid)
	{
		if (s_ActiveScenes.find(uuid) != s_ActiveScenes.end())
			return s_ActiveScenes.at(uuid);//TODO: to be fixed, there are just AspectRefs in `script` only

		return {};
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;

		return entity;
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name, bool shouldSort)
	{
		AS_PROFILE_FUNC();

		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = uuid;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();

		AS_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end());
		m_EntityIDMap[uuid] = entity;

		if (shouldSort)
			SortEntities();

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		// TODO: Destroy Script Entity
		if (entity.HasComponent<ScriptComponent>())
			ScriptEngine::ShutdownScriptEntity(entity, ModeManager::IsEditState() == true);

		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		m_IsPlaying = true;
		//m_ShouldSimulate = true;
		AspectRef<Scene> _this = this;
		Application::Get().DispatchEvent<ScenePreStartEvent, true>(_this);

		OnPhysics2DStart();
		//TODO:
		OnPhysics3DStart();

		// Scripting
		{

			//ScriptEngine::OnRuntimeStart(this);
			// Instantiate all script entities
			//auto view = m_Registry.view<ScriptComponent>();
			//for (auto e : view)
			//{
				//Entity entity = { e, this };
				//[Obsolete] ScriptEngine::OnCreateEntity(entity);
			//}
			ScriptEngine::InitializeRuntime();
		}

		Application::Get().DispatchEvent<ScenePostStartEvent, true>(_this);
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;
		m_IsPlaying = false;

		ScriptEngine::ShutdownRuntime();

		OnPhysics2DStop();

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Update scripts[Obsolete]
			{
				// C# Entity OnUpdate
				/*auto view = m_Registry.view<ScriptComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					ScriptEngine::OnUpdateEntity(entity, ts);
				}*/

				m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
					{
						// TODO: Move to Scene::OnScenePlay
						if (!nsc.Instance)
						{
							nsc.Instance = nsc.InstantiateScript();
							nsc.Instance->m_Entity = Entity{ entity, this };
							nsc.Instance->OnCreate();
						}

						nsc.Instance->OnUpdate(ts);
					});
			}

			{
				if (m_IsPlaying)
				{
					AS_PROFILE_FUNC("Scene::OnUpdate - C# OnUpdate");

					for (const auto& [entityID, entityInstance] : ScriptEngine::GetEntityInstances())
					{
						if (m_EntityIDMap.find(entityID) != m_EntityIDMap.end())
						{
							Entity entity = m_EntityIDMap.at(entityID);
							AS_CORE_WARN("NAME:{0}", entity.GetUUID());
							if (ScriptEngine::IsEntityInstantiated(entity))
								ScriptEngine::CallMethod<float>(entityInstance, "OnUpdate", ts);
						}

					}

					for (auto&& fn : m_PostUpdateQueue)
						fn();
					m_PostUpdateQueue.clear();
				}
			}

			// Physics 2D
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve transform from Box2D
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;

					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
				}
			}
		}

		//TODO: to update physics3d
		OnPhysics3DRuntime(ts);
	}
	void Scene::OnRenderRuntime(Timestep ts, EditorCamera& camera)
	{
		// 1. Skybox in Runtime
		RenderEnvironment(camera);

		// 2. Render 2D in Runtime(比editor多个相机调整,3d as well)
		{
			Camera* mainCamera = nullptr;
			glm::mat4 cameraTransform;
			{
				auto view = m_Registry.view<TransformComponent, CameraComponent>();
				for (auto entity : view)
				{
					auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

					if (camera.Primary)
					{
						mainCamera = &camera.Camera;
						cameraTransform = transform.GetTransform();
						break;
					}
				}
			}

			if (mainCamera)
			{
				Renderer2D::BeginScene(*mainCamera, cameraTransform);

				// Draw sprites
				{
					auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
					for (auto entity : group)
					{
						auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

						Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
					}
				}

				// Draw circles
				{
					auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
					for (auto entity : view)
					{
						auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

						Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
				}

				// Draw text
				{
					auto view = m_Registry.view<TransformComponent, TextComponent>();
					for (auto entity : view)
					{
						auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

						Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
					}
				}

				Renderer2D::EndScene();
			}
		}


		// 3. Render 3D in Runtime
		{
			Camera* mainCamera = nullptr;
			glm::mat4 cameraTransform;
			glm::vec3 cameraPos;
			{
				auto view = m_Registry.view<TransformComponent, CameraComponent>();
				for (auto entity : view)
				{
					auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

					if (camera.Primary)
					{
						mainCamera = &camera.Camera;
						cameraTransform = transform.GetTransform();
						cameraPos = transform.Translation;
						break;
					}
				}
			}

			if (mainCamera)
			{
				Renderer3D::BeginScene(*mainCamera, cameraTransform);

				// Get the Light 
				{

				}

				auto view = m_Registry.view<TransformComponent, MeshComponent>();
				for (auto entity : view)
				{
					auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);
					//AS_CORE_WARN(mesh.Path);
					Renderer3D::DrawModel(transform.GetTransform(), cameraPos, mesh, (int)entity);
				}

				Renderer3D::EndScene();
			}
		}
	}


	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		// update animation

		//TODO: update editor physics3d
		OnPhysics3DEditor(camera);
	}
	void Scene::OnRenderEditor(Timestep ts, EditorCamera& camera)
	{
		// 1. Render skybox etc. in editor
		// just mode changing
		switch (ModeManager::mSceneMode)
		{
		case SceneMode::None:
			break;
		case SceneMode::EnvironmentHdr:
			DrawEnvironmentHdr(camera.GetViewMatrix(), camera.GetProjection());
			break;
		case SceneMode::SkyBox:
			DrawSkyBox(camera.GetViewMatrix(), camera.GetProjection());
			break;
		default:
			break;
		}

		// 2. Render 3D
		Renderer3D::BeginScene(camera);

		Ref<Shader> defaultShader = Library<Shader>::GetInstance().GetDefaultShader();
		if (ModeManager::bHdrUse)
			defaultShader->SetFloat("exposure", environmentSettings.exposure);
		else
			defaultShader->SetFloat("exposure", 1.0f);

		// directional light reset
		defaultShader->SetInt("cascadeCount", -2);
		//for (size_t i = 0; i < 4; ++i)
		//{
		//	defaultShader->SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]", 1);
		//}
		defaultShader->SetFloat3("lightDir", glm::vec3(0.0f));

		// Lights
		{
			m_LightEnvironment = LightEnvironment();
			//Directional Lights
			{
				auto dirLights = m_Registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
				uint32_t directionalLightIndex = 0;

				// Set Shader
				defaultShader->Bind();
				defaultShader->SetInt("shadowMap", 8);
				Renderer3D::lightFBO->UnBindDepthTex3D(8);
				for (auto entity : dirLights)
				{
					if (directionalLightIndex >= LightEnvironment::MaxDirectionalLights)
						break;

					auto [transformComponent, lightComponent] = dirLights.get<TransformComponent, DirectionalLightComponent>(entity);
					glm::vec3 direction = -glm::normalize(glm::mat3(transformComponent.GetTransform()) * glm::vec3(1.0f));
					m_LightEnvironment.DirectionalLights[directionalLightIndex++] =
					{
						direction,
						//lightComponent.Radiance,
						lightComponent.Intensity
						//lightComponent.ShadowAmount,
						//lightComponent.CastShadows
					};

					//=============From Hegine: Set Base Shader Per Directional light===============
					float cameraNearPlane = camera.GetNearPlane();
					float cameraFarPlane = camera.GetFarPlane();
					std::vector<float> shadowCascadeLevels{ cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f };

					glm::vec3 lightDir = glm::normalize(glm::eulerAngles(glm::quat(transformComponent.Rotation)));
					//TODO:glm::vec3 direction = -glm::normalize(glm::mat3(transformComponent.GetTransform()) * glm::vec3(1.0f));
					//lightDir = direction;

					const auto lightMatrices = Utils::getLightSpaceMatrices(cameraNearPlane, cameraFarPlane, camera.GetFOV(),
						camera.GetAspect(), lightDir, camera.GetViewMatrix(), shadowCascadeLevels);
					Ref<UniformBuffer> lightMatricesUBO = Library<UniformBuffer>::GetInstance().Get("LightMatricesUniform");
					for (size_t i = 0; i < lightMatrices.size(); i++)
					{
						lightMatricesUBO->SetData(&lightMatrices[i], sizeof(glm::mat4x4), i * sizeof(glm::mat4x4));
					}
					defaultShader->SetMat4("view", camera.GetViewMatrix());
					defaultShader->SetFloat3("lightDir", lightDir);
					defaultShader->SetFloat("farPlane", cameraFarPlane);
					defaultShader->SetInt("cascadeCount", shadowCascadeLevels.size());
					defaultShader->SetFloat("dirLightIntensity", lightComponent.Intensity);

					for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
					{
						defaultShader->SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
					}
					Renderer3D::lightFBO->BindDepthTex3D(8);
					//=======================================
				}
			}

			// Point Lights
			/* {
				auto pointLights = m_Registry.group<PointLightComponent>(entt::get<TransformComponent>);
				m_LightEnvironment.PointLights.resize(pointLights.size());
				uint32_t pointLightIndex = 0;
				for (auto entity : pointLights)
				{
					auto [transformComponent, lightComponent] = pointLights.get<TransformComponent, PointLightComponent>(entity);
					auto transform = GetWorldSpaceTransform(Entity(entity, this));
					m_LightEnvironment.PointLights[pointLightIndex++] = {
						transform.Translation,
						lightComponent.Intensity,
						//lightComponent.Radiance,
						//lightComponent.MinRadius,
						//lightComponent.Radius,
						//lightComponent.Falloff,
						//lightComponent.LightSize,
						//lightComponent.CastsShadows,
					};

				}
			}*/
			/*
			// Spot Lights
			{
				auto spotLights = m_Registry.group<SpotLightComponent>(entt::get<TransformComponent>);
				m_LightEnvironment.SpotLights.resize(spotLights.size());
				uint32_t spotLightIndex = 0;
				for (auto e : spotLights)
				{
					Entity entity(e, this);
					auto [transformComponent, lightComponent] = spotLights.get<TransformComponent, SpotLightComponent>(e);
					auto transform = entity.HasComponent<RigidBodyComponent>() ? entity.Transform() : GetWorldSpaceTransform(entity);
					glm::vec3 direction = glm::normalize(glm::rotate(transform.GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f)));

					m_LightEnvironment.SpotLights[spotLightIndex++] = {
						transform.Translation,
						lightComponent.Intensity,
						direction,
						lightComponent.AngleAttenuation,
						lightComponent.Radiance,
						lightComponent.Range,
						lightComponent.Angle,
						lightComponent.Falloff,
						lightComponent.SoftShadows,
						{},
						lightComponent.CastsShadows
					};
				}
			}*/

			//==============================From Hegine=============================
			// Directional light
			/* {
				auto view = m_Registry.view<TransformComponent, DirectionalLightComponent>();

				defaultShader->Bind();
				defaultShader->SetInt("shadowMap", 8);
				Renderer3D::lightFBO->UnBindDepthTex3D(8);

				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& directionalLight = entity.GetComponent<DirectionalLightComponent>();

					float cameraNearPlane = camera.GetNearPlane();
					float cameraFarPlane = camera.GetFarPlane();
					std::vector<float> shadowCascadeLevels{ cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f };

					glm::vec3 lightDir = glm::normalize(glm::eulerAngles(glm::quat(transform.Rotation)));
					glm::vec3 direction = -glm::normalize(glm::mat3(transform.GetTransform()) * glm::vec3(1.0f));
					lightDir = direction;

					const auto lightMatrices = Utils::getLightSpaceMatrices(cameraNearPlane, cameraFarPlane, camera.GetFOV(),
						camera.GetAspect(), lightDir, camera.GetViewMatrix(), shadowCascadeLevels);
					Ref<UniformBuffer> lightMatricesUBO = Library<UniformBuffer>::GetInstance().Get("LightMatricesUniform");
					for (size_t i = 0; i < lightMatrices.size(); i++)
					{
						lightMatricesUBO->SetData(&lightMatrices[i], sizeof(glm::mat4x4), i * sizeof(glm::mat4x4));
					}
					defaultShader->SetMat4("view", camera.GetViewMatrix());
					defaultShader->SetFloat3("lightDir", lightDir);
					defaultShader->SetFloat("farPlane", cameraFarPlane);
					defaultShader->SetInt("cascadeCount", shadowCascadeLevels.size());
					defaultShader->SetFloat("dirLightIntensity", directionalLight.Intensity);

					for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
					{
						defaultShader->SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
					}
					Renderer3D::lightFBO->BindDepthTex3D(8);

					break; // now we only support one directional light
				}
			}*/

			// Point Light 
			{
				auto view = m_Registry.view<TransformComponent, PointLightComponent>();
				int i = 0;
				for (auto entity : view)
				{
					auto [transform, light] = view.get<TransformComponent, PointLightComponent>(entity);

					glm::vec3 lightPos = transform.GetTranslation();
					float intensity = light.Intensity;
					glm::vec3 lightColor = light.LightColor;

					defaultShader->Bind();
					defaultShader->SetFloat3("lightPositions[" + std::to_string(i) + "]", lightPos);
					defaultShader->SetFloat3("lightColors[" + std::to_string(i) + "]", intensity * lightColor);

					i++;
				}
				if (i == 0)
				{
					for (size_t i = 0; i < 4; i++)
					{
						defaultShader->Bind();
						defaultShader->SetFloat3("lightColors[" + std::to_string(i) + "]", glm::vec3{ -1.0f });
					}
				}
			}

		}

		uint32_t mainFramebuffer = RenderCommand::GetDrawFrameBuffer();

		// Light Depth pass
		Renderer3D::lightFBO->Bind();

		RenderCommand::SetViewport(0, 0, Renderer3D::lightFBO->GetSpecification().Width, Renderer3D::lightFBO->GetSpecification().Height);
		RenderCommand::Clear();
		RenderCommand::CullFrontOrBack(true); // peter panning
		auto view = m_Registry.view<TransformComponent, MeshComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& mesh = entity.GetComponent<MeshComponent>();

			Ref<Shader> csmShader = Library<Shader>::GetInstance().Get("CSM_Depth");
			csmShader->Bind();
			if (mesh.m_Mesh->bPlayAnim)
				csmShader->SetBool("u_Animated", true);
			else
				csmShader->SetBool("u_Animated", false);

			mesh.m_Mesh->Draw(transform.GetTransform(), camera.GetPosition(), csmShader, (int)e);
		}
		RenderCommand::CullFrontOrBack(false);

		// Render pass
		RenderCommand::BindFrameBuffer(mainFramebuffer);
		RenderCommand::SetViewport(0, 0, ConfigManager::m_ViewportSize.x, ConfigManager::m_ViewportSize.y);

		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& mesh = entity.GetComponent<MeshComponent>();

			if ((int)e == ConfigManager::selectedEntity)
			{
				RenderCommand::SetStencilFunc(StencilFunc::ALWAYS, 1, 0xFF);
				RenderCommand::StencilMask(0xFF);
				Renderer3D::DrawModel(transform.GetTransform(), camera.GetPosition(), mesh, (int)e);

				RenderCommand::SetStencilFunc(StencilFunc::NOTEQUAL, 1, 0xFF);
				RenderCommand::StencilMask(0x00);
				if (mesh.m_Mesh->bPlayAnim)
					mesh.m_Mesh->Draw(transform.GetTransform(), camera.GetPosition(), Library<Shader>::GetInstance().Get("NormalOutline_anim"), (int)e);
				else
					mesh.m_Mesh->Draw(transform.GetTransform(), camera.GetPosition(), Library<Shader>::GetInstance().Get("NormalOutline"), (int)e);
				RenderCommand::StencilMask(0xFF);
				RenderCommand::SetStencilFunc(StencilFunc::ALWAYS, 0, 0xFF);
				RenderCommand::ClearStencil();
			}
			else
				Renderer3D::DrawModel(transform.GetTransform(), camera.GetPosition(), mesh, (int)e);
		}

		Renderer3D::EndScene();


		// 3. Render 2D
		RenderScene2D(camera);
	}

	void Scene::DrawGrid(const glm::vec3& cameraPosition, const glm::vec3& gridColor)
	{
		constexpr float gridSize = 5000.0f;

		// 1. set transformation
		glm::mat4 model = glm::translate(glm::mat4(1.0f), cameraPosition);// * glm::scale, glm::rotate and so on

		// 2. set material/shader and draw mesh
		//m_gridMaterial.Set("u_Color", p_color);
		//m_context.renderer->DrawModelWithSingleMaterial(*m_context.editorResources->GetModel("Plane"), m_gridMaterial, &model);
		Ref<Shader> gridShader = Library<Shader>::GetInstance().Get("Grid");
		gridShader->Bind();
		//gridShader->SetFloat3("u_Color", gridColor);

		// temporarily get meshcomp
		MeshComponent gridMesh("Resources//Test//Models//Box.obj");
		Renderer3D::DrawModel(model, cameraPosition, gridMesh, UUID());

		// 3. draw grid line
		/*
		Renderer2D::DrawLine(glm::vec3(-gridSize + cameraPosition.x, 0.0f, 0.0f), glm::vec3(gridSize + cameraPosition.x, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f,1.0f), 1.0f);
		Renderer2D::DrawLine(glm::vec3(0.0f, -gridSize + cameraPosition.y, 0.0f), glm::vec3(0.0f, gridSize + cameraPosition.y, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f,1.0f), 1.0f);
		Renderer2D::DrawLine(glm::vec3(0.0f, 0.0f, -gridSize + cameraPosition.z), glm::vec3(0.0f, 0.0f, gridSize + cameraPosition.z), glm::vec4(0.0f, 0.0f, 1.0f,1.0f), 1.0f);*/
	}

	// render env in runtime
	void Scene::RenderEnvironment(EditorCamera& camera)
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::vec3 cameraPos;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					cameraPos = transform.Translation;
					break;
				}
			}
		}

		//TODO:temp
		//ModeManager::mSceneMode = SceneMode::EnvironmentHdr;
		switch (ModeManager::mSceneMode)
		{
		case SceneMode::None:
			break;
		case SceneMode::EnvironmentHdr:
			if (mainCamera)
				DrawEnvironmentHdr(glm::inverse(cameraTransform), mainCamera->GetProjection());
			break;
		case SceneMode::SkyBox:
			if (mainCamera)
				DrawSkyBox(glm::inverse(cameraTransform), mainCamera->GetProjection());
			break;
		default:
			break;
		}
	}


	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Physics
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve transform from Box2D
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
				}
			}
		}

		// Render 2D
		RenderScene2D(camera);
	}


	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	Entity Scene::GetEntityWithUUID(UUID id) const
	{
		AS_PROFILE_FUNC();
		AS_CORE_VERIFY(m_EntityIDMap.find(id) != m_EntityIDMap.end(), "Invalid entity ID or entity doesn't exist in scene!");
		AS_CORE_ERROR("Entity Map size {0}", m_EntityIDMap.size());
		return m_EntityIDMap.at(id);
	}

	Entity Scene::TryGetEntityWithUUID(UUID id) const
	{
		//AS_PROFILE_FUNC();
		if (const auto iter = m_EntityIDMap.find(id); iter != m_EntityIDMap.end())
			return iter->second;
		return Entity{};
	}

	Entity Scene::TryGetEntityWithTag(const std::string& tag)
	{
		AS_PROFILE_FUNC();

		auto entities = GetAllEntitiesWith<TagComponent>();
		for (auto e : entities)
		{
			if (entities.get<TagComponent>(e).Tag == tag)
				return Entity(e, const_cast<Scene*>(this));
		}

		return Entity{};
	}

	Entity Scene::TryGetDescendantEntityWithTag(Entity entity, const std::string& tag)
	{
		AS_PROFILE_FUNC();

		if (entity)
		{
			if (entity.GetComponent<TagComponent>().Tag == tag)
				return entity;

			for (const auto childId : entity.Children())
			{
				Entity descendant = TryGetDescendantEntityWithTag(GetEntityWithUUID(childId), tag);
				if (descendant)
					return descendant;
			}
		}
		return {};
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		//TODO:
		// Copy name because we're going to modify component data structure
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
	}
	/*
	Entity Scene::DuplicateEntity(Entity entity)
	{
		AS_PROFILE_FUNC();

		auto parentNewEntity = [&entity, scene = this](Entity newEntity)
			{
				if (auto parent = entity.GetParent(); parent)
				{
					newEntity.SetParentUUID(parent.GetUUID());
					parent.Children().push_back(newEntity.GetUUID());
				}
			};

		if (entity.HasComponent<PrefabComponent>())
		{
			auto prefabID = entity.GetComponent<PrefabComponent>().PrefabID;
			AS_CORE_VERIFY(AssetManager::IsAssetHandleValid(prefabID));
			const auto& entityTransform = entity.GetComponent<TransformComponent>();
			glm::vec3 rotation = entityTransform.GetRotationEuler();
			Entity prefabInstance = Instantiate(AssetManager::GetAsset<Prefab>(prefabID), &entityTransform.Translation, &rotation, &entityTransform.Scale);
			parentNewEntity(prefabInstance);
			return prefabInstance;
		}

		Entity newEntity;
		if (entity.HasComponent<TagComponent>())
			newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);
		else
			newEntity = CreateEntity();

		CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		// NOTE(Peter): We can't use this method for copying the RelationshipComponent since we
		//				need to duplicate the entire child hierarchy and basically reconstruct the entire RelationshipComponent from the ground up
		//CopyComponentIfExists<RelationshipComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<MeshComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<StaticMeshComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<AnimationComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<ScriptComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<TextComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<RigidBody2DComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<RigidBodyComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<CharacterControllerComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<FixedJointComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<BoxColliderComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<SphereColliderComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<CapsuleColliderComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<MeshColliderComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<DirectionalLightComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<PointLightComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<SpotLightComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		CopyComponentIfExists<SkyLightComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<AudioComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);
		//CopyComponentIfExists<AudioListenerComponent>(newEntity.m_EntityHandle, m_Registry, entity.m_EntityHandle);

#if _DEBUG && 0
		// Check that nothing has been forgotten...
		bool foundAll = true;
		m_Registry.visit(entity, [&](entt::id_type type)
			{
				if (type != entt::type_index<RelationshipComponent>().value())
					bool foundOne = false;
				m_Registry.visit(newEntity, [type, &foundOne](entt::id_type newType) {if (newType == type) foundOne = true; });
				foundAll = foundAll && foundOne;
			});
		AS_CORE_ASSERT(foundAll, "At least one component was not duplicated - have you added a new component type and not dealt with it here?");
#endif

		auto childIds = entity.Children(); // need to take a copy of children here, because the collection is mutated below
		for (auto childId : childIds)
		{
			Entity childDuplicate = DuplicateEntity(GetEntityWithUUID(childId));

			// At this point childDuplicate is a child of entity, we need to remove it from that entity
			UnparentEntity(childDuplicate, false);

			childDuplicate.SetParentUUID(newEntity.GetUUID());
			newEntity.Children().push_back(childDuplicate.GetUUID());
		}

		parentNewEntity(newEntity);

		//TODO:BuildBoneEntityIds(newEntity);

		if (newEntity.HasComponent<ScriptComponent>())
			ScriptEngine::DuplicateScriptInstance(entity, newEntity);

		return newEntity;
	}*/

	Entity Scene::FindEntityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}
		return {};
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		// TODO(Yan): Maybe should be assert
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		return {};
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnPhysics3DStart()
	{

		mBroadphase = new btDbvtBroadphase();
		mCollisionConfiguration = new btDefaultCollisionConfiguration();
		mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
		mSolver = new btSequentialImpulseConstraintSolver();
		mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
		mDynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));
		//mDynamicsWorld->setForceUpdateAllAabbs(true);

		auto view = this->m_Registry.view<TransformComponent, Rigidbody3DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb3d = entity.GetComponent<Rigidbody3DComponent>();

			btTransform trans;
			btCollisionShape* shape;
			btVector3 inertia{ 0.0f, 0.0f, 0.0f };

			if (rb3d.Shape == CollisionShape::Box)
			{
				// Calculate Obb
				AS_CORE_ASSERT(entity.HasComponent<MeshComponent>());
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& mesh = entity.GetComponent<MeshComponent>();

				std::vector<glm::vec3> vertices;
				for (const auto& subMesh : mesh.m_Mesh->mSubMeshes)
				{
					if (subMesh.mStaticVertices.empty())
					{
						for (const auto& vertex : subMesh.mSkinnedVertices)
						{
							vertices.emplace_back(vertex.Pos);
						}
					}
					else
					{
						for (const auto& vertex : subMesh.mStaticVertices)
						{
							vertices.emplace_back(vertex.Pos);
						}
					}
				}

				glm::vec3 originPos(0.0f);
				glm::mat3 covMat = Math::CalculateCovMatrix(vertices, originPos);

				glm::vec3 eValues;
				glm::mat3 eVectors;
				Math::JacobiSolver(covMat, eValues, eVectors);

				for (int i = 0; i < 3; i++)
				{
					if (eValues[i] == 0 || i == 2)
					{
						Math::SchmidtOrthogonalization(eVectors[(i + 1) % 3], eVectors[(i + 2) % 3], eVectors[i]);
						break;
					}
				}

				constexpr float infinity = std::numeric_limits<float>::infinity();
				glm::vec3 minExtents(infinity, infinity, infinity);
				glm::vec3 maxExtents(-infinity, -infinity, -infinity);

				for (const glm::vec3& displacement : vertices)
				{
					minExtents[0] = std::min(minExtents[0], glm::dot(displacement, eVectors[0]));
					minExtents[1] = std::min(minExtents[1], glm::dot(displacement, eVectors[1]));
					minExtents[2] = std::min(minExtents[2], glm::dot(displacement, eVectors[2]));

					maxExtents[0] = std::max(maxExtents[0], glm::dot(displacement, eVectors[0]));
					maxExtents[1] = std::max(maxExtents[1], glm::dot(displacement, eVectors[1]));
					maxExtents[2] = std::max(maxExtents[2], glm::dot(displacement, eVectors[2]));
				}

				glm::vec3 halfExtent = (maxExtents - minExtents) / 2.0f;
				glm::vec3 offset = halfExtent + minExtents;
				originPos += offset.x * eVectors[0] + offset.y * eVectors[1] + offset.z * eVectors[2];
				glm::vec3 offsetScale = originPos * (transform.Scale - 1.0f);
				originPos += offsetScale;
				originPos = glm::mat3(transform.GetRotationMatrix()) * originPos;
				originPos += transform.Translation;

				shape = new btBoxShape(btVector3(halfExtent.x * transform.Scale.x, halfExtent.y * transform.Scale.y, halfExtent.z * transform.Scale.z));
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				trans.setOrigin(Utils::GlmToBtVec3(originPos));

				auto comQuat = glm::quat(transform.Rotation) * glm::quat(glm::mat4(eVectors));
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				trans.setRotation(btQuat);
			}
			else if (rb3d.Shape == CollisionShape::Sphere)
			{
				shape = new btSphereShape(transform.Scale.x);
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				trans.setOrigin(btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z));

				auto comQuat = glm::quat(transform.Rotation);
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				trans.setRotation(btQuat);
			}
			else if (rb3d.Shape == CollisionShape::ConvexHull && entity.HasComponent<MeshComponent>())
			{
				auto& meshc = entity.GetComponent<MeshComponent>();

				shape = new btConvexHullShape();

				for (const auto& submesh : meshc.m_Mesh->mSubMeshes)
				{
					auto& staticVertices = submesh.mStaticVertices;
					auto& skinnedVertices = submesh.mSkinnedVertices;
					for (const auto& vertex : staticVertices)
					{
						static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Pos.x * transform.Scale.x, vertex.Pos.y * transform.Scale.y, vertex.Pos.z * transform.Scale.z));
					}
					for (const auto& vertex : skinnedVertices)
					{
						static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Pos.x * transform.Scale.x, vertex.Pos.y * transform.Scale.y, vertex.Pos.z * transform.Scale.z));
					}
				}

				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				trans.setOrigin(btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z));

				auto comQuat = glm::quat(transform.Rotation);
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				trans.setRotation(btQuat);
			}

			btDefaultMotionState* motion = new btDefaultMotionState(trans);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(rb3d.mass, motion, shape, inertia);
			rbInfo.m_linearDamping = rb3d.linearDamping;
			rbInfo.m_angularDamping = rb3d.angularDamping;
			rbInfo.m_restitution = rb3d.restitution;
			rbInfo.m_friction = rb3d.friction;

			btRigidBody* body = new btRigidBody(rbInfo);
			body->setSleepingThresholds(0.01f, glm::radians(0.1f));
			body->setActivationState(DISABLE_DEACTIVATION);

			if (rb3d.Type == Rigidbody3DComponent::Body3DType::Static)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
			}
			else if (rb3d.Type == Rigidbody3DComponent::Body3DType::Kinematic)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			}
			else if (rb3d.Type == Rigidbody3DComponent::Body3DType::Dynamic)
			{
				AS_CORE_ASSERT("Rigidbody Dynamic Error at {0}", __FILE__);
				//body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
			}

			rb3d.RuntimeBody = body;
			mDynamicsWorld->addRigidBody(body);
		}
	}

	void Scene::OnPhysics3DRuntime(Timestep ts)
	{

		mDynamicsWorld->stepSimulation(ts, 10);

		auto view = this->m_Registry.view<TransformComponent, Rigidbody3DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto componentsTuple = entity.GetComponents<TransformComponent, Rigidbody3DComponent>();
			auto [transform, rb3d] = componentsTuple;

			btRigidBody* body = (btRigidBody*)(*rb3d).RuntimeBody;
			const auto& trans = body->getWorldTransform();
			(*transform).Translation.x = trans.getOrigin().x();
			(*transform).Translation.y = trans.getOrigin().y();
			(*transform).Translation.z = trans.getOrigin().z();

			btScalar yawZ, pitchY, rollX;
			trans.getRotation().getEulerZYX(yawZ, pitchY, rollX);

			(*transform).Rotation.x = rollX;
			(*transform).Rotation.y = pitchY;
			(*transform).Rotation.z = yawZ;
		}

		if (ModeManager::bShowPhysicsColliders)
		{
			Entity camera = this->GetPrimaryCameraEntity();
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());

			mDynamicsWorld->setDebugDrawer(&mDebugDrawer);
			mDynamicsWorld->debugDrawWorld();

			Renderer2D::EndScene();
		}
	}

	void Scene::OnPhysics3DEditor(EditorCamera& camera)
	{
		static bool initFlag = true;
		if (ModeManager::bShowPhysicsColliders)
		{
			//if (initFlag)
			OnRuntimeStart();

			Renderer2D::BeginScene(camera);

			mDynamicsWorld->setDebugDrawer(&mDebugDrawer);
			mDynamicsWorld->debugDrawWorld();

			Renderer2D::EndScene();

			OnRuntimeStop();
			//initFlag = false;
		}
		else
		{
			//if (!initFlag)
				//OnAspectStop();
			//initFlag = true;
		}
	}

	void Scene::OnPhysics3DStop()
	{
		//TODO: Fix physics 3d
		//auto view = mLevel->mRegistry.view<Rigidbody3DComponent>();
		//for (auto e : view)
		//{
		//	Entity entity = { e, mLevel };
		//	auto& rb3d = entity.GetComponent<Rigidbody3DComponent>();

		//	// it seems when you delete the btRigidBody,
		//	// the motion and the shape will also be deleted
		//	delete (btRigidBody*)rb3d.AspectBody; 
		//}

		// it seems we only need to delete the DynamicsWorld
		delete mDynamicsWorld;
		delete mSolver;
		delete mDispatcher;
		delete mCollisionConfiguration;
		delete mBroadphase;
	}

	void Scene::RenderScene2D(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		//TODO: temporarily render grid in 2d 
		//TODO: Render Grid
		//DrawGrid(camera.GetPosition(), glm::vec3(1, 1, 1));

		// Draw sprites
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

				Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::RenderScene3D(EditorCamera& camera)
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::vec3 cameraPos;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					cameraPos = transform.Translation;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer3D::BeginScene(*mainCamera, cameraTransform);

			// Get the Light 
			{

			}

			// Draw model
			{
				auto view = m_Registry.view<TransformComponent, MeshComponent>();
				for (auto entity : view)
				{
					auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);
					//AS_CORE_WARN(mesh.Path);
					Renderer3D::DrawModel(transform.GetTransform(), cameraPos, mesh, (int)entity);
				}
			}

			Renderer3D::EndScene();
		}
	}

	void Scene::SortEntities()
	{
		m_Registry.sort<IDComponent>([&](const auto lhs, const auto rhs)
			{
				auto lhsEntity = m_EntityIDMap.find(lhs.ID);
				auto rhsEntity = m_EntityIDMap.find(rhs.ID);
				return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
			});
	}

	//TODO:================================
	void Scene::ConvertToLocalSpace(Entity entity)
	{
		AS_PROFILE_FUNC();

		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

		if (!parent)
			return;

		auto& transform = entity.Transform();
		glm::mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);
		glm::mat4 localTransform = glm::inverse(parentTransform) * transform.GetTransform();
		transform.SetTransform(localTransform);
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
		AS_PROFILE_FUNC();

		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

		if (!parent)
			return;

		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		auto& entityTransform = entity.Transform();
		entityTransform.SetTransform(transform);
	}

	glm::mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
	{
		//AS_PROFILE_FUNC();

		glm::mat4 transform(1.0f);

		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
		if (parent)
			transform = GetWorldSpaceTransformMatrix(parent);

		return transform * entity.Transform().GetTransform();
	}
	/*
	// TODO: Definitely cache this at some point
	TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
	{
		AS_PROFILE_FUNC();

		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		TransformComponent transformComponent;
		transformComponent.SetTransform(transform);
		return transformComponent;
	}
	//======================================
	*/

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	//TODO:
	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
	{
	}


	template<>
	void Scene::OnComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody3DComponent>(Entity entity, Rigidbody3DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
	{
	}



	void Scene::DrawEnvironmentHdr(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix)
	{
		if (!ModeManager::bHdrUse)
		{
			return;
		}

		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = ProjectMatrix * glm::mat4(glm::mat3(ViewMatrix));
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		oldId = id;
		Ref<Texture2D> hdrTex = Library<Texture2D>::GetInstance().Get("DefaultHdr");
		id = hdrTex->GetRendererID();

		Ref<CubeMapTexture> envCubemap = Library<CubeMapTexture>::GetInstance().Get("EnvironmentHdr");

		if (id != oldId)
		{
			Ref<Shader> equirectangularToCubemapShader = Library<Shader>::GetInstance().Get("IBL_equirectangularToCubemap");
			equirectangularToCubemapShader->Bind();
			equirectangularToCubemapShader->SetInt("equirectangularMap", 0);
			equirectangularToCubemapShader->SetMat4("projection", captureProjection);

			int framebufferOld = 0;
			framebufferOld = RenderCommand::GetDrawFrameBuffer();

			// Temp
			unsigned int captureFBO;
			unsigned int captureRBO;
			glGenFramebuffers(1, &captureFBO);
			glGenRenderbuffers(1, &captureRBO);

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

			hdrTex->Bind();
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			RenderCommand::SetViewport(0, 0, envCubemap->GetWidth(), envCubemap->GetHeight());
			for (unsigned int i = 0; i < 6; ++i)
			{
				equirectangularToCubemapShader->SetMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap->GetRendererID(), 0);
				RenderCommand::Clear();

				Library<Mesh>::GetInstance().Get("Box")->Draw();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// End temp

			//FramebufferSpecification fbSpec;
			//fbSpec.Attachments = { FramebufferTextureFormat::DEPTH24STENCIL8 };
			//fbSpec.Width = 512;
			//fbSpec.Height = 512;
			//static Ref<Framebuffer> captureFBO = Framebuffer::Create(fbSpec);

			//hdrTex->Bind();
			//captureFBO->Bind();
			//captureFBO->BindDrawFramebuffer();
			//RenderCommand::SetViewport(0, 0, envCubemap->GetWidth(), envCubemap->GetHeight());
			//for (unsigned int i = 0; i < 6; ++i)
			//{
			//	equirectangularToCubemapShader->SetMat4("view", captureViews[i]);
			//	captureFBO->FramebufferTexture2D(i, envCubemap->GetRendererID());
			//	RenderCommand::Clear();

			//	Library<Mesh>::GetInstance().Get("Box")->Draw();
			//}
			//captureFBO->Unbind();


			envCubemap->Bind(0);
			envCubemap->GenerateMipmap();

			// irradiance map
			Ref<CubeMapTexture> irradianceMap = Library<CubeMapTexture>::GetInstance().Get("EnvironmentIrradiance");
			Ref<Shader> irradianceShader = Library<Shader>::GetInstance().Get("IBL_irradiance");



			//Temp
			// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
			// --------------------------------------------------------------------------------
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap->GetRendererID());
			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

			// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
			// -----------------------------------------------------------------------------
			irradianceShader->Bind();
			irradianceShader->SetInt("environmentMap", 0);
			irradianceShader->SetMat4("projection", captureProjection);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap->GetRendererID());

			glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			for (unsigned int i = 0; i < 6; ++i)
			{
				irradianceShader->SetMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap->GetRendererID(), 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				Library<Mesh>::GetInstance().Get("Box")->Draw();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//end temp


			//captureFBO->Bind();
			//glBindRenderbuffer(GL_RENDERBUFFER, captureFBO->GetDepthAttachmentRendererID());
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 32, 32);

			//irradianceShader->Bind();
			//irradianceShader->SetInt("environmentMap", 0);
			//irradianceShader->SetMat4("projection", captureProjection);
			//envCubemap->Bind(0);

			//RenderCommand::SetViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
			//captureFBO->Bind();
			//for (unsigned int i = 0; i < 6; ++i)
			//{
			//	irradianceShader->SetMat4("view", captureViews[i]);
			//	captureFBO->FramebufferTexture2D(i, irradianceMap->GetRendererID());
			//	RenderCommand::Clear();

			//	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
			//	Library<Mesh>::GetInstance().Get("Box")->Draw();
			//}
			//captureFBO->Unbind();
			Library<CubeMapTexture>::GetInstance().Set("EnvironmentIrradiance", irradianceMap);

			// prefilter map
			Ref<CubeMapTexture> prefilterMap = Library<CubeMapTexture>::GetInstance().Get("EnvironmentPrefilter");
			Ref<Shader> prefilterShader = Library<Shader>::GetInstance().Get("IBL_prefilter");
			prefilterMap->Bind();
			prefilterMap->GenerateMipmap();
			prefilterShader->Bind();
			prefilterShader->SetInt("environmentMap", 0);
			prefilterShader->SetMat4("projection", captureProjection);
			envCubemap->Bind(0);

			// Temp
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				// reisze framebuffer according to mip-level size.
				unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
				unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
				glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
				glViewport(0, 0, mipWidth, mipHeight);

				float roughness = (float)mip / (float)(maxMipLevels - 1);
				prefilterShader->SetFloat("roughness", roughness);
				for (unsigned int i = 0; i < 6; ++i)
				{
					prefilterShader->SetMat4("view", captureViews[i]);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap->GetRendererID(), mip);

					RenderCommand::Clear();
					Library<Mesh>::GetInstance().Get("Box")->Draw();
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// End temp


			//prefilterShader->Bind();
			//prefilterShader->SetInt("environmentMap", 0);
			//prefilterShader->SetMat4("projection", captureProjection);
			//envCubemap->Bind(0);
			//captureFBO->Bind();
			//unsigned int maxMipLevels = 5;
			//for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			//{
			//	// reisze framebuffer according to mip-level size.
			//	unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			//	unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			//	
			//	glBindRenderbuffer(GL_RENDERBUFFER, captureFBO->GetDepthAttachmentRendererID());
			//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			//	RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);

			//	float roughness = (float)mip / (float)(maxMipLevels - 1);
			//	prefilterShader->SetFloat("roughness", roughness);
			//	for (unsigned int i = 0; i < 6; ++i)
			//	{
			//		prefilterShader->SetMat4("view", captureViews[i]);
			//		captureFBO->FramebufferTexture2D(i, prefilterMap->GetRendererID());

			//		RenderCommand::Clear();
			//		Library<Mesh>::GetInstance().Get("Box")->Draw();
			//	}
			//}
			//captureFBO->Unbind();
			Library<CubeMapTexture>::GetInstance().Set("EnvironmentPrefilter", prefilterMap);


			RenderCommand::BindFrameBuffer(framebufferOld);
		}

		envCubemap->Bind(0);

		RenderCommand::SetViewport(0, 0, ConfigManager::m_ViewportSize.x, ConfigManager::m_ViewportSize.y);

		RenderCommand::DepthFunc(DepthComp::LEQUAL);

		Ref<Shader> backgroundShader = Library<Shader>::GetInstance().Get("IBL_background");
		backgroundShader->Bind();
		backgroundShader->SetInt("environmentMap", 0);
		backgroundShader->SetFloat("SkyBoxLod", environmentSettings.SkyBoxLod);
		backgroundShader->SetFloat("exposure", environmentSettings.exposure);

		Library<Mesh>::GetInstance().Get("Box")->Draw();
		RenderCommand::DepthFunc(DepthComp::LESS);
	}

	void Scene::DrawSkyBox(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = ProjectMatrix * glm::mat4(glm::mat3(ViewMatrix));
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));

		RenderCommand::Cull(0);

		RenderCommand::DepthFunc(DepthComp::LEQUAL);
		Library<Shader>::GetInstance().GetSkyBoxShader()->Bind();

		Library<CubeMapTexture>::GetInstance().Get("SkyBoxTexture")->Bind(0);
		Library<Shader>::GetInstance().GetSkyBoxShader()->SetInt("SkyBox", 0);
		Library<Mesh>::GetInstance().Get("Box")->Draw();

		RenderCommand::DepthFunc(DepthComp::LESS);
	}
}
