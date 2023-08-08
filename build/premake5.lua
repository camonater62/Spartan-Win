workspace "Spartan"
    configurations { "Debug", "Release" }
    platforms { "Windows", "Linux" }
    targetname "Spartan"

project "Core"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    warnings "Extra"
    cppdialect "C++17"

    filter "platforms:Linux"
        toolset "clang"
        buildoptions { "-march=x86-64-v2", "-mtune=generic", "-pipe" }
        linkoptions { "-flto=thin" }

    files { "../*.h", "../*.cpp" }
    files { "../Tests/*.h", "../Tests/*.cpp" }
    includedirs { "..", "../Tests" }

    -- STB
    files { "../vendor/stb_image/**.h", "../vendor/stb_image/**.cpp" }
    includedirs { "../vendor/stb_image" }

    -- Imgui
    files { "../vendor/imgui/**.h", "../vendor/imgui/*.cpp" } 
    files { "../vendor/imgui/backends/imgui_impl_glfw.*", "../vendor/imgui/backends/imgui_impl_opengl3*" }
    includedirs { "../vendor/imgui", "../vendor/imgui/backends" }

    -- Assimp
    libdirs { "../vendor/assimp/lib" }
    includedirs { "../vendor/assimp/include" }
    filter "platforms:Windows"
        links { "assimp-vc142-mtd" } -- TODO: Check
    filter "platforms:Linux"
        links { "assimp", "zlibstatic", "minizip" }

    -- GLFW
    libdirs { "../vendor/glfw" }
    includedirs { "../vendor/glfw/include" }
    filter "platforms:Windows"
        links { "glfw3" } -- TODO: Check
    filter "platforms:Linux"
        links { "glfw" }

    -- GLEW
    libdirs { "../vendor/glew/" }
    includedirs { "../vendor/glew/include" }
    filter "platforms:Windows"
        links { "glew32s" } -- TODO: Check 
    filter "platforms:Linux"
        links { "GLEW" }

    -- GLM
    includedirs { "../vendor/glm" }

    -- OpenGL
    filter "platforms:Windows"
        links { "opengl32" } -- TODO: Check
    filter "platforms:Linux" 
        links { "GL" }

    -- Jolt
    -- TODO: Handle debug/release
    libdirs { "../vendor/Jolt/lib" }
    includedirs { "../vendor/Jolt/include" }
    filter "platforms:Windows"
        links { "Jolt" } -- TODO: Check
    filter "platforms:Linux"
        links { "Jolt" }
    defines { "JPH_DEBUG_RENDERER", "JPH_PROFILE_ENABLED", "JPH_USE_AVX" }
    defines { "JPH_USE_AVX2", "JPH_USE_F16C", "JPH_USE_FMADD", "JPH_USE_LZCNT" }
    defines { "JPH_USE_SSE4_1", "JPH_USE_SSE4_2", "JPH_USE_TZCNT" }
    buildoptions { "-ffp-model=precise", "-pthread", "-mavx2", "-mbmi", "-mpopcnt", "-mlzcnt", "-mf16c", "-mfma" }

    filter "configurations:Debug"
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "RELEASE" }
        optimize "On"

    -- postbuildcommands {
    --     "{COPY} %{cfg.targetdir}/%{cfg.targetname} .."
    -- }
    