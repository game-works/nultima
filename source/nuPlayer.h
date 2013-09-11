#pragma once

#include "nuMapLocation.h"

namespace Nultima
{

class World;
class Camera;

class Player
{
public:
    Player(MapLocation location);
    ~Player() {};

    void        render(World* world, Camera* camera);
    void        move  (Vec3i d);

private:
    MapLocation m_location;
};

};
