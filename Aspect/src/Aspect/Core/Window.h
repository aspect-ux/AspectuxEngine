#pragma once

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Events/Event.h"
#include "Aspect/Renderer/GraphicsContext.h"

#include <sstream>

namespace Aspect
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Aspect Engine",
				uint32_t width = 1600,
				uint32_t height = 900)
			: Title(title),Width(width),Height(height) {}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual Ref<GraphicsContext> GetGraphicsContext() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());

	//========= DX11 PART=============
	public:
		virtual void SetHWND(HWND hwnd) = 0;
		virtual HWND GetHWND() = 0;
	
	};
}
