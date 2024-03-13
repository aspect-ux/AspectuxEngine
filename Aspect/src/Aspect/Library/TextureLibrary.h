#pragma once 

#include "Aspect/Renderer/Texture.h"
#include "Aspect/Library/Library.h"

namespace Aspect
{
	template<>
	class Library<Texture2D> : public LibraryBase<Library, Texture2D>
	{
	public:
		Library();

		[[nodiscard]] Ref<Texture2D> GetDefaultTexture();
		[[nodiscard]] Ref<Texture2D> GetWhiteTexture();
	};
}
