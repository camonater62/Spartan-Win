#pragma once

#include "Camera.h"
#include "IndexBuffer.h"
#include "Plane.h"
#include "Shader.h"
#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <glm/glm.hpp>
#include <memory>

namespace test {

class TestNoise : public Test {
public:
    TestNoise();
    ~TestNoise();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    Plane m_Plane;

    glm::mat4 m_Model;

    Camera m_Camera;
};

}