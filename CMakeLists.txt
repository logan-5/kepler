cmake_minimum_required(VERSION 3.11)

project(kepler)
string(TOUPPER ${PROJECT_NAME} PROJECT_UPPER_NAME)
set(PROJECT_VERSION 0)
set(PROJECT_USE_NAMESPACE false)
set(PROJECT_OPENGL_VERSION_MAJOR 3)
set(PROJECT_OPENGL_VERSION_MINOR 3)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" 
    OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
    OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(COMPILER_IS_GCCLIKE TRUE)
endif ()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(INCLUDE_DIRS ${INCLUDE_DIRS} ${SRC})

function(SET_SRC_HPP _NAME)
    set(SRC ${SRC} ${SRC_DIR}/${_NAME}.hpp PARENT_SCOPE)
endfunction()
function(SET_SRC_HPP_CPP _NAME)
    set(SRC ${SRC} ${SRC_DIR}/${_NAME}.cpp ${SRC_DIR}/${_NAME}.hpp PARENT_SCOPE)
endfunction()
function(SET_SRC_FILE _NAME)
    set(SRC ${SRC} ${SRC_DIR}/${_NAME} PARENT_SCOPE)
endfunction()

SET_SRC_HPP(binding)
SET_SRC_HPP(common)
SET_SRC_HPP(cube)
SET_SRC_HPP(deferred_shading_technique)
SET_SRC_HPP(gl_object)
SET_SRC_HPP(invoke_result)
SET_SRC_HPP(lazy)
SET_SRC_HPP(optional)

SET_SRC_HPP_CPP(behavior)
SET_SRC_HPP_CPP(behaviors)
SET_SRC_HPP_CPP(buffer)
SET_SRC_HPP_CPP(camera)
SET_SRC_HPP_CPP(frame_buffer)
SET_SRC_HPP_CPP(fs)
SET_SRC_HPP_CPP(gbuffer)
SET_SRC_HPP_CPP(gl)
SET_SRC_HPP_CPP(image)
SET_SRC_HPP_CPP(input)
SET_SRC_HPP_CPP(light)
SET_SRC_HPP_CPP(light_data)
SET_SRC_HPP_CPP(light_volume_technique)
SET_SRC_HPP_CPP(material)
SET_SRC_HPP_CPP(object)
SET_SRC_HPP_CPP(random)
SET_SRC_HPP_CPP(renderer)
SET_SRC_HPP_CPP(scene)
SET_SRC_HPP_CPP(simple_technique)
SET_SRC_HPP_CPP(shader)
SET_SRC_HPP_CPP(texture)
SET_SRC_HPP_CPP(types)
SET_SRC_HPP_CPP(util)
SET_SRC_HPP_CPP(vertex_array)
SET_SRC_HPP_CPP(window)

SET_SRC_FILE(input.inl)
SET_SRC_FILE(main.cpp)

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${GENERATED_DIR})

set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CONFIG_HEADER_PATH ${GENERATED_DIR}/${PROJECT_NAME}_config.hpp)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/project_config_template.hpp.in
    ${CONFIG_HEADER_PATH}
    )
set(SRC ${SRC} ${CONFIG_HEADER_PATH})

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
if (APPLE)
    if (${COMPILER_IS_GCCLIKE})
        # OpenGL is deprecated on MacOS...
        target_compile_options(glfw PRIVATE -Wno-deprecated-declarations)
    endif (${COMPILER_IS_GCCLIKE})
endif ()

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

set(INCLUDE_DIRS ${INCLUDE_DIRS} ${LIB_DIR}/optional-lite/include)

# validate shaders
add_custom_target(validate_shaders 
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/validate_shaders.py ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${PROJECT_OPENGL_VERSION_MAJOR} ${PROJECT_OPENGL_VERSION_MINOR})

# main target
add_executable(${PROJECT_NAME} ${SRC})
add_dependencies(${PROJECT_NAME} validate_shaders)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILE_DEFS})
if (${COMPILER_IS_GCCLIKE})
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>: -g>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:RELEASE>: -flto=thin>)
endif (${COMPILER_IS_GCCLIKE})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
