#include "TestVehicle.h"

#include "Macros.h"
#include "Renderer.h"
#include "Texture.h"

#include <PxPhysicsAPI.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui.h>
#include <iostream>
#include <thread>

namespace test {

TestVehicle::TestVehicle() {
    float vertices[] = {
        // Position (3) // Normal (3) // Tex coord (2)
        // Front
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.f, 0.0f, 0.0f, // 0
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.f, 1.0f, 0.0f, // 1
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.f, 1.0f, 1.0f, // 2
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.f, 0.0f, 1.0f, // 3
        // Back
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.f, 0.0f, 0.0f, // 4
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.f, 1.0f, 0.0f, // 5
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.f, 1.0f, 1.0f, // 6
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.f, 0.0f, 1.0f, // 7
        // Left
        -0.5f, -0.5f, -0.5f, -1.f, 0.0f, 0.f, 0.0f, 0.0f, // 8
        -0.5f, -0.5f, 0.5f, -1.f, 0.0f, 0.f, 1.0f, 0.0f, // 9
        -0.5f, 0.5f, 0.5f, -1.f, 0.0f, 0.f, 1.0f, 1.0f, // 10
        -0.5f, 0.5f, -0.5f, -1.f, 0.0f, 0.f, 0.0f, 1.0f, // 11
        // Right
        0.5f, -0.5f, -0.5f, 1.f, 0.0f, 0.f, 0.0f, 0.0f, // 12
        0.5f, -0.5f, 0.5f, 1.f, 0.0f, 0.f, 1.0f, 0.0f, // 13
        0.5f, 0.5f, 0.5f, 1.f, 0.0f, 0.f, 1.0f, 1.0f, // 14
        0.5f, 0.5f, -0.5f, 1.f, 0.0f, 0.f, 0.0f, 1.0f, // 15
        // Top
        -0.5f, 0.5f, -0.5f, 0.f, 1.0f, 0.f, 0.0f, 0.0f, // 16
        -0.5f, 0.5f, 0.5f, 0.f, 1.0f, 0.f, 1.0f, 0.0f, // 17
        0.5f, 0.5f, 0.5f, 0.f, 1.0f, 0.f, 1.0f, 1.0f, // 18
        0.5f, 0.5f, -0.5f, 0.f, 1.0f, 0.f, 0.0f, 1.0f, // 19
        // Bottom
        -0.5f, -0.5f, -0.5f, 0.f, -1.0f, 0.f, 0.0f, 0.0f, // 20
        -0.5f, -0.5f, 0.5f, 0.f, -1.0f, 0.f, 1.0f, 0.0f, // 21
        0.5f, -0.5f, 0.5f, 0.f, -1.0f, 0.f, 1.0f, 1.0f, // 22
        0.5f, -0.5f, -0.5f, 0.f, -1.0f, 0.f, 0.0f, 1.0f, // 23
    };
    unsigned int indices[] = {
        // Front
        0, 1, 2, // 0
        2, 3, 0, // 1
        // Back
        4, 5, 6, // 2
        6, 7, 4, // 3
        // Left
        8, 9, 10, // 4
        10, 11, 8, // 5
        // Right
        12, 13, 14, // 6
        14, 15, 12, // 7
        // Top
        16, 17, 18, // 8
        18, 19, 16, // 9
        // Bottom
        20, 21, 22, // 10
        22, 23, 20 // 11
    };

    m_VAO = std::make_unique<VertexArray>();
    // 6 faces of 4 vertices of 8 floats
    m_VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // UV
    m_VAO->AddBuffer(*m_VBO, layout);
    m_IBO = std::make_unique<IndexBuffer>(indices, 36);

    m_Proj = glm::perspective(glm::radians(45.f), 960.f / 540.f, 0.1f, 1000.f);
    m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, -4, -50));

    m_GroundShader = std::make_unique<Shader>("res/shaders/Normal.shader");

    m_BlockShader = std::make_unique<Shader>("res/shaders/QuestionBlock.shader");
    m_BlockShader->Bind();

    m_Texture = std::make_unique<Texture>("res/textures/question.png");
    m_BlockShader->SetUniform1i("u_Texture", 0);

    m_VAO->UnBind();
    m_VBO->UnBind();
    m_IBO->UnBind();
    m_BlockShader->UnBind();

    // init physx
    m_Foundation = PxCreateFoundation(
        PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback);
    if (!m_Foundation)
        throw("PxCreateFoundation failed!");

    m_TolerancesScale.length = 1;
    m_TolerancesScale.speed = 9.81f;

    m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_TolerancesScale, true, m_Pvd);
    physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    unsigned int numWorkers = std::max(1u, std::thread::hardware_concurrency() / 2);
    std::cout << "Using " << numWorkers << " PhysX workers" << std::endl;
    m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(numWorkers);
    sceneDesc.cpuDispatcher = m_Dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_Scene = m_Physics->createScene(sceneDesc);

#ifdef _DEBUG
    m_Pvd = PxCreatePvd(*m_Foundation);
    m_Transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    m_Pvd->connect(*m_Transport, physx::PxPvdInstrumentationFlag::eALL);

    physx::PxPvdSceneClient *pvdClient = m_Scene->getScenePvdClient();
    if (pvdClient) {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
#endif

    // create simulation
    m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
    m_GroundPlane = PxCreatePlane(*m_Physics, physx::PxPlane(0, 1, 0, 1), *m_Material);
    m_Scene->addActor(*m_GroundPlane);

    float halfExtent = 0.5f;
    physx::PxU32 size = 50;
    const physx::PxTransform t(physx::PxVec3(0, 10, 0));
    createStack(t, size, halfExtent);

    // {
    //     using namespace physx::vehicle2;
    //     PxInitVehicleExtension(*m_Foundation);

    //     PxVehicleFrame vehicleFrame;
    //     PxVehicleRigidBodyParams physxActorRigidBodyParams;
    //     /*
    //     const PxVehicleRigidBodyParams& _physxActorRigidBodyParams,
    //     const char* _physxActorName
    //     */
    //     PxVehiclePhysXRigidActorParams rigidActorParams(physxActorRigidBodyParams, "TestVehicle");
    //     physx::PxTransform rigidActorCmassLocalPose(physx::PxVec3(0, 0, 0));
    //     /*
    //     const physx::PxGeometry &_geometry,
    //     const physx::PxTransform &_localPose,
    //     const physx::PxMaterial &_material,
    //     const physx::PxShapeFlags _flags,
    //     const physx::PxFilterData &_simulationFilterData,
    //     const physx::PxFilterData &_queryFilterData
    //     */
    //     PxVehiclePhysXRigidActorShapeParams rigidActorShapeParams();
    //     PxVehiclePhysXActor *vehiclePhysXActor = nullptr;
    //     /*
    //     const physx::vehicle2::PxVehicleFrame &vehicleFrame,
    //     const physx::vehicle2::PxVehiclePhysXRigidActorParams &rigidActorParams,
    //     const physx::PxTransform &rigidActorCmassLocalPose,
    //     const physx::vehicle2::PxVehiclePhysXRigidActorShapeParams &rigidActorShapeParams,
    //     const physx::vehicle2::PxVehiclePhysXWheelParams &wheelParams,
    //     const physx::vehicle2::PxVehiclePhysXWheelShapeParams &wheelShapeParams,
    //     physx::PxPhysics &physics,
    //     const physx::PxCookingParams &params,
    //     physx::vehicle2::PxVehiclePhysXActor &vehiclePhysXActor)
    //     */
    //     PxVehiclePhysXActorCreate()
    // }
}

TestVehicle::~TestVehicle() {
    m_Scene->release();
    m_Dispatcher->release();
    m_Physics->release();
#ifdef _DEBUG
    m_Transport->release();
    m_Pvd->release();
#endif
    m_Foundation->release();
}

void TestVehicle::OnUpdate(float deltaTime) {
    m_Scene->simulate(deltaTime);
    m_Scene->fetchResults(true);
}

void TestVehicle::OnRender() {
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDisable(GL_CULL_FACE));

    Renderer renderer;

    { // Ground Plane
        physx::PxVec3 position = m_GroundPlane->getGlobalPose().p;
        physx::PxQuat rotation = m_GroundPlane->getGlobalPose().q;
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 0.5f, 100.0f));
        model *= glm::translate(
            glm::mat4(1.0f), glm::vec3(position.x, position.y - 1.5f, position.z));

        glm::mat4 mvp = m_Proj * m_View * model;
        m_GroundShader->Bind();
        m_GroundShader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IBO, *m_GroundShader);
    }

    m_Texture->Bind();

    for (const auto &box : m_Boxes) {
        physx::PxVec3 position = box->getGlobalPose().p;
        physx::PxQuat rotation = box->getGlobalPose().q;
        glm::mat4 model
            = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z))
              * glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));

        glm::mat4 mvp = m_Proj * m_View * model;
        m_BlockShader->Bind();
        m_BlockShader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IBO, *m_BlockShader);
    }
}

void TestVehicle::OnImGuiRender() {
    ImGui::Text(
        "%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void TestVehicle::createStack(
    const physx::PxTransform &t, physx::PxU32 size, physx::PxReal halfExtent) {
    physx::PxShape *shape = m_Physics->createShape(
        physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_Material);
    for (physx::PxU32 i = 0; i < size; i++) {
        for (physx::PxU32 j = 0; j < size - i; j++) {
            physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i),
                                           physx::PxReal(i * 2 + 1), 0)
                                       * halfExtent);
            physx::PxRigidDynamic *body = m_Physics->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            physx::PxRigidBodyExt::updateMassAndInertia(*body, 10000.0f);
            m_Scene->addActor(*body);
            m_Boxes.push_back(body);
        }
    }
    shape->release();
}

}