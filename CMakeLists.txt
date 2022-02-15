cmake_minimum_required(VERSION 3.15)

project(glub VERSION 1.0.2 DESCRIPTION "Easy to use CMake boilerplate for OpenGL development in C++" LANGUAGES CXX)

find_package(Git)

if(GIT_FOUND)
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
		message(STATUS "Initializing git repository...")
		execute_process(COMMAND ${GIT_EXECUTABLE} init WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} RESULT_VARIABLE GIT_INIT_RESULT)

		if(NOT GIT_INIT_RESULT EQUAL "0")
			message(WARNING "Unable to update git submodules, please update them manually.")
		endif()
	endif()

	message(STATUS "Updating git submodules...")

	set(SUBMODULES lib/glad;lib/freetype;lib/glfw;lib/imgui;lib/stb;lib/glm;)
	set(REPOSITORIES https://github.com/Dav1dde/glad.git;git://git.sv.nongnu.org/freetype/freetype2.git;https://github.com/glfw/glfw.git;https://github.com/ocornut/imgui.git;https://github.com/nothings/stb.git;https://github.com/g-truc/glm.git;)

	foreach(UPD_SUB IN LISTS SUBMODULES)
		message(STATUS "Updating ${UPD_SUB}...")

		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote -- ${UPD_SUB} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} RESULT_VARIABLE GIT_SUBMOD_RESULT)

		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			list(FIND SUBMODULES ${UPD_SUB} SUB_INDEX)
			list(GET REPOSITORIES ${SUB_INDEX} SUB_URL)

			execute_process(COMMAND ${GIT_EXECUTABLE} submodule add ${SUB_URL} ${UPD_SUB} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
			execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${UPD_SUB} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} RESULT_VARIABLE GIT_SUBMOD_RESULT)

			if(NOT GIT_SUBMOD_RESULT EQUAL "0")
				message(WARNING "Unable to update submodule ${UPD_SUB}")
			endif()
		endif()
	endforeach()
else()
	message(WARNING "Unable to update git submodules, please update them manually.")
endif()

set(LIB_GLAD ON)
set(LIB_Freetype ON)
set(LIB_GLFW ON)
set(LIB_Imgui ON)
set(LIB_STB ON)
set(LIB_glm ON)

add_compile_definitions(LIB_GLAD)
add_compile_definitions(LIB_FREETYPE)
add_compile_definitions(LIB_GLFW)
add_compile_definitions(LIB_IMGUI)
add_compile_definitions(LIB_STB)
add_compile_definitions(LIB_GLM)

if(UNIX)
	add_compile_definitions(UNIX)
elseif(WIN32)
	add_compile_definitions(WINDOWS)
else()
	message(FATAL_ERROR "Detected platform is not supported!")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

message(STATUS "Setting up GLAD...")
add_subdirectory(lib/glad)
include_directories(${CMAKE_BINARY_DIR}lib/glad/include)

message(STATUS "Setting up Freetype...")
add_subdirectory(lib/freetype)
include_directories(lib/freetype/include)

message(STATUS "Setting up GLFW...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

message(STATUS "Setting up Imgui...")
include_directories(lib/imgui lib/imgui/backends)
if(LIB_GLFW)
	file(GLOB IMGUI_FILES "./lib/imgui/*.h" "./lib/imgui/*.cpp" "./lib/imgui/backends/*glfw*" "./lib/imgui/backends/*opengl3*")
elseif(LIB_SDL)
	file(GLOB IMGUI_FILES "./lib/imgui/*.h" "./lib/imgui/*.cpp" "./lib/imgui/backends/*sdl*" "./lib/imgui/backends/*opengl3*")
endif()

if(LIB_GLAD)
	set(IMGUI_COMPILE_OPTIONS "-DIMGUI_IMPL_OPENGL_LOADER_GLAD")
else()
	set(IMGUI_COMPILE_OPTIONS "")
endif()

message(STATUS "Setting up STB...")
include_directories(lib/stb)

message(STATUS "Setting up glm...")
include_directories(lib/glm/glm)

if(EXISTS ${PROJECT_SOURCE_DIR}/res/)
	message(STATUS "Copying resources...")
	file(COPY ${PROJECT_SOURCE_DIR}/res/ DESTINATION ${CMAKE_BINARY_DIR}/res/)
endif()

message(STATUS "Setting up build options...")
file(GLOB_RECURSE SRC_FILES "./src/*.h" "./src/*.cpp")
add_executable(glub ${SRC_FILES} ${IMGUI_FILES})

if(UNIX)
	target_compile_options(glub PUBLIC -Wall -Wextra -pedantic -lX11 -lGL ${IMGUI_COMPILE_OPTIONS})
elseif(WIN32)
	target_compile_options(glub PUBLIC /Wall)
endif()

message(STATUS "Linking...")
find_package(OpenGL REQUIRED)
target_link_libraries(glub OpenGL::GL)
target_link_libraries(glub glad ${CMAKE_DL_LIBS})
target_link_libraries(glub freetype)
target_link_libraries(glub glfw)
