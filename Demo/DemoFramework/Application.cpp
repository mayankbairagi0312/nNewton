#include "Application.h"

Application::Application()
	: m_testWindow(nullptr),m_debugDrawer(nullptr), m_running(false), m_CurrTime(0), m_PrevTime(SDL_GetPerformanceCounter()), m_DeltaTime(0.0f) { }

bool Application::Initialize() {

    m_testWindow = std::make_unique<Window>();
    

    //m_debugDrawer = new OpneGLDebugRenderer();
    m_debugDrawer = std::make_unique<OpneGLDebugRenderer>();
    m_TestDebugRenderer.SetDrawer(m_debugDrawer.get());

    // Initialize window 
    if (!m_testWindow->Init()) {
        std::cerr << "Failed to initialize window system!" << std::endl;
        return false;
    }

    // Create window
    if (!m_testWindow->CreateWindow()) {  
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    m_input.SetCamera(m_camera);
    if (!m_debugDrawer->init_renderer(&m_camera))
    {
        std::cerr << "camera ki MKC" << std::endl;
    }
    m_camera.setPosition(glm::vec3(0.0f, 0.0f, 15.0f));
    glViewport(0, 0, m_testWindow->getWidth(), m_testWindow->getHeight());
    m_running = true;


    std::cout << "Application initialized successfully.\n";

    return true;
	
}

void Application::Run() {

    if (!m_testWindow || !m_testWindow->IsValid()) {
        std::cerr << "Cannot run: Window is not valid!" << std::endl;
        return;
    }
   
    m_PrevTime = SDL_GetPerformanceCounter();
    std::cout << "Application starting main loop...\n";
	while (m_running)
	{   
        m_CurrTime = SDL_GetPerformanceCounter();
        m_DeltaTime = (double)((m_CurrTime - m_PrevTime)  / (double)SDL_GetPerformanceFrequency());
        m_PrevTime = m_CurrTime;

        m_input.BeginFrame();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                m_running = false;
            }
           
            m_input.ProcessEvent(&event);
            
            
        }

        
        m_input.ProcessInputKey(m_DeltaTime);
        m_input.ProcessMosueInput();
        TRender();
        m_input.EndFrame();
        SDL_GL_SwapWindow(m_testWindow->GetNativeHandle());
	}
}



void Application::TRender()
{
    auto min = glm::vec3(-1.5f, -1.5f, -1.5f);  
    auto max = glm::vec3(1.5f, 1.5f, 1.5f);
    auto min1 = glm::vec3(2.0f, 2.0f, 2.0f);
    auto max1 = glm::vec3(4.5f, 4.5f, 4.5f);
    auto center = (min + max) * 0.5f;
    auto center2 = (min1 + max1) * 0.5f;
    auto color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    auto color2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    m_TestDebugRenderer.BeginFrame();
    //m_TestDebugRenderer.DrawLine(from,to ,color);
    //m_TestDebugRenderer.DrawLine(from, to1, color);
    m_TestDebugRenderer.drawArrow(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f), 0.4,color2);
    m_TestDebugRenderer.DrawBox(min, max, center, color);
    m_TestDebugRenderer.DrawSphere(center, color);
    m_TestDebugRenderer.DrawSphere(center2, color);
    m_TestDebugRenderer.DrawBox(min1, max1, center2, color2);
    m_TestDebugRenderer.Endframe();
    
}

void Application::Shutdown() {
    std::cout << "Application shutting down...\n";
    m_TestDebugRenderer.clear();
    if (m_testWindow) {
        
        m_testWindow->Shutdown();

    }

    m_testWindow.reset();  
    m_running = false;
}

Application::~Application() {
    Shutdown();  

}