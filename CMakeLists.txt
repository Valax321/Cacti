cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

include(GitVersionInfo)
include(GatherSources)
include(GenerateExportHeader)

get_version_from_git(CACTI_VERSION_STRING CACTI_COMMIT_COUNT CACTI_COMMIT_HASH)
get_branch_from_git(CACTI_GIT_BRANCH)

project(Cacti VERSION "${CACTI_VERSION_STRING}" LANGUAGES CXX)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(CACTI_IS_SUBMODULE off)
else()
    set(CACTI_IS_SUBMODULE on)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(CACTI_ENABLE_ASSERTIONS "Enable Cacti Assertions" OFF)
option(CACTI_ENABLE_FREETYPE "Enable Freetype in Imgui" OFF)
option(CACTI_ENABLE_EXAMPLES "Enable Example Programs" ${CACTI_IS_SUBMODULE})
option(CACTI_FORCE_OPENGL_BACKEND "Force OpenGL backend on Windows/macOS" OFF)

if (NOT CACTI_IS_SUBMODULE)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(Freetype REQUIRED)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT CACTI_FORCE_OPENGL_BACKEND)
    set(CACTI_BACKEND_D3D ON)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND NOT CACTI_FORCE_OPENGL_BACKEND)
    set(CACTI_BACKEND_METAL ON)
    enable_language(OBJCXX)
else()
    set(CACTI_BACKEND_OPENGL ON)
endif()

gather_sources("include" CACTI_HEADERS)
gather_sources("src" CACTI_SRCS)

set(CACTI_IMGUI_SRCS
"imgui_demo.cpp"
"imgui_draw.cpp"
"imgui_internal.h"
"imgui_tables.cpp"
"imgui_widgets.cpp"
"imgui.cpp"
"imgui.h"
"imstb_rectpack.h"
"imstb_textedit.h"
"imstb_truetype.h"
"misc/cpp/imgui_stdlib.cpp"
"misc/cpp/imgui_stdlib.h"
"backends/imgui_impl_sdl2.cpp"
"backends/imgui_impl_sdl2.h"
)

if (CACTI_ENABLE_FREETYPE)
    list(APPEND CACTI_IMGUI_SRCS 
    "misc/freetype/imgui_freetype.cpp"
    "misc/freetype/imgui_freetype.h"
    )
endif()

if (CACTI_BACKEND_D3D)
    list(APPEND CACTI_IMGUI_SRCS 
    "backends/imgui_impl_dx11.cpp"
    "backends/imgui_impl_dx11.h"
    )
elseif(CACTI_BACKEND_METAL)
    list(APPEND CACTI_IMGUI_SRCS 
    "backends/imgui_impl_metal.mm"
    "backends/imgui_impl_metal.h"
    )
elseif(CACTI_BACKEND_OPENGL)
    list(APPEND CACTI_IMGUI_SRCS 
    "backends/imgui_impl_opengl3.cpp"
    "backends/imgui_impl_opengl3.h"
    "backends/imgui_impl_opengl3_loader.h"
    )
else()
    message(FATAL_ERROR "Did not detect which backend to use")
endif()

list(TRANSFORM CACTI_IMGUI_SRCS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/")

add_library(cacti STATIC ${CACTI_SRCS} ${CACTI_HEADERS} ${CACTI_IMGUI_SRCS})
add_library(Cacti::Cacti ALIAS cacti)
generate_export_header(cacti EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/include/cacti/api.hpp")
target_include_directories(cacti PUBLIC "include" "${CMAKE_CURRENT_BINARY_DIR}/include" "thirdparty/imgui" PRIVATE "src" "thirdparty/imgui/backends")
target_link_libraries(cacti SDL2::SDL2)
target_compile_definitions(cacti PUBLIC "IMGUI_USER_CONFIG=\"cacti/imconfig_cacti.hpp\"")
if (CACTI_ENABLE_FREETYPE)
    target_link_libraries(cacti Freetype::Freetype)
endif()
if (CACTI_BACKEND_METAL)
    target_include_directories(cacti PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/metal-cpp")
endif()

if (CACTI_ENABLE_EXAMPLES)
    add_subdirectory("examples")
endif()
