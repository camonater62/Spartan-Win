#include "Cube.h"

#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

Cube::Cube() {
    float vertices[] = {
        // Position (3) // Normal (3) // Tex coord (2)
        // Front
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.f, 0.0f, 0.0f, // 0
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.f, 1.0f, 0.0f, // 1
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.f, 1.0f, 1.0f, // 2
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.f, 0.0f, 1.0f, // 3
        // Back
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.f, 0.0f, 0.0f, // 4
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.f, 1.0f, 0.0f, // 5
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.f, 1.0f, 1.0f, // 6
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.f, 0.0f, 1.0f, // 7
        // Left
        -0.5f, -0.5f, -0.5f, -1.f, 0.0f, 0.f, 0.0f, 0.0f, // 8
        -0.5f, -0.5f, 0.5f, -1.f, 0.0f, 0.f, 1.0f, 0.0f, // 9
        -0.5f, 0.5f, 0.5f, -1.f, 0.0f, 0.f, 1.0f, 1.0f, // 10
        -0.5f, 0.5f, -0.5f, -1.f, 0.0f, 0.f, 0.0f, 1.0f, // 11
        // Right
        0.5f, -0.5f, -0.5f, 1.f, 0.0f, 0.f, 0.0f, 0.0f, // 12
        0.5f, -0.5f, 0.5f, 1.f, 0.0f, 0.f, 1.0f, 0.0f, // 13
        0.5f, 0.5f, 0.5f, 1.f, 0.0f, 0.f, 1.0f, 1.0f, // 14
        0.5f, 0.5f, -0.5f, 1.f, 0.0f, 0.f, 0.0f, 1.0f, // 15
        // Top
        -0.5f, 0.5f, -0.5f, 0.f, 1.0f, 0.f, 0.0f, 0.0f, // 16
        -0.5f, 0.5f, 0.5f, 0.f, 1.0f, 0.f, 1.0f, 0.0f, // 17
        0.5f, 0.5f, 0.5f, 0.f, 1.0f, 0.f, 1.0f, 1.0f, // 18
        0.5f, 0.5f, -0.5f, 0.f, 1.0f, 0.f, 0.0f, 1.0f, // 19
        // Bottom
        -0.5f, -0.5f, -0.5f, 0.f, -1.0f, 0.f, 0.0f, 0.0f, // 20
        -0.5f, -0.5f, 0.5f, 0.f, -1.0f, 0.f, 1.0f, 0.0f, // 21
        0.5f, -0.5f, 0.5f, 0.f, -1.0f, 0.f, 1.0f, 1.0f, // 22
        0.5f, -0.5f, -0.5f, 0.f, -1.0f, 0.f, 0.0f, 1.0f, // 23
    };
    unsigned int indices[] = {
        // Front
        0, 1, 2, // 0
        2, 3, 0, // 1
        // Back
        4, 5, 6, // 2
        6, 7, 4, // 3
        // Left
        8, 9, 10, // 4
        10, 11, 8, // 5
        // Right
        12, 13, 14, // 6
        14, 15, 12, // 7
        // Top
        16, 17, 18, // 8
        18, 19, 16, // 9
        // Bottom
        20, 21, 22, // 10
        22, 23, 20 // 11
    };

    m_VAO = std::make_unique<VertexArray>();
    // 6 faces of 4 vertices of 8 floats
    m_VBO = std::make_unique<VertexBuffer>(vertices, 6 * 4 * 8 * sizeof(float));
    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // UV
    m_VAO->AddBuffer(*m_VBO, layout);
    m_IBO = std::make_unique<IndexBuffer>(indices, 36);

    m_Shader = std::make_unique<Shader>("res/shaders/Normal.shader");
    m_Shader->Bind();

    m_VAO->UnBind();
    m_VBO->UnBind();
    m_IBO->UnBind();
    m_Shader->UnBind();
}

Cube::~Cube() {
}

void Cube::draw(const glm::mat4 &MVP, std::optional<Shader *> opt_shader,
    std::optional<Texture *> opt_texture) {
    Shader *shader = opt_shader.value_or(m_Shader.get());
    Renderer renderer;
    shader->Bind();
    shader->SetUniformMat4f("u_MVP", MVP);
    if (opt_texture) {
        opt_texture.value()->Bind();
    }
    renderer.Draw(*m_VAO, *m_IBO, *shader);
}