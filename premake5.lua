workspace "DEL-10"
	architecture "x64"
	startproject "DEL-10"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Libraries/GLFW/include"
IncludeDir["Glad"] = "Libraries/Glad/include"
IncludeDir["glm"] = "Libraries/glm"
IncludeDir["ImGui"] = "Libraries/imgui"
IncludeDir["stb"] = "Libraries/stb"
IncludeDir["Assimp"] = "Libraries/assimp/include"

include "Libraries/GLFW"
include "Libraries/Glad"
include "Libraries/imgui"

project "DEL-10"
	location "DEL-10"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	openmp "On"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "dlpch.h"
	pchsource "DEL-10/Core/dlpch.cpp"
	
	
	files
	{
		"%{prj.name}/Core/**.h",
		"%{prj.name}/Core/**.cpp",
		"%{prj.name}/Core/**.hpp",
		"%{prj.name}/Input/**.h",
		"%{prj.name}/Input/**.cpp",
		"%{prj.name}/Input/**.hpp",
		"%{prj.name}/Rendering/**.h",
		"%{prj.name}/Rendering/**.cpp",
		"%{prj.name}/Rendering/**.hpp",
		
		"Libraries/glm/**.hpp",
		"Libraries/glm/**.inl",
		
		"Libraries/stb/**.h",
	}
	
	includedirs
	{
		"%{prj.name}/Core",
		"%{prj.name}/Input",
		"%{prj.name}/Rendering",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.Assimp}",
	}
	
	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"Libraries/assimp/lib/assimp-vc142-mt.lib",
		"opengl32.lib"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"