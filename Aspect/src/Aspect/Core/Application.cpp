#include "aspch.h"
#include "Application.h"

#include "Log.h"

#include <Glad/glad.h>
#include "Aspect/Core/Input.h"
#include "glm/glm.hpp"

#include "Aspect/Renderer/Renderer.h"

#include "Aspect/Renderer/Buffer.h"
#include "Aspect/Platform/OpenGL/OpenGLBuffer.h"

#include <GLFW/glfw3.h>

namespace Aspect {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1) // _1��������ص��е�һ��������_2���Դ�����


	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		AS_PROFILE_FUNCTION();

		AS_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// Set working directory here
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallback(AS_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		// TODO: virutal class ImGuiLayer,we should Create Correct ImGuiLayer with correct platform
		m_ImGuiLayer = ImGuiLayer::Create();
		PushOverlay(m_ImGuiLayer);

		//ScriptEngine::Init(specification.ScriptConfig);
	}

	//TODO: Delete
	Application::Application(const std::string& name)
	{
		AS_CORE_ASSERT(!s_Instance,"Application already exists!")
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_Window->SetVSync(false); //������������Ϊ0֡

		// ��ʼ����Ⱦ��������blendģʽ
		Renderer::Init();

		// ��ImGui��������
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}
	
	Application::~Application() {

	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		// ����e.Handled + ���¼�
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		//AS_CORE_INFO("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(function);
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Run() {
		AS_PROFILE_FUNCTION();

		while (m_Running)
		{
			AS_PROFILE_SCOPE("RunLoop");

			//glClearColor(0.1f, 0.1f, 0.1f, 1);
			//glClear(GL_COLOR_BUFFER_BIT);
			/* ת�Ƶ�SandBox��
			//======================== ʹ��RenderCommand��Ⱦָ���װ=========================
			RenderCommand::SetClearColor(glm::vec4({ 0.1f, 0.1f, 0.1f, 1.0f }));
			RenderCommand::Clear();

			//m_Camera.SetPosition({ 0.5f,0.5f,0.0f });
			m_Camera.SetRotation(45.0f);

			Renderer::BeginScene(m_Camera);

			//m_BlueShader->Bind();
			//m_BlueShader->UploadUniformMat4("u_ViewProjectionMatrix", m_Camera.GetViewProjectionMatrix());
			Renderer::Submit(m_BlueShader,m_SquareVA);

			//m_Shader->Bind();
			//m_Shader->UploadUniformMat4("u_ViewProjectionMatrix", m_Camera.GetViewProjectionMatrix());
			Renderer::Submit(m_Shader,m_VertexArray);

			Renderer::EndScene();
			//================================================================================
			/* �󶨻���ͼԪ
			m_SquareVA->Bind();
			glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);


			m_Shader->Bind();

			m_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT , nullptr);
			*/

			float time = (float)glfwGetTime(); // ������Ҫ����ƽ̨��ͬ��ȡ
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// ���������С���ˣ���ô���ھͲ��ٽ��������¼���
			ExecuteMainThreadQueue();

			if (!m_Minimized)
			{
				{
					AS_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					AS_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}
}
