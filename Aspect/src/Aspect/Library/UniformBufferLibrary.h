#pragma once 

#include "Aspect/Renderer/UniformBuffer.h"
#include "Aspect/Library/Library.h"

#include <glm/glm.hpp>

namespace Aspect
{
	struct CameraData
	{
		glm::mat4 ViewProjection;
	};

	template<>
	class Library<UniformBuffer> : public LibraryBase<Library, UniformBuffer>
	{
	public:
		Library();

		[[nodiscard]] Ref<UniformBuffer> GetCameraUniformBuffer();
	};
}
