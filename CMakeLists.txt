cmake_minimum_required(VERSION 3.20)

project(ComputerGraphics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_executable(ComputerGraphics
    src/main.cpp
    src/logging.cpp
)

target_include_directories(ComputerGraphics
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ComputerGraphics
    PRIVATE
        glfw
        GLEW::GLEW
        OpenGL::GL
)

file(GLOB SHADERS
    "${CMAKE_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_SOURCE_DIR}/shaders/*.frag"
)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:ComputerGraphics>/shaders
)

add_dependencies(ComputerGraphics copy_shaders)