#pragma once
#include "Aspect/Core/Ref.h"
namespace Aspect
{
	class GraphicsContext : public RefCounted
	{
	public:
		//virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		// TODO: Window pointer type
		static Ref<GraphicsContext> Create(void* window);

	};
}
