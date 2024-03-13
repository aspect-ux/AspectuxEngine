#pragma once

#include "Aspect/Core/Window.h"

#include "Aspect/Renderer/GraphicsContext.h"

//#include "Aspect/Platform/DirectX11/DX11InputListener.h"

#include <GLFW/glfw3.h>

//struct GLFWwindow;

namespace Aspect
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		inline virtual Ref<GraphicsContext> GetGraphicsContext() const override { return m_Context; };
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;

		Ref<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

		//============DX11 PART============
	public:
		virtual void SetHWND(HWND hwnd) override;
		virtual HWND GetHWND() { return m_HWND; }
	private:
		HWND m_HWND; // windef.h
	};

}
