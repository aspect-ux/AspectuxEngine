#pragma once

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Animation/AnimationData.h"
#include "Aspect/Animation/Animator.h"

#include "Aspect/Renderer/Shader.h"
#include "Aspect/Renderer/SubMesh.h"
#include "Aspect/Renderer/Texture.h"
#include "Aspect/Renderer/Material.h"
#include "Aspect/Library/ShaderLibrary.h"
#include "Aspect/Renderer/Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <optional>
#include <map>

namespace Aspect
{
	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const Mesh&) = default;
		Mesh(const std::string& path)
		{
			mMaterial.push_back(CreateRef<Material>(Library<Shader>::GetInstance().GetDefaultShader()));
			LoadModel(path);
		}

		Mesh(const std::string& path, Ref<Shader> shader)
		{
			mMaterial.push_back(CreateRef<Material>(shader));
			LoadModel(path);
		}

		void SetShader(Ref<Shader> shader) { mMaterial[0]->SetShader(shader); };
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, int entityID);
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, Ref<Shader> shader, int entityID);

		void Draw();

		auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		int& GetBoneCount() { return mBoneCounter; }
	private:
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, uint32_t& subMeshIndex);

		template <typename Vertex>
		SubMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, uint32_t& subMeshIndex);
		std::optional<std::vector<MaterialTexture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, uint32_t& subMeshIndex);
	public:
		// Animation
		bool bAnimated = false;
		bool bPlayAnim = false;
		bool bStopAnim = false;

		Animation m_Animation;
		Animator m_Animator;

		float mAnimPlaySpeed = 1.0f;

		std::vector<Ref<Material>> mMaterial;
		std::vector<SubMesh> mSubMeshes;
	private:
		std::string mDirectory;

		// Animation
		int mBoneCounter = 0;
		std::map<std::string, BoneInfo> m_BoneInfoMap;
	};
}
