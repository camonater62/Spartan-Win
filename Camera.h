#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    ~Camera();

    void SetProjectionMatrix(const glm::mat4 &proj) {
        m_Proj = proj;
    }
    void SetLookAt(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &worldUp);

    glm::mat4 GetViewMatrix() const {
        return m_View;
    }
    glm::mat4 GetProjectionMatrix() const {
        return m_Proj;
    }
    glm::mat4 GetViewProjectionMatrix() const {
        return m_Proj * m_View;
    }

private:
    glm::mat4 m_Proj;
    glm::mat4 m_View;
};