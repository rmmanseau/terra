#ifndef ENTITY_H
#define ENTITY_H

#include "common.h"
#include "component.h"
#include <unordered_map>
#include <string>

typedef std::unordered_map<std::type_index, std::shared_ptr<Component> > ComponentMap;

class Entity
{
    EntityId id;
    EntityType type;
    ComponentMap components;

public:
    // Entity() {}
    Entity(EntityId id, EntityType type);

    EntityId getId();
    EntityType getType();

    void addComponent(std::shared_ptr<Component> component);

    template <typename T>
    std::shared_ptr<T> getComponent()
    {
        std::type_index index(typeid(T));
        if (components.count(std::type_index(typeid(T))) != 0)
        {
            std::shared_ptr<T> ptr = std::static_pointer_cast<T>(components[index]);
            return ptr;
        }
        else
        {
            return std::shared_ptr<T>();
        }
    }
};

#endif // ENTITY_H
