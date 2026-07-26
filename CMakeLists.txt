cmake_minimum_required(VERSION 3.20)

project(Minecraft
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# macOS OpenGL is deprecated but still supported
if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()


# -------------------------
# Source files
# -------------------------

file(GLOB_RECURSE SOURCES
    Minecraft/src/*.cpp
)

file(GLOB_RECURSE HEADERS
    Minecraft/includes/*.h
)


# -------------------------
# Vendor files
# -------------------------

set(VENDOR_SOURCES
    Minecraft/vendor/glad.c

    Minecraft/vendor/imgui/imgui.cpp
    Minecraft/vendor/imgui/imgui_demo.cpp
    Minecraft/vendor/imgui/imgui_draw.cpp
    Minecraft/vendor/imgui/imgui_tables.cpp
    Minecraft/vendor/imgui/imgui_widgets.cpp
    Minecraft/vendor/imgui/imgui_impl_glfw.cpp
    Minecraft/vendor/imgui/imgui_impl_opengl3.cpp
)


# -------------------------
# Executable
# -------------------------

add_executable(Minecraft
    ${SOURCES}
    ${HEADERS}
    ${VENDOR_SOURCES}
)


# -------------------------
# Include directories
# Matches old .vcxproj:
# Libraries/include
# vendor
# includes
# -------------------------

target_include_directories(Minecraft
PRIVATE
    Minecraft/includes
    Minecraft/vendor
    Minecraft/Libraries/include
)


# -------------------------
# External libraries
# -------------------------

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)


target_link_libraries(Minecraft
PRIVATE
    OpenGL::GL
    glfw
    assimp::assimp
)


# -------------------------
# Compiler warnings
# -------------------------

if(MSVC)
    target_compile_options(Minecraft PRIVATE
        /W4
    )
else()
    target_compile_options(Minecraft PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()


# -------------------------
# Resources
# -------------------------

add_custom_command(TARGET Minecraft POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Minecraft/Resources
    $<TARGET_FILE_DIR:Minecraft>/Resources
)


# -------------------------
# Output directories
# -------------------------

set_target_properties(Minecraft PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
