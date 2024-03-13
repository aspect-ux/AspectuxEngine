#pragma once

#include "Base/Base.h"
#include "Window.h"
#include "Aspect/Events/Event.h"
#include "Aspect/Events/ApplicationEvent.h"
#include "Aspect/Core/LayerStack.h"
#include "Aspect/ImGui/ImGuiLayer.h"
#include "Aspect/Script/ScriptEngine.h"

#include "Aspect/Core/Timestep.h"

#include <queue>

int main(int argc, char** argv);

namespace Aspect {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			AS_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Aspect Application";
		uint32_t WindowWidth = 1600, WindowHeight = 900;
		std::string WorkingDirectory;
		bool EnableImGui = true;
		ApplicationCommandLineArgs CommandLineArgs;
		ScriptEngineConfig ScriptConfig;
	};

	class Application
	{
		using EventCallbackFn = std::function<void(Event&)>;
	public:
		Application(const ApplicationSpecification& specification);
		Application(const std::string& name = "Aspect App");
		virtual ~Application();
		
		void AddEventCallback(const EventCallbackFn& eventCallback) { m_EventCallbacks.push_back(eventCallback); }

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		void Close();

		/// Creates & Dispatches an event either immediately, or adds it to an event queue which will be proccessed at the end of each frame
		template<typename TEvent, bool DispatchImmediately = false, typename... TEventArgs>
		void DispatchEvent(TEventArgs&&... args)
		{
			static_assert(std::is_assignable_v<Event, TEvent>);

			std::shared_ptr<TEvent> event = std::make_shared<TEvent>(std::forward<TEventArgs>(args)...);
			if constexpr (DispatchImmediately)
			{
				OnEvent(*event);
			}
			else
			{
				std::scoped_lock<std::mutex> lock(m_EventQueueMutex);
				m_EventQueue.push([event]() { Application::Get().OnEvent(*event); });
			}
		}

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		void SubmitToMainThread(const std::function<void()>& function);
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ExecuteMainThreadQueue();
	private:
		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

		//event
		std::mutex m_EventQueueMutex;
		std::queue<std::function<void()>> m_EventQueue;
		std::vector<EventCallbackFn> m_EventCallbacks;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	protected:
		//inline static bool s_IsRuntime = true;
	};

	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}
