#pragma once

#include "Camera.h"
#include "Cube.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <glm/glm.hpp>
#include <memory>

namespace test {

class TestCube : public Test {
public:
    TestCube();
    ~TestCube();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    Cube m_Cube;
    glm::mat4 m_Model;
    float m_RotationSpeed;

    Camera m_Camera;
    glm::vec3 m_CameraPosition;
};

}