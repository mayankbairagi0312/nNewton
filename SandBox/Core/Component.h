#pragma once
#include <nNewton/nTransform.hpp>
#include <nNewton/nRigidBody.hpp>
#include <nNewton/nCollisionShapes.hpp>
#include <cstdint>

enum class ComponentType : uint8_t {
    Transform,
    Physics,
    Color,
    Tag,
    Folder,
    Count
};

// ---------- Component structs ----------

struct TransformComponent {
    nNewton::nTransform local;
};

struct PhysicsComponent {
    float mass = 0.0f;
    nNewton::nBodyType Type = nNewton::nBodyType::Static;
    bool HasCollider = false;
    nNewton::nCollisionShapeType ShapeType = nNewton::nCollisionShapeType::nBox;
};

struct ColorComponent {
    nNewton::nVector4 color{ 0.2f, 0.7f, 0.8f, 1.0f };
};

struct TagComponent {
    char name[64] = "Entity";
};

struct FolderComponent {
    int folderId = -1;
};

// ---------- Type traits ----------

template<typename T> struct ComponentTraits;

#define REGISTER_COMPONENT(T, idx) \
    template<> struct ComponentTraits<T> { \
        static constexpr ComponentType Value = ComponentType::idx; \
    }

REGISTER_COMPONENT(TransformComponent, Transform);
REGISTER_COMPONENT(PhysicsComponent, Physics);
REGISTER_COMPONENT(ColorComponent, Color);
REGISTER_COMPONENT(TagComponent, Tag);
REGISTER_COMPONENT(FolderComponent, Folder);
