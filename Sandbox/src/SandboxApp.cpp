#include <Aspect.h>

#include "Sandbox2D.h"

#include <Aspect/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "GameLayer.h"


class Sandbox : public Aspect::Application
{
public:
	Sandbox(const Aspect::ApplicationSpecification& specification)
		: Aspect::Application(specification){
		PushOverlay(new Sandbox2D());
		//PushLayer(new ExampleLayer());
		//PushLayer(new GameLayer());
	}

	~Sandbox()
	{

	}
};

Aspect::Application* Aspect::CreateApplication(Aspect::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Aspectnut";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
