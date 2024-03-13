project "Aspectnut"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp", 

		-- Shaders
		"%{prj.name}/Resources/Shaders/**.glsl",
		"%{prj.name}/Resources/Shaders/**.glslh",
		"%{prj.name}/Resources/Shaders/**.hlsl",
		"%{prj.name}/Resources/Shaders/**.hlslh",
		"%{prj.name}/Resources/Shaders/**.slh",
	}

	includedirs
	{
		"%{wks.location}/Aspect/vendor/spdlog/include",
		"%{wks.location}/Aspect/src",
		"%{wks.location}/Aspect/vendor",
		"%{IncludeDir.entt}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.magic_enum}",
		"%{IncludeDir.choc}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Bullet3}",
	}

	--postbuildcommands 
	--{
	--	'{COPY} "../Aspect/vendor/NvidiaAftermath/lib/x64/GFSDK_Aftermath_Lib.x64.dll" "%{cfg.targetdir}"'
	--}

	links
	{
		"Aspect"
	}

	defines 
	{
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "AS_DEBUG"
		runtime "Debug"
		symbols "on"

		includedirs
		{
			"%{IncludeDir.Optick}",
		}

		defines
		{
			"AS_DEBUG",
			"AS_TRACK_MEMORY"
		}

		links
		{
			"%{Library.Assimp_Debug}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Debug}" "%{cfg.targetdir}"',
			'{COPY} "../Aspect/vendor/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		defines "AS_RELEASE"
		runtime "Release"
		optimize "on"

		includedirs
		{
			"%{IncludeDir.Optick}",
		}

		defines
		{
			"AS_RELEASE",
			"AS_TRACK_MEMORY",
			"NDEBUG" -- PhysX Requires This
		}


		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"',
			'{COPY} "../Aspect/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "AS_DIST"
		runtime "Release"
		optimize "on"

		defines
		{
			"AS_DIST",
			"NDEBUG" -- PhysX Requires This
		}

		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"',
			'{COPY} "../Aspect/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}


	filter "files:**.hlsl"
		flags {"ExcludeFromBuild"}
