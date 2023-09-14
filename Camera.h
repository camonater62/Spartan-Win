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

    float GetFOV() const {
		return m_FOV;
	}
    void SetFOV(float inFOV) {
		m_FOV = inFOV;
        RecalculateProjectionMatrix();
	}

    float GetAspectRatio() const {
        return m_AspectRatio;
    }
    void SetAspectRatio(float inAspectRatio) {
		m_AspectRatio = inAspectRatio;
		RecalculateProjectionMatrix();
	}

    float GetNearPlane() const {
		return m_NearPlane;
	}
    void SetNearPlane(float inNearPlane) {
		m_NearPlane = inNearPlane;
        RecalculateProjectionMatrix();
	}

    float GetFarPlane() const {
		return m_FarPlane;
	}
    void SetFarPlane(float inFarPlane) {
		m_FarPlane = inFarPlane;
        RecalculateProjectionMatrix();
	}

private:
    glm::mat4 m_Proj;
    glm::mat4 m_View;

    float m_FOV;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    void RecalculateProjectionMatrix();
};