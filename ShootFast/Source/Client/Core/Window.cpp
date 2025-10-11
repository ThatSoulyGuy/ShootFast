#include "Client/Core/Window.hpp"
#include <stdexcept>

#include "../../../../cmake-build-debug-event-trace/_deps/glfw-src/src/internal.h"

namespace ShootFast::Client::Core
{
    Window::~Window()
    {
        glfwDestroyWindow(handle);

        glfwTerminate();
    }

    void Window::Initialize(const std::string& title, const glm::ivec2& dimensions)
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW!");

        glfwWindowHint(GLFW_RESIZABLE, true);
        glfwWindowHint(GLFW_VISIBLE, false);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

        handle = glfwCreateWindow(dimensions.x, dimensions.y, title.c_str(), nullptr, nullptr);

        if (!handle)
            throw std::runtime_error("Failed to create window!");

        glfwMakeContextCurrent(handle);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            throw std::runtime_error("Failed to initialize GLAD!");

        glfwSetFramebufferSizeCallback(handle, [](GLFWwindow*, const int width, const int height)
        {
            glViewport(0, 0, width, height);
        });

        glfwShowWindow(handle);
    }

    GLFWwindow* Window::GetHandle() const
    {
        return handle;
    }

    glm::ivec2 Window::GetDimensions() const
    {
        int width, height;

        glfwGetWindowSize(handle, &width, &height);

        return { width, height };
    }

    void Window::SetDimensions(const glm::ivec2& dimensions) const
    {
        glfwSetWindowSize(handle, dimensions.x, dimensions.y);
    }

    glm::ivec2 Window::GetPosition() const
    {
        int x, y;

        glfwGetWindowPos(handle, &x, &y);

        return { x, y };
    }

    void Window::SetPosition(const glm::ivec2& position) const
    {
        glfwSetWindowPos(handle, position.x, position.y);
    }

    bool Window::IsRunning() const
    {
        return !glfwWindowShouldClose(handle);
    }

    void Window::Clear()
    {
        glClearColor(0.0f, 0.45f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::Present() const
    {
        glfwSwapBuffers(handle);
        glfwPollEvents();
    }

    Window& Window::GetInstance()
    {
        static Window instance = {};

        return instance;
    }
}
