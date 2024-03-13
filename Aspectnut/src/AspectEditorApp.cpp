#include <iostream>

#include <Aspect.h>

#include "Aspect/Platform/OpenGL/OpenGLShader.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Aspect/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Aspect
{
	class Aspectnut : public Application
	{
	public:
		Aspectnut(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}

		~Aspectnut()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		Aspect::ApplicationSpecification specification;
		specification.Name = "Aspectnut";
		specification.WindowWidth = 1600;
		specification.WindowHeight = 900;
		specification.CommandLineArgs = args;
		//specification.StartMaximized = true;
		//specification.VSync = true;
		//specification.RenderConfig.ShaderPackPath = "Resources/ShaderPack.hsp";

		specification.ScriptConfig.CoreAssemblyPath = "Resources/Scripts/Aspect-ScriptCore.dll";
		specification.ScriptConfig.EnableDebugging = true;
		specification.ScriptConfig.EnableProfiling = true;

		//specification.CoreThreadingPolicy = ThreadingPolicy::SingleThreaded;

		return new Aspectnut(specification);
	}

}

