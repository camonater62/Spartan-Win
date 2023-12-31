#include "Plane.h"

#include "Renderer.h"

#include <glm/gtc/noise.hpp>
#include <iostream>
#include <vector>

Plane::Plane(int widthSegments, int heightSegments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    generatePlane(widthSegments, heightSegments, vertices, indices);

    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(3); // Position
    layout.Push<float>(3); // Normal
    layout.Push<float>(2); // Texture
    layout.Push<float>(3); // Color

    m_VAO = std::make_shared<VertexArray>();
    m_VBO = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(float));
    m_VAO->AddBuffer(*m_VBO, layout);
    m_IBO = std::make_shared<IndexBuffer>(&indices[0], indices.size());

    m_Shader = std::make_shared<Shader>("res/shaders/Plane.shader");
    m_Shader->Bind();

    m_VAO->UnBind();
    m_VBO->UnBind();
    m_IBO->UnBind();
    m_Shader->UnBind();
}

Plane::~Plane() {
}

void Plane::Render(glm::mat4 MVP) {
    Renderer renderer;
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP", MVP);
    renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
}

void Plane::generatePlane(int widthSegments, int heightSegments, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float seg_width = 1.0f / widthSegments;
    float seg_height = 1.0f / heightSegments;

    for (int i = 0; i < heightSegments + 1; i++) {
        float y = i * seg_height - 0.5f;

        for (int j = 0; j < widthSegments + 1; j++) {
            float x = j * seg_width - 0.5f;
            float z = 0.0f;

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);

            // Texture
            vertices.push_back(j * seg_width);
            vertices.push_back(i * seg_height);

            // Color
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
        }
    }

    for (int i = 0; i < heightSegments; i++) {
        for (int j = 0; j < widthSegments; j++) {
            unsigned int a = i * (widthSegments + 1) + (j + 1);
            unsigned int b = i * (widthSegments + 1) + j;
            unsigned int c = (i + 1) * (widthSegments + 1) + j;
            unsigned int d = (i + 1) * (widthSegments + 1) + (j + 1);

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(c);
            indices.push_back(d);
            indices.push_back(a);
        }
    }
}