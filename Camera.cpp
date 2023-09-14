#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : m_View(glm::mat4(1.0f))
    , m_FOV(60.0f)
    , m_AspectRatio(16.0f / 9.0f)
    , m_NearPlane(0.01f)
    , m_FarPlane(100.0f)
{
    RecalculateProjectionMatrix();
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

void Camera::RecalculateProjectionMatrix() {
	m_Proj = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}