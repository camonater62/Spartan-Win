#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "Test.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <memory>

namespace test {

class TestAssimp : public Test {
public:
    TestAssimp();
    ~TestAssimp();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    Assimp::Importer m_Importer;
    const aiScene *m_Scene;

    glm::mat4 m_Proj;
    glm::mat4 m_View;

    float m_RotationSpeed;

    struct Mesh {
        std::shared_ptr<VertexArray> VAO;
        std::shared_ptr<VertexBuffer> VBO;
        std::shared_ptr<IndexBuffer> IBO;
        std::shared_ptr<Shader> Shader;
        std::shared_ptr<Texture> Texture;

        unsigned int MaterialIndex;

        glm::mat4 Model;
    };

    std::vector<Mesh> m_Meshes;
};

}