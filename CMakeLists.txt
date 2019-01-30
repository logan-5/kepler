cmake_minimum_required(VERSION 3.11)

project(kepler)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(INCLUDE_DIRS ${INCLUDE_DIRS} ${SRC})
set(SRC ${SRC} ${SRC_DIR}/behavior.cpp)
set(SRC ${SRC} ${SRC_DIR}/buffer.cpp)
set(SRC ${SRC} ${SRC_DIR}/camera.cpp)
set(SRC ${SRC} ${SRC_DIR}/frame_buffer.cpp)
set(SRC ${SRC} ${SRC_DIR}/fs.cpp)
set(SRC ${SRC} ${SRC_DIR}/gbuffer.cpp)
set(SRC ${SRC} ${SRC_DIR}/gl.cpp)
set(SRC ${SRC} ${SRC_DIR}/image.cpp)
set(SRC ${SRC} ${SRC_DIR}/input.cpp)
set(SRC ${SRC} ${SRC_DIR}/light.cpp)
set(SRC ${SRC} ${SRC_DIR}/light_data.cpp)
set(SRC ${SRC} ${SRC_DIR}/light_volume_technique.cpp)
set(SRC ${SRC} ${SRC_DIR}/main.cpp)
set(SRC ${SRC} ${SRC_DIR}/material.cpp)
set(SRC ${SRC} ${SRC_DIR}/object.cpp)
set(SRC ${SRC} ${SRC_DIR}/renderer.cpp)
set(SRC ${SRC} ${SRC_DIR}/scene.cpp)
set(SRC ${SRC} ${SRC_DIR}/simple_technique.cpp)
set(SRC ${SRC} ${SRC_DIR}/shader.cpp)
set(SRC ${SRC} ${SRC_DIR}/texture.cpp)
set(SRC ${SRC} ${SRC_DIR}/types.cpp)
set(SRC ${SRC} ${SRC_DIR}/util.cpp)
set(SRC ${SRC} ${SRC_DIR}/vertex_array.cpp)
set(SRC ${SRC} ${SRC_DIR}/window.cpp)

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${GENERATED_DIR})

set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/project_path_template.hpp.in
    ${GENERATED_DIR}/project_path.hpp
    )

# GLFW
set(GLFW_DIR ${LIB_DIR}/glfw)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
set(LINK_LIBS ${LINK_LIBS} glfw ${GLFW_LIBRARIES})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${GLFW_DIR}/include)
set(COMPILE_DEFINITIONS ${COMPILE_DEFS} GLFW_INCLUDE_NONE)
target_compile_options(glfw PRIVATE -Wno-deprecated-declarations) # OpenGL is deprecated on MacOS...

# GLAD
set(GLAD_DIR ${LIB_DIR}/glad)
set(GLAD_INCLUDE ${GLAD_DIR}/include)
add_library(glad ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_INCLUDE})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${GLAD_INCLUDE})
set(LINK_LIBS ${LINK_LIBS} glad)

# GLM
set(GLM_DIR ${LIB_DIR}/glm)
set(GLM_INCLUDE ${LIB_DIR}/glm)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${GLM_INCLUDE})

# STB
set(STB_DIR ${LIB_DIR}/stb)
set(STB_INCLUDE ${STB_DIR})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${STB_INCLUDE})

# validate shaders
add_custom_target(validate_shaders 
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/validate_shaders.py ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# main target
add_executable(${PROJECT_NAME} ${SRC})
add_dependencies(${PROJECT_NAME} validate_shaders)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILE_DEFS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
