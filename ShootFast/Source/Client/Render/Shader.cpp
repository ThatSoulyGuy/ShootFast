#include "Client/Render/Shader.hpp"
#include <stdexcept>
#include <glad/gl.h>
#include "Client/Render/ClientRenderContext.hpp"
#include "Client/Render/Handles.hpp"
#include "Client/Render/Registries.hpp"
#include "Independent/Utility/FileHelper.hpp"

using namespace ShootFast::Independent::Utility;

namespace
{
    GLuint Compile(const GLenum type, const std::string& source)
    {
        const GLuint result = glCreateShader(type);
        const char* sourceCharArray = source.c_str();

        glShaderSource(result, 1, &sourceCharArray, nullptr);
        glCompileShader(result);

        GLint status = 0;

        glGetShaderiv(result, GL_COMPILE_STATUS, &status);

        if (!status)
        {
            GLint messageLength = 0;

            glGetShaderiv(result, GL_INFO_LOG_LENGTH, &messageLength);
            std::string log(static_cast<size_t>(messageLength), '\0');

            glGetShaderInfoLog(result, messageLength, nullptr, log.data());
            glDeleteShader(result);

            throw std::runtime_error("Shader compile error: " + log);
        }

        return result;
    }

    GLuint Link(const GLuint vertexShader, const GLuint fragmentShader)
    {
        const GLuint program = glCreateProgram();

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);

        GLint status = 0;

        glGetProgramiv(program, GL_LINK_STATUS, &status);

        if (!status)
        {
            GLint messageLength = 0;

            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &messageLength);

            std::string log(static_cast<size_t>(messageLength), '\0');

            glGetProgramInfoLog(program, messageLength, nullptr, log.data());
            glDeleteProgram(program);

            throw std::runtime_error("Program link error: " + log);
        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }
}

namespace ShootFast::Client::Render
{
    Shader::Handle Shader::Create(const ClientRenderContext& context, const Shader& shader)
    {
        const GLuint program = Link(Compile(GL_VERTEX_SHADER, FileHelper::ReadFile(shader.vertexPath)), Compile(GL_FRAGMENT_SHADER, FileHelper::ReadFile(shader.fragmentPath)));

        return context.shaderRegistry->Create(program);
    }

    void Shader::Destroy(const ClientRenderContext& context, const Handle handle)
    {
        if (auto& [programId] = context.shaderRegistry->Get(handle); programId != 0u)
        {
            glDeleteProgram(programId);

            programId = 0u;
        }
    }
}
