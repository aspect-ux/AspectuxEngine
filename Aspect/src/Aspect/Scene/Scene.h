#pragma once

#include "Aspect/Core/Timestep.h"
#include "Aspect/Core/UUID.h"
#include "Aspect/Renderer/EditorCamera.h"
#include "Aspect/Scene/Components.h"
#include "Aspect/Physics/BulletPhysics/3D/PhysicsSystem3D.h"

#include "entt.hpp"

class b2World;

namespace Aspect {

	class Entity;

	class SceneRenderer;
	class Renderer2D;
	class Prefab;

	struct DirLight
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };

		float Intensity = 1.0f;
	};

	struct DirectionalLight
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		//glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		//float ShadowAmount = 1.0f;
		// C++ only
		//bool CastShadows = true;
	};

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };
		float MinRadius = 0.001f;
		float Radius = 25.0f;
		float Falloff = 1.f;
		float SourceSize = 0.1f;
		bool CastsShadows = true;
		char Padding[3]{ 0, 0, 0 };
	};

	struct SpotLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		float AngleAttenuation = 0.0f;
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };
		float Range = 0.1f;
		float Angle = 0.0f;
		float Falloff = 1.0f;
		bool SoftShadows = true;
		char Padding0[3]{ 0, 0, 0 };
		bool CastsShadows = true;
		char Padding1[3]{ 0, 0, 0 };
	};

	struct LightEnvironment
	{
		static constexpr size_t MaxDirectionalLights = 4;

		DirectionalLight DirectionalLights[MaxDirectionalLights];
		std::vector<PointLight> PointLights;
		std::vector<SpotLight> SpotLights;
		[[nodiscard]] uint32_t GetPointLightsSize() const { return (uint32_t)(PointLights.size() * sizeof PointLight); }
		[[nodiscard]] uint32_t GetSpotLightsSize() const { return (uint32_t)(SpotLights.size() * sizeof SpotLight); }
	};

	//TODO:
	//temp for skybox and environment hdr
	struct EnvironmentHdrSettings
	{
		float SkyBoxLod = 0.0f;
		float exposure = 1.0f;
	};


	class Entity;
	using EntityMap = std::unordered_map<UUID, Entity>;

	class Scene : public RefCounted
	{
	public:
		Scene();
		~Scene();

		void OnUpdateRuntime(Timestep ts); //update physics & script & render2D(move to runtime?)
		void OnRenderRuntime(Timestep ts, EditorCamera& camera);//render 3D
		
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);// update animation etc.
		void OnRenderEditor(Timestep ts, EditorCamera& editorCamera);// render 3D & 2D

		void OnUpdateSimulation(Timestep ts, EditorCamera& camera); // update physics

		//TODO: Render Grid
		void DrawGrid(const glm::vec3& position, const glm::vec3& gridColor);
		void RenderEnvironment(EditorCamera& camera); // for skybox etc.

		void ParentEntity(Entity entity, Entity parent);
		void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

		static AspectRef<Scene> Copy(AspectRef<Scene> other);

		//TODO:
		UUID GetUUID() const { return m_SceneID; }
		void CopyTo(AspectRef<Scene>& target);
		static AspectRef<Scene> GetScene(UUID uuid);
		bool IsPlaying() const { return m_IsPlaying; }

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "", bool shouldSort = true);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		Entity FindEntityByName(std::string_view name);
		Entity GetEntityByUUID(UUID uuid);

		Entity GetPrimaryCameraEntity();

		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		void Step(int frames = 1);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		// return entity with id as specified. entity is expected to exist (runtime error if it doesn't)
		Entity GetEntityWithUUID(UUID id) const;

		// return entity with id as specified, or empty entity if cannot be found - caller must check
		Entity TryGetEntityWithUUID(UUID id) const;

		// return entity with tag as specified, or empty entity if cannot be found - caller must check
		Entity TryGetEntityWithTag(const std::string& tag);

		// return descendant entity with tag as specified, or empty entity if cannot be found - caller must check
		// descendant could be immediate child, or deeper in the hierachy
		Entity TryGetDescendantEntityWithTag(Entity entity, const std::string& tag);

		void ConvertToLocalSpace(Entity entity);
		void ConvertToWorldSpace(Entity entity);
		glm::mat4 GetWorldSpaceTransformMatrix(Entity entity);
		//TransformComponent GetWorldSpaceTransform(Entity entity);
		
		template<typename TComponent>
		void CopyComponentIfExists(entt::entity dst, entt::registry& dstRegistry, entt::entity src)
		{
			//TODO: has -> all_of
			if (m_Registry.all_of<TComponent>(src))
			{
				auto& srcComponent = m_Registry.get<TComponent>(src);
				dstRegistry.emplace_or_replace<TComponent>(dst, srcComponent);
			}
		}
		
		
		template<typename TComponent>
		static void CopyComponentFromScene(Entity dst, AspectRef<Scene> dstScene, Entity src, AspectRef<Scene> srcScene)
		{
			srcScene->CopyComponentIfExists<TComponent>((entt::entity)dst, dstScene->m_Registry, (entt::entity)src);
		}


		//TODO: delete
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
		}

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void OnPhysics3DStart();
		void OnPhysics3DRuntime(Timestep ts);
		void OnPhysics3DEditor(EditorCamera& camera);
		void OnPhysics3DStop();

		void RenderScene2D(EditorCamera& camera);
		void RenderScene3D(EditorCamera& camera);		

		void SortEntities();
	private:
		// temp for draw environment
		void DrawEnvironmentHdr(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix);
		void DrawSkyBox(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix);
	public:
		static EnvironmentHdrSettings environmentSettings;
	private:
		UUID m_SceneID;
		std::string m_Name;
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		EntityMap m_EntityIDMap;

		LightEnvironment m_LightEnvironment;

		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;

		b2World* m_PhysicsWorld = nullptr;

		std::vector<std::function<void()>> m_PostUpdateQueue;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		// TODO:
		bool m_IsPlaying = false;

		// TODO: to be removed
		//AspectRef<Shader> m_gridShader;
	private:
		btBroadphaseInterface* mBroadphase;
		btDefaultCollisionConfiguration* mCollisionConfiguration;
		btCollisionDispatcher* mDispatcher;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld* mDynamicsWorld;

		static Utils::BulletDrawer mDebugDrawer;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}
