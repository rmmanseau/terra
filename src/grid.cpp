#include "grid.h"

Grid::GridInfo::GridInfo()
    : id(-1), type(EntityType::Empty) {}

Grid::Grid(int w, int h)
    : width(w)
    , height(h)
{
    int size = width*height;
    for (int i = 0; i < size; ++i)
        g.push_back(GridInfo());
}

Grid::GridInfo& Grid::at(Vec2 v)
{
    return g[v.y*width + v.x];
}

bool Grid::inside(Vec2 v)
{
    return v.x >= 0 && v.y >= 0 && v.x < width && v.y < height;
}

EntityId Grid::getIdAt(Vec2 v)
{
    if (inside(v))
        return at(v).id;
    else
        return -1;
}

EntityType Grid::getTypeAt(Vec2 v)
{
    if (inside(v))
        return at(v).type;
    else
        return EntityType::Null;
}

void Grid::setInfoAt(Vec2 v, EntityId id, EntityType type)
{
    if (inside(v))
    {
        at(v).id = id;
        at(v).type = type;
    }
}

void Grid::erase(Vec2 v)
{
    setInfoAt(v, -1, EntityType::Empty);
}

bool Grid::empty(Vec2 v)
{
    return at(v).type == EntityType::Empty;
}
