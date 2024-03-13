#pragma once

#include "Aspect.h"

#include "ParticleSystem.h"


class Sandbox2D : public Aspect::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(Aspect::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Aspect::Event& e) override;
private:
	Aspect::OrthographicCameraController m_CameraController; // 使用controller来控制相机

	// Temp
	Aspect::Ref<Aspect::VertexArray> m_SquareVA;
	Aspect::Ref<Aspect::Shader> m_FlatColorShader;
	Aspect::Ref<Aspect::Texture2D> m_Checkerboard;
	Aspect::Ref<Aspect::Texture2D> m_SpriteSheet;

	Aspect::Ref<Aspect::SubTexture2D> m_TextureStair, m_TextureBush, m_TextureTree;

	struct ProfileResult
	{
		const char* Name;
		float Time;
	};
	std::vector<ProfileResult> m_ProfileResults;

	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0};
	glm::vec3 m_SquarePosition;

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;

	std::unordered_map<char, Aspect::Ref<Aspect::SubTexture2D>> s_TextureMap;// map数据结构

	uint32_t m_MapWidth, m_MapHeight;

	Aspect::Ref<Aspect::FrameBuffer> m_FrameBuffer;
};