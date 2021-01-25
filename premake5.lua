-- premake5.lua
workspace "opengl_cpp"
	architecture "x86_64"
	configurations {
		"debug", 
		"release"
	}

	flags {
		"MultiProcessorCompile"
	}

output_dir = "%{cfg.buildcfg}-%{cfg.system}"

include_dirs = {}
include_dirs["glfw"]       = "%{wks.location}/lib/glfw/include"
include_dirs["glad"]       = "%{wks.location}/lib/glad/include"
include_dirs["glm"]        = "%{wks.location}/lib/glm"
include_dirs["opengl_cpp"] = "%{wks.location}/src"

include "lib/glfw"
include "lib/glad"

project "opengl_cpp"
	kind "ConsoleApp"
	language   "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. output_dir .. "/%{prj.name}")
	objdir	  ("%{wks.location}/obj/" .. output_dir .. "/%{prj.name}")

	files {
		"%{prj.location}/src/**.hpp",
		"%{prj.location}/src/**.cpp",
		"%{prj.location}/src/**.h", 
		"%{prj.location}/src/**.cc"
	}

	includedirs {
		"%{include_dirs.glfw}",
		"%{include_dirs.glad}",
		"%{include_dirs.glm}",
		"%{include_dirs.opengl_cpp}"
	}

	links {
		"glfw",
		"glad"
	}

	filter "system:windows"
		systemversion "latest"
		links { "OpenGL32" }

	filter "system:linux"
		systemversion "latest"
		links { 
			"dl",	   -- needed for glfw
			"pthread", -- \/
			"m",	   -- \/
			"GL"	   -- needed for opengl
		}

	filter "configurations:debug"
		defines "DEBUG"
		symbols "On"

	filter "configurations:release"
		defines  "RELEASE"
		optimize "On"
