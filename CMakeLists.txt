cmake_minimum_required(VERSION 3.20)
project(StructForce VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

find_package(OpenGL REQUIRED)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

set(APP_SOURCES
    src/main.cpp
    src/data.cpp
    src/logic.cpp
    src/presentation.cpp
    src/views_contacts.cpp
    src/views_forms.cpp
    src/views_sidebar.cpp
    src/logo.cpp
    src/auth.cpp
    src/views_auth.cpp
)

# Download stb_image (single-header image loader)
set(STB_DIR "${CMAKE_BINARY_DIR}/stb")
file(MAKE_DIRECTORY "${STB_DIR}")
if(NOT EXISTS "${STB_DIR}/stb_image.h")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
        "${STB_DIR}/stb_image.h"
    )
endif()

# Copy logos to build directory so the executable can find them at runtime
configure_file(
    "${CMAKE_SOURCE_DIR}/../StructForce.png"
    "${CMAKE_BINARY_DIR}/logo_light.png"
    COPYONLY
)
configure_file(
    "${CMAKE_SOURCE_DIR}/../Struct.png"
    "${CMAKE_BINARY_DIR}/logo_dark.png"
    COPYONLY
)

add_executable(StructForce ${APP_SOURCES} ${IMGUI_SOURCES})

target_compile_definitions(StructForce PRIVATE
    APP_FONT_PATH="${imgui_SOURCE_DIR}/misc/fonts/Roboto-Medium.ttf"
    LOGO_LIGHT_PATH="${CMAKE_BINARY_DIR}/logo_light.png"
    LOGO_DARK_PATH="${CMAKE_BINARY_DIR}/logo_dark.png"
)

target_include_directories(StructForce PRIVATE "${STB_DIR}")

target_include_directories(StructForce PRIVATE
    include/
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(StructForce PRIVATE
    glfw
    OpenGL::GL
)

if(APPLE)
    target_link_libraries(StructForce PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
    target_compile_definitions(StructForce PRIVATE GL_SILENCE_DEPRECATION)
endif()

if(MSVC)
    target_compile_options(StructForce PRIVATE /MP)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

