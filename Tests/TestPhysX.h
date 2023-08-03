#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "Test.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <memory>

namespace test {

class TestPhysX : public Test {
public:
    TestPhysX();
    ~TestPhysX();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;
    std::unique_ptr<Shader> m_BlockShader;
    std::unique_ptr<Shader> m_GroundShader;
    std::unique_ptr<Texture> m_Texture;

    glm::mat4 m_Proj;
    glm::mat4 m_View;

    physx::PxDefaultAllocator m_DefaultAllocatorCallback;
    physx::PxDefaultErrorCallback m_DefaultErrorCallback;
    physx::PxDefaultCpuDispatcher *m_Dispatcher;
    physx::PxTolerancesScale m_TolerancesScale;

    physx::PxPhysics *m_Physics;
    physx::PxFoundation *m_Foundation;

    physx::PxScene *m_Scene;
    physx::PxMaterial *m_Material;

    physx::PxPvd *m_Pvd = nullptr;
    physx::PxPvdTransport *m_Transport;

    physx::PxRigidStatic *m_GroundPlane;
    std::vector<physx::PxRigidDynamic *> m_Boxes;

    void createStack(const physx::PxTransform &t, physx::PxU32 size, physx::PxReal halfExtent);
};

}