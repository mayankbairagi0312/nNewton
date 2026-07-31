#pragma once
#include <nNewton/nTransform.hpp>
#include <nNewton/nCollisionShapes.hpp>
#include <cstdint>

enum class ComponentType : uint8_t {
    Transform,
    RigidBody,
    Color,
    Tag,
    Folder,
    ColliderShape,
    cCount
};

// ---------- component structs ----------
struct TransformComponent {
    nNewton::nTransform local;
};

struct RigidBodyComponent {
    float mass = 0.0f;
    bool  isStatic = true;
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

struct ColliderShapeComponent {
    nNewton::nCollisionShapeType type = nNewton::nCollisionShapeType::nBox;
};

// ---------- traits ----------
template<typename T> struct ComponentTraits;

#define REGISTER_COMPONENT(T, idx) \
    template<> struct ComponentTraits<T> { \
        static constexpr ComponentType Value = ComponentType::idx; \
    }

REGISTER_COMPONENT(TransformComponent, Transform);
REGISTER_COMPONENT(RigidBodyComponent, RigidBody);
REGISTER_COMPONENT(ColorComponent, Color);
REGISTER_COMPONENT(TagComponent, Tag);
REGISTER_COMPONENT(FolderComponent, Folder);
REGISTER_COMPONENT(ColliderShapeComponent, ColliderShape);

