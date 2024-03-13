#pragma once

#include <Aspect.h>

#include "Platform/OpenGL/OpenGLShader.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"

#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Aspect/Core/EntryPoint.h>

// =======================1. dll import test 2. glm import test===============================
//#pragma once
//namespace Aspect
//{
//	__declspec(dllimport) void Print();
//}
/*
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}*/
//================================================================================================

class ExampleLayer : public Aspect::Layer
{
public:
	ExampleLayer() : Layer("Example"), m_CameraController(1280.0f / 720.0f, true), m_SquarePosition(0.0f)
	{
		// ===========������vertexArray,vertexBuffer=============
		//glGenVertexArrays(1, &m_VertexArray);
		//glBindVertexArray(m_VertexArray);

		//glGenBuffers(1, &m_VertexBuffer);
		//glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		m_VertexArray = (Aspect::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f,-0.5f,0.0f, 1.0f,0.0f,1.0f,1.0f,
			 0.5f,-0.5f,0.0f, 0.0f,0.0f,1.0f,1.0f,
			 0.0f, 0.5f, 0.0, 1.0f,1.0f,1.0f,1.0f
		};
		Aspect::Ref<Aspect::VertexBuffer> vertexBuffer;
		vertexBuffer = (Aspect::VertexBuffer::Create(vertices, sizeof(vertices)));
		Aspect::BufferLayout layout = {
			{Aspect::ShaderDataType::Float3,"a_Position"}, // a: application
			{Aspect::ShaderDataType::Float4,"a_Color"}
			//{Aspect::ShaderDataType::Float2,"a_TexCoord"}
		};
		/*VertexBuffer buffer = VertexBuffer::Create(sizeof(vertices), vertices);
		buffer.Bind();*/
		vertexBuffer->SetLayout(layout);

		// ���ö���ָ������
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		//glGenBuffers(1, &m_IndexBuffer); // ���ƶ������index˳��
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		Aspect::Ref<Aspect::IndexBuffer> indexBuffer;
		uint32_t indices[3] = { 0,1,2 };
		indexBuffer = (Aspect::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		//======================================
		m_SquareVA = (Aspect::VertexArray::Create());

		// Ҳ���Ը���ͼ���rgbֵ���ж�ͼ�������Ƿ���ȷ��Ӧ
		float squareVertices[5 * 4] = {
			-0.5f,-0.5f,0.0f,  0.0f,0.0f,
			 0.5f,-0.5f,0.0f,  1.0f,0.0f,
			 0.5f, 0.5f,0.0f,  1.0f,1.0f,
			-0.5f, 0.5f,0.0f,  0.0f,1.0f
		};
		Aspect::Ref<Aspect::VertexBuffer> squareVB;
		squareVB = (Aspect::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		// layout �����Ƕ����鲼�ֵ����ã�����ֱ�ӳ������������ֱ�Ӵ�������ӳ����Ӧ������
		squareVB->SetLayout({
			{Aspect::ShaderDataType::Float3,"a_Position"},
			{Aspect::ShaderDataType::Float2,"a_TexCoord"}
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2,2,3,0 };
		Aspect::Ref<Aspect::IndexBuffer> squareIB;
		squareIB = (Aspect::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		// R����ԭʼ�ַ����������ڲ��ᱻ���������ԣ��Ҳ��ᱻת��
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjectionMatrix;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);	
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		// shader2
		std::string flatColorVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjectionMatrix;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);	
			}
		)";
		std::string flatColorFragSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color,1.0);
			}
		)";


		// texture shader
		std::string textureVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjectionMatrix;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);	
			}
		)";
		std::string textureFragSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture,v_TexCoord);
			}
		)";
		//m_Shader = std::make_unique<Shader>();

		/*@point shared_ptr��reset����
		* 1. reset�������õ���ָ���ָ��
		*    ������Ϊ�ײ����make_shared�ķ�ʽ��ʼ�������Բ�����reset;ֱ�ӵ��ںŸ�ֵ����
		* 2. ��ȡ���ִ���shader(��ȡshader)�ķ�ʽ
		*    ʹ����ShaderLibrary�⣬���Ը���·����ȡglsl�ļ�����shader��Ҳ���Դ���name,vert,frag����shader
		*/
		m_Shader = Aspect::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		m_flatColorShader = m_ShaderLibrary.Load("assets/shaders/FlatColor.glsl");
		//m_flatColorShader = Aspect::Shader::Create("FlatColor", flatColorVertexSrc, flatColorFragSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = (Aspect::Texture2D::Create("assets/textures/Checkerboard.png"));
		m_AspectLogTexture = (Aspect::Texture2D::Create("assets/textures/ChernoLogo.png"));

		std::dynamic_pointer_cast<Aspect::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Aspect::OpenGLShader>(textureShader)->UploadUniformInt("u_Textuer", 0);
	}

	void OnUpdate(Aspect::Timestep ts) override
	{
		// AS_TRACE("Delta Time = {0}s : ({1}ms)", ts.GetSeconds(),ts.GetMilliseconds());

		// 1. ע�⣬���������update��ܹ���֤ÿ֡����⣬�ƶ�������������¼�����ã������ᱻ���Ϊһ�Σ�Ҳ��ֻ�ƶ�һ�Ρ�
		//    ����������ƶ��ĽǶ�������update����������һ���棬�¼�ֻ�����һ�Σ����ɿء�
		// 2. ����Timestep,������Unity��Time.deltaTime ��Ϊ����ʱ��m֡��ʱ��n֡�����ƶ�������updateÿ֡����һ�Σ���������֡�����ȶ���
		//    ����ÿһ֡ʱ�䲻һ��������ÿһ֡�ƶ�����һ����Ҳ���ƶ���ͬ����ȴ���˲�ͬʱ�䣬�ƶ��ٶȲ�����Ҫ��
		//    Timestep�ĺô���ʵʱ���㵱ǰ֡��deltaTime,����ֱ����ΪȨ�س����ٶȣ����ܱ�֤����֡�����ȶ���Ҳ�����ȶ����ƶ������
		// 3. OpenGL����������ϵ

		/* Move Square
		if (Aspect::Input::IsKeyPressed(Aspect::Key::L))
			m_SquarePosition.x += m_SquareSpeed * ts;  // ����timestep,

		if (Aspect::Input::IsKeyPressed(Aspect::Key::J))
			m_SquarePosition.x -= m_SquareSpeed * ts;

		if (Aspect::Input::IsKeyPressed(Aspect::Key::I))
			m_SquarePosition.y += m_SquareSpeed * ts;

		if (Aspect::Input::IsKeyPressed(Aspect::Key::K))
			m_SquarePosition.y -= m_SquareSpeed * ts;*/

			/// Update
		m_CameraController.OnUpdate(ts);

		/// Renderer
		Aspect::RenderCommand::SetClearColor(glm::vec4({ 0.1f, 0.1f, 0.1f, 1.0f }));
		Aspect::RenderCommand::Clear();

		// ���������update ����λ�ú���ת
		//m_Camera.SetPosition(m_CameraPosition);
		//m_Camera.SetRotation(m_CameraRotation);

		Aspect::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		//glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
		//glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		std::dynamic_pointer_cast<Aspect::OpenGLShader>(m_flatColorShader)->Bind();
		std::dynamic_pointer_cast<Aspect::OpenGLShader>(m_flatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				// if (i % 2 == 0) m_flatColorShader->UploadUniformFloat4("u_Color", redColor);
				// else  m_flatColorShader->UploadUniformFloat4("u_Color",blueColor);
				Aspect::Renderer::Submit(m_flatColorShader, m_SquareVA, transform);
			}
		}

		// ��ShaderLibrary��Getָ��shader
		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();

		Aspect::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_AspectLogTexture->Bind();

		Aspect::Renderer::Submit(textureShader, m_SquareVA,
			glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Aspect::Renderer::Submit(m_Shader, m_VertexArray);
		Aspect::Renderer::EndScene();
	}

	void OnEvent(Aspect::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

	bool OnKeyPressedEvent(Aspect::KeyPressedEvent& event)
	{


		return false;
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("ImGui Settings");
		ImGui::Text("Hello World");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
private:
	Aspect::ShaderLibrary m_ShaderLibrary;
	Aspect::Ref<Aspect::Shader> m_Shader;
	Aspect::Ref<Aspect::VertexArray> m_VertexArray;

	Aspect::Ref<Aspect::Shader> m_flatColorShader;// , m_TextureShader;
	Aspect::Ref<Aspect::VertexArray> m_SquareVA;
	Aspect::Ref<Aspect::Texture2D> m_Texture, m_AspectLogTexture;

	Aspect::OrthographicCameraController m_CameraController; // ʹ��controller���������

	glm::vec3 m_SquarePosition;
	float m_SquareSpeed = 1.0f;

	glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};
