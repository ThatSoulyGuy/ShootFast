#include "Client/Core/InputManager.hpp"

#include "Client/Core/Window.hpp"

namespace ShootFast::Client::Core
{
    void InputManager::Initialize()
    {
        handle = Window::GetInstance().GetHandle();

        glfwSetScrollCallback(handle, ScrollCallback);
        glfwSetCharCallback(handle, CharacterCallback);

        currentKeyStates.resize(GLFW_KEY_LAST + 1, false);
        previousKeyStates.resize(GLFW_KEY_LAST + 1, false);

        currentMouseStates.resize(8, false);
        previousMouseStates.resize(8, false);

        double x, y;

        glfwGetCursorPos(handle, &x, &y);

        mousePosition = { static_cast<float>(x), static_cast<float>(y) };
    }

    bool InputManager::GetKeyState(const KeyCode& code, const KeyState& state) const
    {
        const int key = static_cast<int>(code);

        const bool wasPressed = previousKeyStates[key];
        const bool isPressed = currentKeyStates[key];

        switch (state)
        {
            case KeyState::PRESSED:
                return isPressed && !wasPressed;

            case KeyState::HELD:
                return isPressed;

            case KeyState::RELEASED:
                return !isPressed && wasPressed;

            default:
                return false;
        }
    }

    bool InputManager::GetMouseState(const MouseCode& code, const MouseState& state) const
    {
        const int button = static_cast<int>(code);
        const bool wasPressed = previousMouseStates[button];
        const bool isPressed = currentMouseStates[button];

        switch (state)
        {
            case MouseState::PRESSED:
                return isPressed && !wasPressed;

            case MouseState::HELD:
                return isPressed;

            case MouseState::RELEASED:
                return !isPressed && wasPressed;

            default:
                return false;
        }
    }

    MouseMode InputManager::GetMouseMode() const
    {
        switch (glfwGetInputMode(handle, GLFW_CURSOR))
        {

            case GLFW_CURSOR_DISABLED:
                return MouseMode::LOCKED;

            case GLFW_CURSOR_NORMAL:
                return MouseMode::FREE;

            default:
                return MouseMode::FREE;
        }
    }

    void InputManager::SetMouseMode(const MouseMode& mode) const
    {
        if (GetMouseMode() == mode)
            return;

        switch (mode)
        {

            case MouseMode::LOCKED:
                glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;

            case MouseMode::FREE:
                glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;

            default:
                break;
        }
    }

    std::string InputManager::ConsumeTextInput()
    {
        std::lock_guard lock(textMutex);
        std::string out;

        out.swap(typedAscii);

        return out;
    }

    glm::vec2 InputManager::GetMousePosition() const
    {
        return mousePosition;
    }

    glm::vec2 InputManager::GetMouseDelta() const
    {
        return mouseDelta;
    }

    float InputManager::GetScrollDelta() const
    {
        return scrollDelta;
    }

    void InputManager::Update()
    {
        if (!handle)
            return;

        previousKeyStates = currentKeyStates;
        previousMouseStates = currentMouseStates;

        for (int key = 0; key < static_cast<int>(currentKeyStates.size()); ++key)
            currentKeyStates[key] = glfwGetKey(handle, key) == GLFW_PRESS || glfwGetKey(handle, key) == GLFW_REPEAT;

        for (int button = 0; button < static_cast<int>(currentMouseStates.size()); ++button)
            currentMouseStates[button] = glfwGetMouseButton(handle, button) == GLFW_PRESS;

        double x, y;

        glfwGetCursorPos(handle, &x, &y);

        const glm::vec2 newPosition{ static_cast<float>(x), static_cast<float>(y) };

        mouseDelta = newPosition - mousePosition;
        mousePosition = newPosition;

        scrollDelta = pendingScroll;
        pendingScroll = 0.0f;
    }

    void InputManager::Reset()
    {
        mousePosition = { 0.0f, 0.0f };
        mouseDelta = { 0.0f, 0.0f };

        typedAscii = "";

        scrollDelta = 0.0f;
        pendingScroll = 0.0f;
    }

    void InputManager::ScrollCallback(GLFWwindow*, double, const double offsetY)
    {
        GetInstance().pendingScroll += static_cast<float>(offsetY);
    }

    void InputManager::CharacterCallback(GLFWwindow *, const unsigned int codepoint)
    {
        if (codepoint >= 32u && codepoint <= 126u)
        {
            auto& self = GetInstance();

            std::lock_guard lock(self.textMutex);
            self.typedAscii.push_back(static_cast<char>(codepoint));
        }
    }
}
