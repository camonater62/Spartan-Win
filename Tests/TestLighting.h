#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "Test.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Cube.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <memory>

namespace test {
	class TestLighting : public Test {
	public:
		TestLighting();
		~TestLighting();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnWindowResize(int width, int height) override;

	private:
		Cube m_Cube;
		glm::mat4 m_Model;
		float m_RotationSpeed;

		Camera m_Camera;
		glm::vec3 m_CameraPosition;

		std::shared_ptr<Shader> m_Shader;

		glm::vec3 m_LightPosition;
		glm::vec3 m_LightColor;
	};
}