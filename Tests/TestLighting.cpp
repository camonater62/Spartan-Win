#include "TestLighting.h"

#include "Macros.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <iostream> 

namespace test {

    TestLighting::TestLighting()
        : m_Model(1.0f)
        , m_RotationSpeed(0.0f)
        , m_CameraPosition(0, 0, 3)
        , m_LightPosition(1.2f, 1.0f, -2.0f)
        , m_LightColor(0.2, 0.3, 0.5) {

        m_Shader = std::make_shared<Shader>("res/shaders/Lighting.shader");
    }

    TestLighting::~TestLighting() {
    }

    void TestLighting::OnUpdate(float deltaTime) {
        glm::vec3 axis = glm::normalize(glm::vec3(1.0f, 0.5f, 0.0f));
        float angle = m_RotationSpeed * deltaTime * glm::radians(45.f);
        m_Model = glm::rotate(m_Model, angle, axis);
        m_Camera.SetLookAt(
            m_CameraPosition, m_CameraPosition + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    }

    void TestLighting::OnRender() {
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDisable(GL_CULL_FACE));

        glm::mat4 mvp = m_Camera.GetViewProjectionMatrix() * m_Model;
        m_Shader->Bind();
        m_Shader->SetUniform3f("u_LightPos", m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
        m_Shader->SetUniform3f("u_LightColor", m_LightColor.x, m_LightColor.y, m_LightColor.z);
        m_Shader->SetUniform3f("u_ViewPos", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
        m_Cube.draw(mvp, m_Shader.get());
    }

    void TestLighting::OnImGuiRender() {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
        ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 20.0f);
        ImGui::SliderFloat3("Camera Position", &m_CameraPosition.x, -10.0f, 10.0f);
        ImGui::ColorPicker3("Light Color", &m_LightColor.x);
    }

    void TestLighting::OnWindowResize(int width, int height) {
        m_Camera.SetAspectRatio((float)width / (float)height);
    }

}