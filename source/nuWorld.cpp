#include "nuDefs.h"
#include "nuWorld.h"
#include "nuBlock.h"
#include "nuCell.h"
#include "nuContext.h"
#include "nuTexManager.h"
#include "nuTilemap.h"
#include "nuVec3.h"

#include <fstream>

#define STBI_HEADER_FILE_ONLY
#include "stb/stb_image.c"

using namespace Nultima;

// TODO [sampo] streaming: unique tags for each block in the data 

World::World(std::string fileName)
{
    // TODO [sampo] use fileUtils
    std::ifstream worldFile(fileName.c_str(), std::ios_base::binary);
    if (worldFile.is_open())
    {
        deserialize(&worldFile);
        worldFile.close();
    }
}

void World::generateFromPNG(std::string fname)
{
    int w, h, bpp;
    unsigned char* data = stbi_load(fname.c_str(), &w, &h, &bpp, 0);

    TexManager* tex = Context::get()->getTexManager();
    Tilemap* tilemap = tex->getTilemap();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {

            int ofs = (y*w + x)*bpp;
            int p = 0;
            for (int b = 0; b < bpp; b++)
                p = (p << 8) | data[ofs+b];

            Vec3i loc(x, h-y-1, 0);

/*
0x0000ff = water
0x00ff00 = park (using grassland2)
0xffffff = generic ground (using grassland1)
0x00ffff = Unkown (using mountain_small)
0xffff00 = highway / road (using road_vertical)
0xff00ff = Unkown (using forest_normal)
*/
            if (p == 0x0000ff)      insertBlock(new Block(tilemap->getTileIndex("sea_normal"), loc));
            else if (p == 0x00ff00) insertBlock(new Block(tilemap->getTileIndex("grassland"), loc));
            else if (p == 0xffffff) insertBlock(new Block(tilemap->getTileIndex("plains"), loc));
            else if (p == 0x00ffff) insertBlock(new Block(tilemap->getTileIndex("mountain_medium"), loc));
            else if (p == 0xffff00) insertBlock(new Block(tilemap->getTileIndex("road_vert"), loc));
            else if (p == 0xff00ff) insertBlock(new Block(tilemap->getTileIndex("forest_normal"), loc));
            else
            {
                NU_ASSERT(!"unkown block type");
            }
        }
    }

    free(data);
}

void World::serialize(std::ofstream* stream)
{
    char cWrite = WORLD_TAG_VERSION;
    stream->write(&cWrite, 1);
    cWrite = VERSION_BLOCK_REPRESENTATION;
    stream->write(&cWrite, 1);

    cWrite = WORLD_TAG_PLAYERSTART;
    stream->write(&cWrite, 1);
    Vec3i playerStart(1, 3, 1);
    playerStart.serialize(stream);

    for (std::tr1::unordered_map<unsigned int, Cell*>::iterator it = m_cellMap.begin(); it != m_cellMap.end(); ++it)
    {
        Cell* cell = it->second;
        if (cell)
            cell->serialize(stream);
    }

    cWrite = WORLD_TAG_END;
    stream->write(&cWrite, 1);
}

void World::deserialize(std::ifstream* stream)
{
    bool finished = false;
    while (!finished)
    {
        char tag = -1;
        stream->read(&tag, 1);
        switch (tag)
        {
        case WORLD_TAG_VERSION:
            {
                stream->read(&m_version, 1);
                printf("World file format version: %d\n", m_version);
                break;
            }

        case WORLD_TAG_PLAYERSTART:
            {
                if (m_version == World::VERSION_INITIAL)
                {
                    int idx;
                    stream->read((char*)&idx, 4);
                    stream->read((char*)&m_playerStart.m_x, 4);
                    stream->read((char*)&m_playerStart.m_y, 4);
                    stream->read((char*)&m_playerStart.m_z, 4);
                }
                else
                {
                    m_playerStart.deserialize(stream);
                }
                break;
            }

        case WORLD_TAG_CELL:
            {
                Cell* cell = new Cell();
                cell->deserialize(stream, m_version);
                m_cellMap[cell->getIndex()] = cell;
                break;
            }

        case WORLD_TAG_END:
            finished = true;
            break;

        default:
            NU_ASSERT(!"Unkown World serialization tag");
        }
    }
}

void World::insertBlock(Block* block)
{
    Vec3i blockLocation = block->getLocation();
    unsigned int cellIdx = Cell::indexAtLocation(blockLocation);
    
    // Create new Cell if needed
    if (!m_cellMap[cellIdx])
    {
        Vec2i cellCoords(blockLocation.m_x, blockLocation.m_y);
        m_cellMap[cellIdx] = new Cell(cellCoords);
    }

    m_cellMap[cellIdx]->insertBlock(block);
}

void World::clearBlock(Vec3i location)
{
    unsigned int cellIdx = Cell::indexAtLocation(location);
    if (m_cellMap[cellIdx])
        m_cellMap[cellIdx]->clearBlock(location);
}

Cell* World::getCellAt(Vec3i v)
{
    unsigned int idx = Cell::indexAtLocation(v);
    return m_cellMap[idx];
}

Block* World::getBlockAt(Vec3i v)
{
    unsigned int idx = Cell::indexAtLocation(v);
    Cell* cell = m_cellMap[idx];
    return (cell) ? (Block*)cell->getBlock(v) : NULL;
}

Vec2i World::getMinCoordinate()
{
    Vec2i min(0,0);
    for (std::tr1::unordered_map<unsigned int, Cell*>::iterator it = m_cellMap.begin(); it != m_cellMap.end(); ++it)
    {
        Cell* cell = it->second;
        if (cell)
        {
            Vec2i position = cell->getPosition();
            if (position.m_x < min.m_x)
                min.m_x = position.m_x;
            if (position.m_y < min.m_y)
                min.m_y = position.m_y;
        }
    }
    return min;
}

Vec2i World::getMaxCoordinate()
{
    Vec2i max(0,0);
    for (std::tr1::unordered_map<unsigned int, Cell*>::iterator it = m_cellMap.begin(); it != m_cellMap.end(); ++it)
    {
        Cell* cell = it->second;
        if (cell)
        {
            Vec2i position = cell->getPosition();
            if (position.m_x > max.m_x)
                max.m_x = position.m_x;
            if (position.m_y > max.m_y)
                max.m_y = position.m_y;
        }
    }
    return max;
}
