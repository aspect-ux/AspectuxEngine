#pragma once
#include "Aspect/Debug/Instrumentor.h"

#ifdef AS_PLATFORM_WINDOWS
	
//extern Aspect::Application* Aspect::CreateApplication();
extern Aspect::Application* Aspect::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc,char** argv) {
	Aspect::Log::Init();
	AS_CORE_WARN("Hello Aspect Engine");

	// create
	AS_PROFILE_BEGIN_SESSION("Startup", "AspectProfile-Startup.json");
	//auto app = Aspect::CreateApplication();
	auto app = Aspect::CreateApplication({ argc, argv });
	AS_PROFILE_END_SESSION();

	// runtime
	AS_PROFILE_BEGIN_SESSION("Startup", "AspectProfile-Runtime.json");
	app->Run();
	AS_PROFILE_END_SESSION();
	
	// release
	AS_PROFILE_BEGIN_SESSION("Startup", "AspectProfile-Runtime.json");
	delete app;
	AS_PROFILE_END_SESSION();
	
}

#endif // AS_PLATFORM_WINDOWS
