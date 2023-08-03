#include "TestAssimp.h"

#include "Macros.h"
#include "Renderer.h"
#include "imgui.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>

namespace test {

TestAssimp::TestAssimp()
    : m_RotationSpeed(1.0f) {
    m_Scene = m_Importer.ReadFile("res/models/Lambo.glb",
        aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
            | aiProcess_SortByPType | aiProcess_OptimizeGraph | aiProcess_GenNormals
            | aiProcess_GenUVCoords | aiProcess_GenBoundingBoxes | aiProcess_ValidateDataStructure
            | aiProcess_ImproveCacheLocality);

    if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << m_Importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int meshIndex = 0; meshIndex < m_Scene->mNumMeshes; meshIndex++) {
        aiMesh *mesh = m_Scene->mMeshes[meshIndex];

        Mesh m;

        VertexBufferLayout layout = VertexBufferLayout();
        layout.Push<float>(3); // Position
        // layout.Push<float>(3); // Normal
        layout.Push<float>(2); // Texture coordinates

        std::vector<float> vertices;
        vertices.reserve(mesh->mNumVertices * layout.GetStride());
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            // if (mesh->HasNormals()) {
            //     vertices.push_back(mesh->mNormals[i].x);
            //     vertices.push_back(mesh->mNormals[i].y);
            //     vertices.push_back(mesh->mNormals[i].z);
            // } else {
            //     vertices.push_back(0.0f);
            //     vertices.push_back(0.0f);
            //     vertices.push_back(0.0f);
            // }
            if (mesh->HasTextureCoords(0)) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
            // std::cout << "Vertex " << i << ": " << mesh->mVertices[i].x << ", "
            //           << mesh->mVertices[i].y << ", " << mesh->mVertices[i].z << std::endl;
        }

        m.VAO = std::make_unique<VertexArray>();
        m.VBO = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

        m.VAO->AddBuffer(*m.VBO, layout);
        std::vector<unsigned int> indices;
        indices.reserve(mesh->mNumFaces * 3);
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace *face = &mesh->mFaces[i];
            unsigned int count = face->mNumIndices;
            // std::cout << "Face " << i << " (" << count << "): ";
            for (unsigned int j = 0; j < count; j++) {
                indices.push_back(face->mIndices[j]);
                // std::cout << face->mIndices[j] << " ";
            }
            // std::cout << std::endl;
        }
        m.IBO = std::make_unique<IndexBuffer>(indices.data(), indices.size());
        // std::cout << "This mesh has " << indices.size() << " m_Indices!" << std::endl;

        m.Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

        m.Shader = std::make_unique<Shader>("res/shaders/BasicTexture.shader");
        m.Shader->Bind();

        m.VAO->UnBind();
        m.VBO->UnBind();
        m.IBO->UnBind();
        m.Shader->UnBind();

        m.MaterialIndex = mesh->mMaterialIndex;
        aiMaterial *material = m_Scene->mMaterials[mesh->mMaterialIndex];
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        const aiTexture *diffuseTexture = m_Scene->GetEmbeddedTexture(path.C_Str());
        // std::cout << "Diffuse Texture: " << path.C_Str() << std::endl;
        material->GetTexture(aiTextureType_SPECULAR, 0, &path);
        const aiTexture *specularTexture = m_Scene->GetEmbeddedTexture(path.C_Str());
        // std::cout << "Specular Texture: " << path.C_Str() << std::endl;

        for (const auto &texture : { specularTexture, diffuseTexture }) {
            if (texture) {
                if (texture->mHeight == 0) {
                    m.Texture = std::make_unique<Texture>(
                        (unsigned char *) texture->pcData, texture->mWidth);
                    // std::cout << "Texture width: " << m.Texture->GetWidth() << std::endl;
                    // std::cout << "Texture height: " << m.Texture->GetHeight() << std::endl;
                } else {
                    std::cout << "TODO!!" << std::endl;
                    // m.Texture = std::make_unique<Texture>(texture->pcData, texture->mWidth, texture->mHeight);
                }
            }
        }

        m_Meshes.push_back(m);
    }

    m_Proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.f);
    m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -10.0f));
}

TestAssimp::~TestAssimp() {
}

void TestAssimp::OnUpdate(float deltaTime) {
    (void) deltaTime;
    (void) m_RotationSpeed;
    glm::vec3 axis = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    float angle = m_RotationSpeed * deltaTime * glm::radians(45.f);
    for (auto &mesh : m_Meshes) {
        mesh.Model = glm::rotate(mesh.Model, angle, axis);
    }
}

void TestAssimp::OnRender() {
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));
    GLCall(glEnable(GL_DEPTH_TEST));

    Renderer renderer;

    for (const auto &mesh : m_Meshes) {

        glm::mat4 mvp = m_Proj * m_View * mesh.Model;
        mesh.Shader->Bind();
        mesh.Shader->SetUniformMat4f("u_MVP", mvp);
        if (mesh.Texture) {
            mesh.Texture->Bind();
        }
        renderer.Draw(*mesh.VAO, *mesh.IBO, *mesh.Shader);
    }
}

void TestAssimp::OnImGuiRender() {
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
    ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 20.0f);
    ImGui::Separator();
    ImGui::Text("Meshes: %d", m_Scene->mNumMeshes);
    ImGui::Separator();
    for (unsigned int i = 0; i < m_Scene->mNumMeshes; i++) {
        ImGui::Text("Name: %s", m_Scene->mMeshes[i]->mName.C_Str());
        ImGui::Text("m_Vertices: %d", m_Scene->mMeshes[i]->mNumVertices);
        ImGui::Text("Faces: %d", m_Scene->mMeshes[i]->mNumFaces);
        ImGui::Text("Material Index: %d", m_Scene->mMeshes[i]->mMaterialIndex);
        ImGui::Text("Bones: %d", m_Scene->mMeshes[i]->mNumBones);
        ImGui::Text("Has Normals: %s", m_Scene->mMeshes[i]->HasNormals() ? "true" : "false");
        ImGui::Text("Has Tangents and Bitangents: %s",
            m_Scene->mMeshes[i]->HasTangentsAndBitangents() ? "true" : "false");
        ImGui::Text("Has Texture Coordinates: %s",
            m_Scene->mMeshes[i]->HasTextureCoords(0) ? "true" : "false");
        ImGui::Text(
            "Has Vertex Colors: %s", m_Scene->mMeshes[i]->HasVertexColors(0) ? "true" : "false");

        ImGui::Separator();
    }

    ImGui::Text("Materials: %d", m_Scene->mNumMaterials);
    ImGui::Separator();
    for (unsigned int i = 0; i < m_Scene->mNumMaterials; i++) {
        ImGui::Text("Name: %s", m_Scene->mMaterials[i]->GetName().C_Str());
        for (unsigned int j = 0; j < m_Scene->mMaterials[i]->mNumProperties; j++) {
            ImGui::Text("Property Name: %s", m_Scene->mMaterials[i]->mProperties[j]->mKey.C_Str());
            ImGui::Text("Property Type: %d", m_Scene->mMaterials[i]->mProperties[j]->mType);
            ImGui::Text("Property Index: %d", m_Scene->mMaterials[i]->mProperties[j]->mIndex);
            ImGui::Text(
                "Property Data Length: %d", m_Scene->mMaterials[i]->mProperties[j]->mDataLength);
            ImGui::Separator();
        }
        ImGui::Separator();
    }

    ImGui::Text("Textures: %d", m_Scene->mNumTextures);
    ImGui::Separator();
    for (unsigned int i = 0; i < m_Scene->mNumTextures; i++) {
        ImGui::Text("Name: %s", m_Scene->mTextures[i]->mFilename.C_Str());
        ImGui::Text("Width: %d", m_Scene->mTextures[i]->mWidth);
        ImGui::Text("Height: %d", m_Scene->mTextures[i]->mHeight);
        ImGui::Text("Format Hint: %s", m_Scene->mTextures[i]->achFormatHint);
        ImGui::Text("Data Length: %d", m_Scene->mTextures[i]->mWidth);
        ImGui::Separator();
    }

    ImGui::Text("Lights: %d", m_Scene->mNumLights);
    ImGui::Text("Cameras: %d", m_Scene->mNumCameras);
    ImGui::Text("Animations: %d", m_Scene->mNumAnimations);
}

}
