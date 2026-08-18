cmake_minimum_required(VERSION 3.24)

project(ModernGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
find_package(glm REQUIRED)

if(NOT OpenGL_EGL_FOUND)
    message(FATAL_ERROR "EGL was not found")
endif()

message(STATUS "OpenGL library: ${OPENGL_opengl_LIBRARY}")
message(STATUS "EGL library:     ${OPENGL_egl_LIBRARY}")
message(STATUS "OpenGL include:  ${OPENGL_INCLUDE_DIR}")
message(STATUS "EGL include:     ${OPENGL_EGL_INCLUDE_DIRS}")


set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

set(GLAD_SRC
        "${VENDOR_DIR}/src/gl.c"
)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(
        GLOB ENGINE_SOURCES
        CONFIGURE_DEPENDS
        "${SRC_DIR}/*.cpp"
)


include_directories("${VENDOR_DIR}/include")

add_executable(${PROJECT_NAME}
        ${ENGINE_SOURCES}
        ${GLAD_SRC}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLFW_EXPOSE_NATIVE_X11
        GLFW_EXPOSE_NATIVE_WAYLAND
)

target_include_directories(${PROJECT_NAME} PRIVATE "${VENDOR_DIR}/include")

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::OpenGL
        OpenGL::EGL
        glfw
        glm::glm
)


 if(CMAKE_BUILD_TYPE STREQUAL "Debug" )
     target_compile_options(${PROJECT_NAME} PRIVATE
             -Wall
             -Wextra
             -Wpedantic
             -Werror
             -Wno-gnu-anonymous-struct
             -Wno-nested-anon-types
             # In code: // NOLINT(cppcoreguidelines-pro-type-member-init)
     )
 endif()



if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /std:c++latest
    )
endif()
