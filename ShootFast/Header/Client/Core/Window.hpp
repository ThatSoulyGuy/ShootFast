#ifndef SHOOTFAST_WINDOW_HPP
#define SHOOTFAST_WINDOW_HPP

#include <string>
#include <glm/vec2.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ShootFast::Client::Core
{
    class Window final
    {

    public:

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        ~Window();

        void Initialize(const std::string& title, const glm::ivec2& dimensions);

        [[nodiscard]]
        GLFWwindow* GetHandle() const;

        [[nodiscard]]
        glm::ivec2 GetDimensions() const;

        void SetDimensions(const glm::ivec2&) const;

        [[nodiscard]]
        glm::ivec2 GetPosition() const;

        void SetPosition(const glm::ivec2&) const;

        [[nodiscard]]
        bool IsRunning() const;

        static void Clear();
        void Present() const;

        static Window& GetInstance();

    private:

        Window() = default;

        GLFWwindow* handle;

    };
}

#endif