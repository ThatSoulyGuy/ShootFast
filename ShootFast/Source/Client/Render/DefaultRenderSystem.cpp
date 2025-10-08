#include "Client/Render/DefaultRenderSystem.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Client/Render/ClientRenderContext.hpp"
#include "Client/Render/Registries.hpp"
#include "Client/Render/RenderMesh.hpp"
#include "Client/Render/Vertices/VertexDefault.hpp"
#include "Independent/ECS/World.hpp"
#include "Independent/Math/Transform.hpp"

namespace ShootFast::Client::Render
{
    static void BindStandardUniforms(const ShaderProgram& prog, const glm::mat4& model)
    {
        if (const GLint location = glGetUniformLocation(prog.programId, "modelMatrix"); location >= 0)
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
    }

    void DefaultRenderSystem::Run(Independent::ECS::World& world) const
    {
        auto& meshRegistry = *context.meshRegistry;
        auto& materialRegistry = *context.materialRegistry;
        auto& shaderRegistry = *context.shaderRegistry;
        auto& textureRegistry = *context.textureRegistry;

        for (auto [gameObject, localToWorld, renderMesh] : world.View<Independent::Math::LocalToWorld, RenderMesh<Vertices::VertexDefault>>())
        {
            if (!renderMesh.mesh || !renderMesh.material)
                continue;

            auto& [shader, albedo] = materialRegistry.Get(renderMesh.material);
            auto& shaderProgram = shaderRegistry.Get(*shader);

            const auto& mesh = meshRegistry.Get(renderMesh.mesh);

            glUseProgram(shaderProgram.programId);

            BindStandardUniforms(shaderProgram, localToWorld.matrix);

            if (albedo)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureRegistry.Get(*albedo).textureId);

                if (const GLint location = glGetUniformLocation(shaderProgram.programId, "diffuse"); location >= 0)
                    glUniform1i(location, 0);
            }

            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
}
