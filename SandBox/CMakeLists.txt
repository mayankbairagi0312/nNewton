find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(nNewtonTest
    main.cpp
    Core/Application.cpp
    "Renderer/DebugRenderer.cpp"
    DebugUI/DebugUI.cpp
    Renderer/Shader.cpp
 "Core/Application.h" 
 "Core/Window.h" 
 "Core/Window.cpp" 
 "Core/Input.h"
 "Core/Input.cpp" 
 
 "Renderer/Camera.cpp"
 "Renderer/GL_Debug_Renderer.cpp"
"Renderer/RenderSystem.cpp"
)

#target_compile_features(nNewtonTest PRIVATE cxx_std_20)


target_include_directories(nNewtonTest PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


target_link_libraries(nNewtonTest PRIVATE
    nNewton::nNewton
    SDL3::SDL3
    imgui
    glad
    OpenGL::GL
)
add_dependencies(nNewtonTest imgui)
target_compile_definitions(nNewton PRIVATE GLM_ENABLE_EXPERIMENTAL)
set_target_properties(nNewtonTest PROPERTIES
    FOLDER "Applications"
)
target_include_directories(nNewtonTest PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}

)
add_custom_command(TARGET nNewtonTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:nNewtonTest>/assets
)
