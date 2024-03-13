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
		// ===========创建绑定vertexArray,vertexBuffer=============
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

		// 设置顶点指针属性
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		//glGenBuffers(1, &m_IndexBuffer); // 控制顶点绘制index顺序
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		Aspect::Ref<Aspect::IndexBuffer> indexBuffer;
		uint32_t indices[3] = { 0,1,2 };
		indexBuffer = (Aspect::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		//======================================
		m_SquareVA = (Aspect::VertexArray::Create());

		// 也可以根据图像的rgb值来判断图形坐标是否正确对应
		float squareVertices[5 * 4] = {
			-0.5f,-0.5f,0.0f,  0.0f,0.0f,
			 0.5f,-0.5f,0.0f,  1.0f,0.0f,
			 0.5f, 0.5f,0.0f,  1.0f,1.0f,
			-0.5f, 0.5f,0.0f,  0.0f,1.0f
		};
		Aspect::Ref<Aspect::VertexBuffer> squareVB;
		squareVB = (Aspect::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		// layout 本身是对数组布局的设置，这里直接抽象出来，方便直接从数组里映射相应的属性
		squareVB->SetLayout({
			{Aspect::ShaderDataType::Float3,"a_Position"},
			{Aspect::ShaderDataType::Float2,"a_TexCoord"}
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2,2,3,0 };
		Aspect::Ref<Aspect::IndexBuffer> squareIB;
		squareIB = (Aspect::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		// R代表原始字符串，括号内不会被编译器忽略，且不会被转义
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

		/*@point shared_ptr的reset函数
		* 1. reset用来重置调用指针的指向
		*    这里因为底层采用make_shared的方式初始化，所以不能用reset;直接等于号赋值就行
		* 2. 采取两种创建shader(读取shader)的方式
		*    使用了ShaderLibrary库，可以根据路径读取glsl文件创建shader，也可以传参name,vert,frag创建shader
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

		// 1. 注意，输入检测放在update里，能够保证每帧都检测，移动流畅；如果在事件里调用，长按会被检测为一次，也即只移动一次。
		//    从物体控制移动的角度来看，update更流畅；另一方面，事件只会调用一次，更可控。
		// 2. 引入Timestep,类似于Unity中Time.deltaTime 因为电脑时而m帧，时而n帧，拿移动举例，update每帧调用一次，但是由于帧数不稳定，
		//    导致每一帧时间不一样，但是每一帧移动距离一样，也即移动相同距离却花了不同时间，移动速度不符合要求！
		//    Timestep的好处是实时计算当前帧的deltaTime,这样直接作为权重乘以速度，就能保证就算帧数不稳定，也能有稳定的移动结果。
		// 3. OpenGL是右手坐标系

		/* Move Square
		if (Aspect::Input::IsKeyPressed(Aspect::Key::L))
			m_SquarePosition.x += m_SquareSpeed * ts;  // 乘以timestep,

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

		// 相机会自行update 设置位置和旋转
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

		// 从ShaderLibrary中Get指定shader
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

	Aspect::OrthographicCameraController m_CameraController; // 使用controller来控制相机

	glm::vec3 m_SquarePosition;
	float m_SquareSpeed = 1.0f;

	glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};
