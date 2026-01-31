#include "Window.h"

Window::Window() : m_window(nullptr), m_glContext(nullptr),m_windowWidth(1600), m_windowHeight(1200), m_isFullscreen(false), m_isInitialized(false),m_isCreated(false){

}

bool Window::Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_isInitialized = true;
    std::cout << "SDL initialized successfully.\n";

    return true;
}
bool Window::CreateWindow()
{   
    if (m_window) {
        std::cerr << "Window already exists!" << std::endl;
        return false;
    }

    m_window = SDL_CreateWindow(
        "nNewton : Physics Engine - Editor",
        m_windowWidth,
        m_windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!m_window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    //OpenGL context
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        return false;
    }


    SDL_GL_MakeCurrent(m_window, m_glContext);

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD (OpenGL)" << std::endl;
        SDL_GL_DestroyContext(m_glContext);
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        m_glContext = nullptr;
        return false;
    }

    //  OpenGL version
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;

    m_isCreated = true;
    std::cout << "Window created successfully. Size: "
        << m_windowWidth << "x" << m_windowHeight << std::endl;

    return true;
}


Window::~Window() {
    DestroyWindow(); 
}

void Window::DestroyWindow() {
    if (m_window) {
        // If using OpenGL
        if (m_glContext) {
            SDL_GL_DestroyContext(m_glContext);
            m_glContext = nullptr;
        }

        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        std::cout << "Window destroyed successfully.\n";
    }

    // Reset state
    m_isCreated = false;
    m_windowWidth = 0;
    m_windowHeight = 0;
}


void Window::Shutdown() {
    DestroyWindow();

    if (m_isInitialized) {
        SDL_Quit();
        m_isInitialized = false;
        std::cout << "SDL shutdown complete.\n";
    }
}
void Window::setWindow(const Sint32& width, const Sint32& height)
{
    m_windowWidth = width;
    m_windowHeight = height;

    glViewport(0, 0, width, height);

    printf("Window Resized -> Update Viewport: %d x %d\n", m_windowWidth, m_windowHeight);
}
bool Window:: IsValid() const { return m_window != nullptr; }
bool Window:: IsInitialized() const { return m_isInitialized; }
bool Window::IsCreated() const { return m_isCreated; }
