workspace "doors"
    configurations { "debug", "release" }

project "doors"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    toolset "clang"

    files {
        "src/*.cpp"
    }

    includedirs {
        "src",
        "/opt/homebrew/Cellar/raylib/4.5.0/include"
    }

    libdirs {
        "/opt/homebrew/Cellar/raylib/4.5.0/lib"
    }

    links {
        "raylib"
    }

    buildoptions {
        "-Wall",
        "-Wpedantic",
        "-Wextra",
        "-Werror",
        
        "-Wno-missing-field-initializers"
    }

    filter "configurations:debug"
        defines { "DEBUG" }
        targetdir "bin/debug"
        symbols "On"
        optimize "Debug"
    
    filter "configurations:release"
        targetdir "bin/release"
        optimize "Full"
