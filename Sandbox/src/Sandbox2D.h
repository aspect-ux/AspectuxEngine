#pragma once

#include "Aspect.h"

class Sandbox2D : public Aspect::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Aspect::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Aspect::Event& e) override;
private:
	Aspect::OrthographicCameraController m_CameraController;
	
	// Temp
	Aspect::Ref<Aspect::VertexArray> m_SquareVA;
	Aspect::Ref<Aspect::Shader> m_FlatColorShader;

	Aspect::Ref<Aspect::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};
