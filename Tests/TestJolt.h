#pragma once

#include "Camera.h"
#include "Cube.h"
#include "Shader.h"
#include "Test.h"

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

namespace test {

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers {
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
    virtual bool ShouldCollide(
        JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
        using namespace JPH;
        switch (inObject1) {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING; // Non moving only collides with moving
        case Layers::MOVING: return true; // Moving collides with everything
        default: JPH_ASSERT(false); return false;
        }
    }
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers {
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::uint NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual unsigned int GetNumBroadPhaseLayers() const override {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        using namespace JPH;
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
        using namespace JPH;
        switch ((BroadPhaseLayer::Type) inLayer) {
        case (BroadPhaseLayer::Type) BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
        case (BroadPhaseLayer::Type) BroadPhaseLayers::MOVING: return "MOVING";
        default: JPH_ASSERT(false); return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    virtual bool ShouldCollide(
        JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
        using namespace JPH;
        switch (inLayer1) {
        case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING: return true;
        default: JPH_ASSERT(false); return false;
        }
    }
};

// An example contact listener
class MyContactListener : public JPH::ContactListener {
public:
    // See: ContactListener
    virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1,
        const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset,
        const JPH::CollideShapeResult &inCollisionResult) override {
        (void) inBody1;
        (void) inBody2;
        (void) inBaseOffset;
        (void) inCollisionResult;
        std::cout << "Contact validate callback" << std::endl;

        // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
        const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override {
        (void) inBody1;
        (void) inBody2;
        (void) inManifold;
        (void) ioSettings;
        std::cout << "A contact was added" << std::endl;
    }

    virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2,
        const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override {
        (void) inBody1;
        (void) inBody2;
        (void) inManifold;
        (void) ioSettings;
        std::cout << "A contact was persisted" << std::endl;
    }

    virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override {
        (void) inSubShapePair;
        std::cout << "A contact was removed" << std::endl;
    }
};

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener {
public:
    virtual void OnBodyActivated(const JPH::BodyID &inBodyID, unsigned long inBodyUserData)  {
        (void) inBodyID;
        (void) inBodyUserData;
        std::cout << "A body got activated" << std::endl;
    }

    virtual void OnBodyDeactivated(
        const JPH::BodyID &inBodyID, unsigned long inBodyUserData)  {
        (void) inBodyID;
        (void) inBodyUserData;
        std::cout << "A body went to sleep" << std::endl;
    }
};

class TestJolt : public Test {
public:
    TestJolt();
    ~TestJolt();
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:
    void createStack(JPH::Vec3 pos, unsigned int size, float halfExtent);

    Cube m_Cube;
    Camera m_Camera;
    glm::vec3 m_CameraPosition;
    std::unique_ptr<Shader> m_Shader;

    float m_PhysicsTime;

    JPH::BodyInterface *body_interface;
    JPH::Body *floor;
    JPH::BodyID sphere_id;
    std::unique_ptr<JPH::TempAllocatorImpl> temp_allocator;
    std::unique_ptr<JPH::JobSystemThreadPool> job_system;
    JPH::PhysicsSystem physics_system;
    JPH::BodyIDVector m_Boxes;

    // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
    const unsigned int cMaxBodies = 65536;

    // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
    const unsigned int cNumBodyMutexes = 0;

    // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
    // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
    // too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
    const unsigned int cMaxBodyPairs = 65536;

    // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
    // number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
    const unsigned int cMaxContactConstraints = 10240;

    // Create mapping table from object layer to broadphase layer
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    BPLayerInterfaceImpl broad_phase_layer_interface;

    // Create class that filters object vs broadphase layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

    // Create class that filters object vs object layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    ObjectLayerPairFilterImpl object_vs_object_layer_filter;

    JPH::BoxShapeSettings floor_shape_settings;
    JPH::ShapeSettings::ShapeResult floor_shape_result;
    JPH::ShapeRefC floor_shape;
    JPH::BodyCreationSettings floor_settings;
    JPH::BodyCreationSettings sphere_settings;
};
}