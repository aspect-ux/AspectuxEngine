
-- 项目Dependencies
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["magic_enum"] = "%{wks.location}/Aspect/vendor/magic_enum/include"
IncludeDir["Assimp"] = "%{wks.location}/Aspect/vendor/assimp/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["GLFW"] = "%{wks.location}/Aspect/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Aspect/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Aspect/vendor/imgui"
--IncludeDir["DirectXTex"] = "%{wks.location}/Aspect/vendor/DirectXTex"
IncludeDir["filewatch"] = "%{wks.location}/Aspect/vendor/filewatch"
IncludeDir["VulkanMemoryAllocator"] = "%{wks.location}/Aspect/vendor/VulkanMemoryAllocator"
IncludeDir["glm"] = "%{wks.location}/Aspect/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Aspect/vendor/stb_image"
IncludeDir["tinyobjloader"] = "%{wks.location}/Aspect/vendor/tinyobjloader"
IncludeDir["choc"] = "%{wks.location}/Aspect/vendor/choc"
IncludeDir["entt"] = "%{wks.location}/Aspect/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Aspect/vendor/yaml-cpp/include" -- 用yaml_cpp下划线是因为"%{IncludeDir.yaml_cpp}"只认识_ 不认识-
IncludeDir["Box2D"] = "%{wks.location}/Aspect/vendor/Box2D/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Aspect/vendor/ImGuizmo" 
IncludeDir["msdfgen"] = "%{wks.location}/Aspect/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/Aspect/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["Optick"] = "%{wks.location}/Aspect/vendor/Optick/src"
IncludeDir["PhysX"] = "%{wks.location}/Aspect/vendor/PhysX/include"
IncludeDir["Bullet3"] = "%{wks.location}/Aspect/vendor/bullet3/include/bullet"

IncludeDir["mono"] = "%{wks.location}/Aspect/vendor/mono/include" -- 手动生成

-- Library's directory
LibraryDir = {}
-- %{wks.location}获取当前项目.sln的路径
-- %{cfg.buildcfg}表示当前编译目标是Debug还是Release
LibraryDir["PhysX"] = "%{wks.location}/Aspect/vendor/PhysX/lib/%{cfg.buildcfg}"
LibraryDir["Bullet3"] = "%{wks.location}/Aspect/vendor/bullet3/lib/%{cfg.buildcfg}"
LibraryDir["mono"] = "%{wks.location}/Aspect/vendor/mono/lib/%{cfg.buildcfg}"

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

-- Concrete Library
Library = {}

Library["Assimp_Debug"] = "%{wks.location}/Aspect/vendor/assimp/bin/Debug/assimp-vc143-mtd.lib"
Library["Assimp_Release"] = "%{wks.location}/Aspect/vendor/assimp/bin/Release/assimp-vc143-mt.lib"
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib" -- 主要是这个

Library["PhysX"] = "%{LibraryDir.PhysX}/PhysX_static_64.lib"
Library["PhysXCharacterKinematic"] = "%{LibraryDir.PhysX}/PhysXCharacterKinematic_static_64.lib"
Library["PhysXCommon"] = "%{LibraryDir.PhysX}/PhysXCommon_static_64.lib"
Library["PhysXCooking"] = "%{LibraryDir.PhysX}/PhysXCooking_static_64.lib"
Library["PhysXExtensions"] = "%{LibraryDir.PhysX}/PhysXExtensions_static_64.lib"
Library["PhysXFoundation"] = "%{LibraryDir.PhysX}/PhysXFoundation_static_64.lib"
Library["PhysXPvd"] = "%{LibraryDir.PhysX}/PhysXPvdSDK_static_64.lib"

--bullet3
--"Bullet3Collision", "Bullet3Common", "Bullet3Dynamics", "Bullet3Geometry", "BulletCollision", "BulletDynamics", "BulletSoftBody", "LinearMath"
Library["Bullet3Collision"] = "%{LibraryDir.Bullet3}/Bullet3Collision.lib"
Library["Bullet3Common"] = "%{LibraryDir.Bullet3}/Bullet3Common.lib"
Library["Bullet3Dynamics"] = "%{LibraryDir.Bullet3}/Bullet3Dynamics.lib"
Library["Bullet3Geometry"] = "%{LibraryDir.Bullet3}/Bullet3Geometry.lib"
Library["BulletCollision"] = "%{LibraryDir.Bullet3}/BulletCollision.lib"
Library["BulletDynamics"] = "%{LibraryDir.Bullet3}/BulletDynamics.lib"
Library["BulletSoftBody"] = "%{LibraryDir.Bullet3}/BulletSoftBody.lib"
Library["LinearMath"] = "%{LibraryDir.Bullet3}/LinearMath.lib"


Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"


Binaries = {}
Binaries["Assimp_Debug"] = "%{wks.location}/Aspect/vendor/assimp/bin/Debug/assimp-vc143-mtd.dll"
Binaries["Assimp_Release"] = "%{wks.location}/Aspect/vendor/assimp/bin/Release/assimp-vc143-mt.dll"
