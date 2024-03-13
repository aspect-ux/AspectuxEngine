#pragma once
#include "Aspect.h"

// ���������
struct ParticleProps
{
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime = 1.0f;
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t maxParticle = 10000);

	void OnUpdate(Aspect::Timestep ts);
	void OnRender(Aspect::OrthographicCamera& camera);

	void Emit(const ParticleProps& particleProps);
private:
	// һ������������
	struct Particle
	{
		glm::vec2 Position;
		glm::vec2 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};
	std::vector<Particle> m_ParticlePool;// ���ӳ�
	uint32_t m_PoolIndex = 999;
};