#ifndef PTRANSLATE_H
#define PTRANSLATE_H

#include "common.h"
#include "process.h"
#include "grid.h"

#include "entity.h"
#include "cposition.h"
#include "ctranslate.h"

class PTranslate : public Process
{
    struct Node
    {
        EntityId id;
        EntityType type;

        std::shared_ptr<CPosition> position;
        std::shared_ptr<CTranslate> translate;
    };

    std::vector<Node> nodes;
    Grid& rGrid;

    static const std::map<Dir, Vec2f> dirMap;

public:
    PTranslate(Grid& grid);

    virtual void registerEntity(Entity& entity);
    virtual void unregisterEntity(EntityId id);
    virtual void update(int timeStep);
};

#endif // PTRANSLATE_H
