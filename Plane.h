#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include <memory>

class Plane {
public:
    Plane(int widthSegments = 1, int heightSegments = 1, bool noisy = false);
    ~Plane();

    void Render(glm::mat4 MVP);

protected:
    void generatePlane(int widthSegments, int heightSegments, bool noisy,
        std::vector<float> &vertices, std::vector<unsigned int> &indices);

    std::shared_ptr<VertexArray> m_VAO;
    std::shared_ptr<VertexBuffer> m_VBO;
    std::shared_ptr<IndexBuffer> m_IBO;
    std::shared_ptr<Shader> m_Shader;
};