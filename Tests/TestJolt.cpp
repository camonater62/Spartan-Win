#include "TestJolt.h"

#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui.h>

// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

// STL includes
#include <cstdarg>
#include <iostream>
#include <thread>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;

namespace test {

TestJolt::TestJolt()
    : m_CameraPosition(0.0f, 15.0f, 90.0f)
    , m_PhysicsTime(0.0f) {
    m_Camera.SetProjectionMatrix(
        glm::perspective(glm::radians(50.0f), 16.0f / 9.0f, 0.1f, 1000.0f));
    m_Camera.SetLookAt(m_CameraPosition, m_CameraPosition - glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

    m_Shader = std::make_unique<Shader>("res/shaders/Normal.shader");

    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    temp_allocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

    // We need a job system that will execute physics jobs on multiple threads. Typically
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    job_system = std::make_unique<JobSystemThreadPool>(
        cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() / 2);

    // Now we can create the actual physics system.
    physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
        broad_phase_layer_interface, object_vs_broadphase_layer_filter,
        object_vs_object_layer_filter);

    // // A body activation listener gets notified when bodies activate and go to sleep
    // // Note that this is called from a job so whatever you do here needs to be thread safe.
    // // Registering one is entirely optional.
    // MyBodyActivationListener body_activation_listener;
    // physics_system.SetBodyActivationListener(&body_activation_listener);

    // // A contact listener gets notified when bodies (are about to) collide, and when they separate again.
    // // Note that this is called from a job so whatever you do here needs to be thread safe.
    // // Registering one is entirely optional.
    // MyContactListener contact_listener;
    // physics_system.SetContactListener(&contact_listener);

    // The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
    // variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
    body_interface = &physics_system.GetBodyInterface();

    // Next we can create a rigid body to serve as the floor, we make a large box
    // Create the settings for the collision volume (the shape).
    // Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
    floor_shape_settings = BoxShapeSettings(Vec3(100.0f, 1.0f, 100.0f));

    // Create the shape
    floor_shape_result = floor_shape_settings.Create();
    floor_shape
        = floor_shape_result
              .Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

    // Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
    floor_settings = BodyCreationSettings(floor_shape, RVec3(0.0_r, -1.0_r, 0.0_r),
        Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);

    // Create the actual rigid body
    floor = body_interface->CreateBody(
        floor_settings); // Note that if we run out of bodies this can return nullptr

    // Add it to the world
    body_interface->AddBody(floor->GetID(), EActivation::DontActivate);

    // Now create a dynamic body to bounce on the floor
    // Note that this uses the shorthand version of creating and adding a body to the world
    // sphere_settings = BodyCreationSettings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r),
    //     Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
    // sphere_id = body_interface->CreateAndAddBody(sphere_settings, EActivation::Activate);
    createStack(JPH::Vec3(0.0_r, 30.0_r, 0.0_r), 100, 0.5f);

    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    // const float cDeltaTime = 1.0f / 60.0f;

    // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
    // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
    // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
    physics_system.OptimizeBroadPhase();
}

TestJolt::~TestJolt() {

    for (const auto &box : m_Boxes) {
        body_interface->RemoveBody(box);
        body_interface->DestroyBody(box);
    }

    // Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
    // body_interface->RemoveBody(sphere_id);

    // Destroy the sphere. After this the sphere ID is no longer valid.
    // body_interface->DestroyBody(sphere_id);

    // Remove and destroy the floor
    body_interface->RemoveBody(floor->GetID());
    body_interface->DestroyBody(floor->GetID());

    // Unregisters all types with the factory and cleans up the default material
    UnregisterTypes();

    // Destroy the factory
    delete Factory::sInstance;
    Factory::sInstance = nullptr;
}

void TestJolt::OnUpdate(float deltaTime) {
    auto tick = std::chrono::high_resolution_clock::now();

    const int cCollisionSteps = 1;
    physics_system.Update(deltaTime, cCollisionSteps, temp_allocator.get(), job_system.get());

    auto tock = std::chrono::high_resolution_clock::now();
    m_PhysicsTime = std::chrono::duration<float, std::chrono::seconds::period>(tock - tick).count();
}

void TestJolt::OnRender() {
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glDisable(GL_CULL_FACE)); // TODO: Remove this

    for (const auto &BodyID : m_Boxes) {
        Vec3 j_position = body_interface->GetCenterOfMassPosition(BodyID);
        Quat j_rotation = body_interface->GetRotation(BodyID);

        glm::mat4 translate = glm::translate(
            glm::mat4(1.0f), glm::vec3(j_position.GetX(), j_position.GetY(), j_position.GetZ()));
        glm::mat4 rotation = glm::mat4_cast(
            glm::quat(j_rotation.GetW(), j_rotation.GetX(), j_rotation.GetY(), j_rotation.GetZ()));

        glm::mat4 model = translate * rotation;

        glm::mat4 MVP = m_Camera.GetViewProjectionMatrix() * model;

        m_Cube.draw(MVP, m_Shader.get());
    }
}

void TestJolt::OnImGuiRender() {
    ImGui::Text("%ld boxes", m_Boxes.size());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
    ImGui::Text(
        "Physics sim took %.3f ms/frame (%.1f FPS)", 1000.0f * m_PhysicsTime, 1.0f / m_PhysicsTime);
}

void TestJolt::createStack(JPH::Vec3 transform, uint size, float halfExtent) {
    // Next we can create a rigid body to serve as the floor, we make a large box
    // Create the settings for the collision volume (the shape).
    // Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
    BoxShapeSettings box_shape_settings(Vec3(halfExtent, halfExtent, halfExtent));

    // Create the shape
    ShapeSettings::ShapeResult box_shape_result = box_shape_settings.Create();
    RefConst<Shape> box_shape
        = box_shape_result
              .Get(); // We don't expect an error here, but you can check box_shape_result for HasError() / GetError()

    for (uint i = 0; i < size; i++) {
        for (uint j = 0; j < size - i; j++) {
            Vec3 localTransform(
                Vec3((float) (j * 2) - (float) (size - i), (float) (i * 2 + 1), 0.0f) * halfExtent);
            Vec3 pos = transform + localTransform;

            // Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
            BodyCreationSettings box_settings(
                box_shape, pos, Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);

            // Create the actual rigid body
            BodyID box = body_interface->CreateAndAddBody(box_settings,
                EActivation::Activate); // Note that if we run out of bodies this can return nullptr

            m_Boxes.push_back(box);
        }
    }
}

}