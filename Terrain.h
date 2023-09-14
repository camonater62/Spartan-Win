#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

class Terrain
{
public:
	Terrain(int width, int height);
	~Terrain();

private:
	void generateTerrain(int widthSegments, int heightSegments,
		std::vector<float>& vertices, std::vector<unsigned int>& indices);

	std::shared_ptr<VertexArray> m_VAO;
	std::shared_ptr<VertexBuffer> m_VBO;
	std::shared_ptr<IndexBuffer> m_IBO;
	std::shared_ptr<Shader> m_Shader;

public:
	void Render(glm::mat4 mvp);

};

