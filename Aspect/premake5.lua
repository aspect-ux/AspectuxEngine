project "Aspect"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    -- On:代码生成的运行库选项是MTD,静态链接MSVCRT.lib库;
    -- Off:代码生成的运行库选项是MDD,动态链接MSVCRT.dll库;打包后的exe放到另一台电脑上若无这个dll会报错
    staticruntime "off" --TODO: to be fixed

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "aspch.h"
    pchsource "src/aspch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/stb_image/**.h",
		"vendor/tinyobjloader/tinyobjloader.h",
        "vendor/stb_image/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl",

        "vendor/ImGuizmo/ImGuizmo.h",
        "vendor/ImGuizmo/ImGuizmo.cpp"
    }

    defines
	{
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "src",
        "vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
		--"%{IncludeDir.DirectXTex}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
		"%{IncludeDir.VulkanMemoryAllocator}",
		 "%{IncludeDir.tinyobjloader}",
		"%{IncludeDir.choc}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.Box2D}",
        "%{IncludeDir.mono}",
        "%{IncludeDir.msdf_atlas_gen}",
        "%{IncludeDir.msdfgen}",
        "%{IncludeDir.filewatch}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.Optick}",
        "%{IncludeDir.PhysX}",
		"%{IncludeDir.PhysX}/PhysX",
		"%{IncludeDir.Bullet3}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
		--"DirectXTex",
        "yaml-cpp",
        "msdf-atlas-gen",
        "opengl32.lib",
        "Box2D",
        "Optick",
        "%{Library.mono}",
        "%{Library.PhysX}",
		"%{Library.PhysXCharacterKinematic}",
		"%{Library.PhysXCommon}",
		"%{Library.PhysXCooking}",
		"%{Library.PhysXExtensions}",
		"%{Library.PhysXFoundation}",
		"%{Library.PhysXPvd}",

		--Bullet3 part
		--"Bullet3Collision", "Bullet3Common", "Bullet3Dynamics", "Bullet3Geometry", "BulletCollision", "BulletDynamics", "BulletSoftBody", "LinearMath"
		"%{Library.Bullet3Collision}",
		"%{Library.Bullet3Common}",
		"%{Library.Bullet3Dynamics}",
		"%{Library.Bullet3Geometry}",
		"%{Library.BulletCollision}",
		"%{Library.BulletDynamics}",
		"%{Library.BulletSoftBody}",
		"%{Library.LinearMath}",
    }

    defines
	{
		"PX_PHYSX_STATIC_LIB", "GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

    -- imguizmo 
    filter "files:vendor/ImGuizmo/**.cpp"
    flags { "NoPCH" }

    -- 如果系统是windows
    filter "system:windows"
        --staticruntime "On"
        -- windows SDK 版本
        systemversion "latest"

        links{
            "%{Library.WinSock}",
            "%{Library.WinMM}",
            "%{Library.WinVersion}",
            "%{Library.BCrypt}"
        }

        defines
        {
        }

        --postbuildcommands
        --{ --动态链接不需要copy dll了
        --	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        --}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

        links{
            "%{Library.ShaderC_Debug}",
            "%{Library.SPIRV_Cross_Debug}",
            "%{Library.SPIRV_Cross_GLSL_Debug}",
            "%{Library.Assimp_Debug}"
        }

        defines 
		{
			"AS_DEBUG",
			"AS_TRACK_MEMORY"
		}

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

        links{
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}",
            "%{Library.Assimp_Release}"
        }

        defines
		{
			"AS_RELEASE",
			"AS_TRACK_MEMORY",
			"NDEBUG" -- PhysX Requires This
		}

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        links{
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}",
            "%{Library.Assimp_Release}"
        }

        defines
		{
			"AS_DIST",
			"NDEBUG" -- PhysX Requires This
		}
