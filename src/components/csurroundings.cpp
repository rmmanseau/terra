#include "csurroundings.h"
#include "glbl_helpers.h"

SRC_FACTORY_REGISTER(CSurroundings);

bool CSurroundings::init(YAML::Node node)
{
    for (int i = 0; i < 8; ++i)
    {
        ids.push_back(0);
        types.push_back(glbl::constants.eTypeNull());
    }

    return true;
}

void CSurroundings::setInfo(int dir, EntityId id, EntityType type)
{
    ids[dir] = id;
    types[dir] = type;
}

int CSurroundings::numberOf(EntityType type)
{
    return glbl::helpers.vecContains(types, type);
}

int CSurroundings::numberOf(std::vector<EntityType> otherTypes)
{
    return glbl::helpers.vecContains(types, otherTypes);
}

int CSurroundings::numberFull()
{
    return 8 - numberOf(glbl::constants.eTypeEmpty());
}

Dir CSurroundings::directionTowards(EntityType type)
{
    std::vector<Dir> validDirs;
    for (int i = 0; i < types.size(); ++i)
    {
        if (types[i] == type)
            validDirs.push_back((Dir)i);
    }
    return validDirs.at(rand() % validDirs.size());
}

Dir CSurroundings::directionTowards(std::vector<EntityType> otherTypes)
{
    std::vector<Dir> validDirs;
    for (int i = 0; i < types.size(); ++i)
    {
        for (int j = 0; j < otherTypes.size(); ++j)
        {
            if (types[i] == otherTypes[j])
                validDirs.push_back((Dir)i);
        }
    }
    return validDirs.at(rand() % validDirs.size());
}

EntityId CSurroundings::idAt(Dir dir)
{
    return ids.at((int)dir);
}

EntityType CSurroundings::typeAt(Dir dir)
{
    return types.at((int)dir);
}
