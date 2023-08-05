#include "TestJolt.h"

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

TestJolt::TestJolt() {

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
        cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

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
    sphere_settings = BodyCreationSettings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r),
        Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
    sphere_id = body_interface->CreateAndAddBody(sphere_settings, EActivation::Activate);

    // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
    // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
    body_interface->SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));

    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    // const float cDeltaTime = 1.0f / 60.0f;

    // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
    // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
    // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
    physics_system.OptimizeBroadPhase();
}

TestJolt::~TestJolt() {
    // Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
    body_interface->RemoveBody(sphere_id);

    // Destroy the sphere. After this the sphere ID is no longer valid.
    body_interface->DestroyBody(sphere_id);

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

    if (body_interface->IsActive(sphere_id)) {
        // Output current position and velocity of the sphere
        RVec3 position = body_interface->GetCenterOfMassPosition(sphere_id);
        Vec3 velocity = body_interface->GetLinearVelocity(sphere_id);
        cout << "Position = (" << position.GetX() << ", " << position.GetY() << ", "
             << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY()
             << ", " << velocity.GetZ() << ")" << endl;

        // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
        const int cCollisionSteps = 1;

        // Step the world
        physics_system.Update(deltaTime, cCollisionSteps, temp_allocator.get(), job_system.get());
    }
}

void TestJolt::OnRender() {
}

void TestJolt::OnImGuiRender() {
}
}