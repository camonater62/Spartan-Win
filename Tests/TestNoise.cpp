#include "TestNoise.h"

#include "Macros.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <iostream>

namespace test {

TestNoise::TestNoise() {
    m_Camera.SetProjectionMatrix(
        glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f));
    m_Camera.SetLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    m_Model = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 0.05));
    m_Plane = Plane(10, 10, true);
}

TestNoise::~TestNoise() {
}

void TestNoise::OnUpdate(float deltaTime) {
    (void) deltaTime;
}

void TestNoise::OnRender() {
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDisable(GL_CULL_FACE));

    glm::mat4 mvp = m_Camera.GetViewProjectionMatrix() * m_Model;
    m_Plane.Render(mvp);
}

void TestNoise::OnImGuiRender() {
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
}

}