#pragma once

#include "Aspect/Renderer/EditorCamera.h"
#include "Aspect/Scene/Components.h"
#include "Aspect/Renderer/Texture.h"
#include "Aspect/Library/ShaderLibrary.h"
#include <glad/glad.h>
#include "Aspect/Renderer/ExtendedRenderer/RasterizationMode.h"

namespace Aspect
{
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		// syntax error identifier 'meshcompo',可能是文件重复include
		static void DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, MeshComponent& meshComponent, int EntityID);

		// to be moved to opengl folder
		void SetRasterizationMode(RasterizationMode p_rasterizationMode)
		{
			// 多边形填充模式
			glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(p_rasterizationMode));
		}
		
		//from overload
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& color, float lineWidth)
		{
			/*
			auto m_LineShader = Library<Shader>::GetInstance().Get("LineShader");
			m_LineShader->Bind();

			m_LineShader->SetFloat3("start", p0);
			m_LineShader->SetFloat3("end", p1);
			m_LineShader->SetFloat3("color", color);

			SetRasterizationMode(RasterizationMode::LINE);
			//SetRasterizationLinesWidth(lineWidth);
			//m_renderer.Draw(*m_lineMesh, Settings::EPrimitiveMode::LINES);
			//m_renderer.SetRasterizationLinesWidth(1.0f);
			SetRasterizationMode(RasterizationMode::FILL);

			m_LineShader->Unbind();*/
		}

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		
	public:
		// shadow pass
		static Ref<class FrameBuffer> lightFBO;
	};
}
