find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

include(FetchContent)


# GLM (header-only, private external)


FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	    e7970a8b26732f1b0df9690f7180546f8c30e48e #refs/tags/
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(glm)


#add_library(glm INTERFACE)
#target_include_directories(glm INTERFACE
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glm>
#)
#set_target_properties(glm PROPERTIES FOLDER "External")


# ImGui

set(IMGUI_VERSION "docking") # For develop:  "docking".  release: "v1.92.5-docking"

FetchContent_Declare(
    imgui_src
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        ${IMGUI_VERSION}  # Variable is used here
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(imgui_src)

# Core files
set(IMGUI_SOURCES
    ${imgui_src_SOURCE_DIR}/imgui.cpp
    ${imgui_src_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_src_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_src_SOURCE_DIR}/imgui_widgets.cpp
)

#optional
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND IMGUI_SOURCES ${imgui_src_SOURCE_DIR}/imgui_demo.cpp)
endif()

# Backend selection
set(IMGUI_BACKEND_SOURCES
    ${imgui_src_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_src_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})

target_include_directories(imgui PUBLIC
    ${imgui_src_SOURCE_DIR}
    ${imgui_src_SOURCE_DIR}/backends
)


target_compile_definitions(imgui PUBLIC 
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)


if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "SDL3 not found - required for ImGui backend")
endif()

target_link_libraries(imgui
    PUBLIC
        SDL3::SDL3
        glad
        OpenGL::GL
)

set_target_properties(imgui PROPERTIES FOLDER "External")


# GLAD

add_library(glad STATIC
    glad/src/gl.c
)

target_include_directories(glad PUBLIC
    glad/include
)
set_target_properties(glad PROPERTIES FOLDER "External")
