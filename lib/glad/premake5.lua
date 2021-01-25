-- premake5.lua
project "glad"
    kind "StaticLib"
    language "C"

	staticruntime "On"
    
    targetdir ("%{wks.location}/bin/" .. output_dir .. "/%{prj.name}")
    objdir    ("%{wks.location}/obj/" .. output_dir .. "/%{prj.name}")

    files {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs { "include" }
    
    filter "system:windows"
        systemversion "latest"

    filter "system:linux"
        systemversion "latest"

    filter "configurations:debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:release"
        runtime "Release"
        optimize "On"
