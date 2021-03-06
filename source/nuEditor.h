#pragma once

#include "nuVec2.h"
#include "nuVec3.h"
#include "nuDefs.h"

namespace Nultima
{

    class Camera;
    class World;
    class Minimap;
    class Block;

/*
 * requirements:
 * 
 * - move cursor around in the world
 * - move up&down layers
 * - add cells?
 * - change block at (x,y,layer)
 * - view list of blocks
 * - load & save world
 * 
 */
class Editor
{

    typedef enum 
    {
        EDITMODE_NONE = 0,
        EDITMODE_PAINT,
        EDITMODE_ERASE,

        // these three should be in sequence
        EDITMODE_ROAD,
        EDITMODE_WALL,
        EDITMODE_RIVER,

        EDITMODE_LAST

    } EditMode;

public:
    Editor(World *w);
    ~Editor();

    void    render();
    void    handleKeypress(int key);
    void    handleMouse();
    Camera* getCamera();
    Vec3i   getCursorPosition() { return m_cursor; }

private:
    std::string getEditModeName();
    void        cycleCursorRepresentation();

    void    paintCurrentBlock();
    void    eraseCurrentBlock();
    void    changeActiveBlockBy(char);
    void    changeEditMode(EditMode m);
    int     getBlocksetStart(int idx);
    
    void    moveCamera(Vec3 d);
    void    updateCameraPosition();

    void    changeCullDistanceBy(float d);
    void    moveSelection(Vec3i d);
    void    saveWorld();
    void    renderHud();
    void    renderActiveBlock();
    
    void    dropPlayer();

    Camera*     m_camera;
    Vec3        m_cameraOffset;
    World*      m_world;
    EditMode    m_editMode;
    bool        m_helpActive;

    Vec3i       m_cursor;
    int         m_cursorType;
    NuUInt8     m_cursorRepresentation;
    Minimap*    m_minimap;
};

};
