#include "TestTexture2D.h"

#include "IndexBuffer.h"
#include "Macros.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test {

TestTexture2D::TestTexture2D()
    : m_TranslationA(200, 200, 0)
    , m_TranslationB(400, 200, 0) {
    float positions[] = {
        -50.0f, -50.0f, 0.0f, 0.0f, // 0
        50.0f, -50.0f, 1.0f, 0.0f, // 1
        50.0f, 50.0f, 1.0f, 1.0f, // 2
        -50.0f, 50.0f, 0.0f, 1.0f, // 3
    };
    unsigned int indices[] = {
        0, 1, 2, // 0
        2, 3, 0 // 1
    };

    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(2); // position
    layout.Push<float>(2); // UV
    m_VAO->AddBuffer(*m_VBO, layout);
    m_IBO = std::make_unique<IndexBuffer>(indices, 6);

    m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
    m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    m_Shader = std::make_unique<Shader>("res/shaders/BasicTexture.shader");
    m_Shader->Bind();
    m_Texture = std::make_unique<Texture>("res/textures/sudou.jpg");

    m_Shader->SetUniform1i("u_Texture", 0);

    m_VAO->UnBind();
    m_VBO->UnBind();
    m_IBO->UnBind();
    m_Shader->UnBind();
}

TestTexture2D::~TestTexture2D() {
}

void TestTexture2D::OnUpdate(float deltaTime) {
    (void) deltaTime;
}

void TestTexture2D::OnRender() {
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;

    m_Texture->Bind();

    { // Rect A
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    }

    { // Rect B
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    }
}

void TestTexture2D::OnImGuiRender() {
    ImGui::SliderFloat3("Translation A", &m_TranslationA[0], 0.0f, 960.0f);
    ImGui::SliderFloat3("Translation B", &m_TranslationB[0], 0.0f, 960.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
}

}
