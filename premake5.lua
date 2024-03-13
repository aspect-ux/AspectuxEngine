include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Aspect"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	startproject "Aspectnut"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

group "Dependencies"
	include "vendor/premake"
	include "Aspect/vendor/Box2D"
	include "Aspect/vendor/GLFW"
	include "Aspect/vendor/Glad"
	include "Aspect/vendor/imgui"
	include "Aspect/vendor/yaml-cpp"
	include "Aspect/vendor/msdf-atlas-gen"
	include "Aspect/vendor/Optick"
group ""

group "Core"
	include "Aspect"
	include "Aspect-ScriptCore"
group ""

group "Tools"
	include "Aspectnut"
group ""

group "Misc"
	include "Sandbox"
group ""



