#include "TestCube.h"

#include "Macros.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <iostream> 

namespace test {

TestCube::TestCube()
    : m_Model(1.0f)
    , m_RotationSpeed(1.0f)
    , m_CameraPosition(0, 0, 3) {
}

TestCube::~TestCube() {
}

void TestCube::OnUpdate(float deltaTime) {
    glm::vec3 axis = glm::normalize(glm::vec3(1.0f, 0.5f, 0.0f));
    float angle = m_RotationSpeed * deltaTime * glm::radians(45.f);
    m_Model = glm::rotate(m_Model, angle, axis);
    m_Camera.SetLookAt(
        m_CameraPosition, m_CameraPosition + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void TestCube::OnRender() {
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDisable(GL_CULL_FACE));

    glm::mat4 mvp = m_Camera.GetViewProjectionMatrix() * m_Model;
    m_Cube.draw(mvp);
}

void TestCube::OnImGuiRender() {
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
    ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 20.0f);
    ImGui::SliderFloat3("Camera Position", &m_CameraPosition.x, -10.0f, 10.0f);
}

void TestCube::OnWindowResize(int width, int height) {
	m_Camera.SetAspectRatio((float)width / (float)height);
}

}