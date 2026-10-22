cmake_minimum_required(VERSION 4.3)
project(cad_graphics LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(OpenGL REQUIRED)

include(FetchContent)

if (NOT TARGET cad_math::cad_math)
    FetchContent_Declare(
            cad_math
            GIT_REPOSITORY https://github.com/1180779/cad_math.git
            GIT_TAG 5e27d4efa50e939ac53f8325e35ed0badbd158e2
    )
    FetchContent_MakeAvailable(cad_math)
endif ()

add_library(cad_graphics STATIC)
add_library(cad_graphics::cad_graphics ALIAS cad_graphics)

set_target_properties(cad_graphics PROPERTIES AUTOMOC ON AUTORCC ON)
target_compile_features(cad_graphics PUBLIC cxx_std_23)
target_include_directories(cad_graphics PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(cad_graphics
        PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        cad_math::cad_math
)

file(GLOB_RECURSE CAD_GRAPHICS_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include/cad_graphics/*.hxx"
)

file(GLOB_RECURSE CAD_GRAPHICS_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/src/*.cxx"
)

target_sources(cad_graphics PRIVATE ${CAD_GRAPHICS_SOURCES})

target_sources(cad_graphics PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
        FILES ${CAD_GRAPHICS_HEADERS}
)
