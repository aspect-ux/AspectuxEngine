#pragma once

#include "Aspect/Renderer/Shader.h"
#include "Aspect/Library/Library.h"

namespace Aspect
{
	template<>
	class Library<Shader> : public LibraryBase<Library, Shader>
	{
	public:
		Library();

		[[nodiscard]] Ref<Shader> GetDefaultShader();
		[[nodiscard]] Ref<Shader> GetSkyBoxShader();
	};
}
