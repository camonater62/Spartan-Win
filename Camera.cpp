#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : m_Proj(glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f))
    , m_View(glm::mat4(1.0f)) {
}

Camera::~Camera() {
}

void Camera::SetLookAt(
    const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &worldUp) {
    glm::vec3 front = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, front));
    m_View = glm::lookAt(position, position + front, up);
}