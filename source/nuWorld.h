#pragma once

#include "nuMapLocation.h"
#include "nuCell.h"

#include <string>
#include <unordered_map>

namespace Nultima
{

class World
{
public:
    World(std::string fileName);
    ~World() {}

    bool getPlayerStart(MapLocation& location) { (void)location; return true; }
    std::vector<Cell*> getCells() { return m_loadedCells; }

private:
    std::unordered_map<unsigned int, Cell*> m_cellMap;
    std::vector<Cell*>                      m_loadedCells;
};

}; // namespace
