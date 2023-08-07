#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <glm/glm.hpp>
#include <memory>
#include <optional>

class Cube {
public:
    Cube();
    ~Cube();
    void draw(const glm::mat4 &MVP, std::optional<Shader *> shader = std::nullopt,
        std::optional<Texture *> texture = std::nullopt);

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;

    std::unique_ptr<Shader> m_Shader;
};