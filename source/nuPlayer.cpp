#include "nuPlayer.h"
#include "nuWorld.h"
#include "nuGraphics.h"
#include "nuContext.h"
#include "nuCamera.h"
#include "nuObject.h"
#include "nuBlock.h"
#include "nuCell.h"
#include "nuCharacter.h"
#include "nuLight.h"

using namespace Nultima;

Player::Player(MapLocation location)
{
    m_location = location;
    m_avatar = new Character();
}

// TODO [sampo] frustum culling
void Player::render(World* world, Camera* inCamera, Light* light)
{
    // TODO [sampo] tick outside?
    m_avatar->tick();

    Graphics* graphics = Context::get()->getGraphics();
    graphics->setPerspectiveProjection();

    // place camera
    Camera playerCamera(m_location);
    Camera* camera = (inCamera) ? inCamera : &playerCamera;
    camera->setView();

    graphics->enableLighting();
    graphics->setLight(light);

    std::tr1::unordered_map<unsigned int, Cell*> cells = world->getCells();

    Vec3i pos = m_location.m_position;

    // loop cells
    for (std::tr1::unordered_map<unsigned int, Cell*>::iterator it = cells.begin(); it != cells.end(); ++it)
    // loop layers
    for (int i=0; i<NU_MAX_LAYERS; i++)
    {
        Cell* cell = it->second;
        // loop blocks

        if (cell)
        {
            for (int x=0; x<NU_CELL_WIDTH; x++)
            for (int y=0; y<NU_CELL_HEIGHT; y++)
            {
                // TODO [muumi] do not render blocks that are above player
                /*
                if (i > pos.m_z && x == pos.m_x && y == pos.m_y)
                    continue;
                */
                const Block* block = cell->getBlock(Vec3i(x, y, i));
                if (block)
                    block->render();
            }
        }
    }

    // Render player
    graphics->pushMatrix();
    graphics->translate((float)m_location.m_position.m_x, (float)m_location.m_position.m_y, (float)m_location.m_position.m_z+0.5f);
    m_avatar->render();
    graphics->popMatrix();

    // TODO [sampo] loop inhabitants
    //     render inhabitant

    // TODO [sampo] loop props
    //     render props

    // TODO [sampo] loop monsters
    //   render monster

    graphics->disableLighting();
}

void Player::move(Vec3i d)
{
    m_location.move(d);
}
