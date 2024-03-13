#include "aspch.h"
#include "Aspect/Core/Window.h"

#ifdef AS_PLATFORM_WINDOWS
	#include "Aspect/Platform/Windows/WindowsWindow.h"
#endif

namespace Aspect
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef AS_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
	#else
		AS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}

}
