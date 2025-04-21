workspace("coursework-2-test")
	configurations { "Debug", "Release" }
	platforms "x64"
	system "Windows"
	architecture "x86_64"

	defines { "SFML_STATIC" }
	libdirs { "dependencies/SFML-3.0.0/lib"	}
	location "build"

project "imgui-test"
	kind "StaticLib"
	language "C++"
	location "build/dependencies/imgui"

	files {
		"**.cpp",
		"**.h"
	}

	filter "configurations:Release"
		optimize "On"

	filter "configurations:Debug"
		symbols "On"
		defines { "DEBUG" }

project "coursework-2-testproj"
	kind "ConsoleApp"
	language "C++"
	
	files {
		"**.cpp",
		"**.h"
	}

	links {
		"gdi32.lib",
		"winmm.lib",
		"opengl32.lib"
	}

	pchheader "src/pch.h"
	pchsource "src/pch.cpp"

	filter "configurations:Release"
		links {
			"sfml-graphics-s.lib",
			"sfml-system-s.lib",
			"sfml-window-s.lib"
		}
		optimize "On"

	filter "configurations:Debug"
		links {
			"sfml-graphics-s-d.lib",
			"sfml-system-s-d.lib",
			"sfml-window-s-d.lib"
		}
		symbols "On"
		defines { "DEBUG" }

	
