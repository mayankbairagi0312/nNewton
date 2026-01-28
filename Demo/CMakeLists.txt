find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(nNewtonTest
    main.cpp
    DemoFramework/Application.cpp
    "Renderer/DebugRenderer.cpp"
    ui/DebugUI.cpp
   
 "DemoFramework/Application.h" 
 "DemoFramework/Window.h" 
 "DemoFramework/Window.cpp" 
 "DemoFramework/Input.h"
 "DemoFramework/Input.cpp" 
 "Renderer/Shader.cpp"
 "Renderer/Camera.cpp"
 "Renderer/GL_Debug_Renderer.cpp")

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
    ${CMAKE_CURRENT_SOURCE_DIR}/application
    ${CMAKE_CURRENT_SOURCE_DIR}/Renderer
)