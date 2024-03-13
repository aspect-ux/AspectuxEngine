#include "aspch.h"

#include "Aspect/Library/Library.h"
#include "Aspect/Asset/AssetManager.h"
#include "Aspect/Renderer/Mesh.h"
#include "Aspect/Renderer/Texture.h"
#include "Aspect/Library/UniformBufferLibrary.h"

namespace Aspect
{
	template<typename LibType>
	Library<LibType>::Library()
	{
	}

	template<>
	Library<Mesh>::Library()
	{
		Add("Box", CreateRef<Mesh>("Resources/Test/Models/Box.obj"));
		Add("AnbiModel", CreateRef<Mesh>("Resources/Models/Anbi_Face.obj"));
	}

	template<>
	Library<CubeMapTexture>::Library()
	{
		std::vector<std::string> Paths{
			"Resources/Test/Textures/Skybox/right.jpg",
			"Resources/Test/Textures/Skybox/left.jpg",
			"Resources/Test/Textures/Skybox/top.jpg",
			"Resources/Test/Textures/Skybox/bottom.jpg",
			"Resources/Test/Textures/Skybox/front.jpg",
			"Resources/Test/Textures/Skybox/back.jpg",
		};
		Add("SkyBoxTexture", CubeMapTexture::Create(Paths));

		std::vector<std::string> blackPaths{
			"Resources/Test/Textures/black.png",
			"Resources/Test/Textures/black.png",
			"Resources/Test/Textures/black.png",
			"Resources/Test/Textures/black.png",
			"Resources/Test/Textures/black.png",
			"Resources/Test/Textures/black.png",
		};
		Add("BlackCubeMap", CubeMapTexture::Create(blackPaths));

		Add("EnvironmentHdr", CubeMapTexture::Create(512, 512));
		Add("EnvironmentIrradiance", CubeMapTexture::Create(32, 32));
		Add("EnvironmentPrefilter", CubeMapTexture::Create(128, 128));
	}
}
