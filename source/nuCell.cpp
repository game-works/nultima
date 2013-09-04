#include "nuCell.h"
#include "nuBlock.h"
#include "nuWorld.h"
#include "nuDefs.h"
#include "nuContext.h"
#include "nuGraphics.h"

#include <cstring>

using namespace Nultima;

Cell::Cell(int x, int y) :
    m_x(x),
    m_y(y)
{
    memset(m_blocks, 0, sizeof(Block*)*NU_MAX_LAYERS*NU_CELL_WIDTH*NU_CELL_HEIGHT);
}

Cell::~Cell()
{
    for (int i=0; i<NU_MAX_LAYERS; i++)
    for (int y=0; y<NU_CELL_HEIGHT; y++)
    for (int x=0; x<NU_CELL_WIDTH; x++)
        if (m_blocks[i][y][x])
            delete m_blocks[i][y][x];
}

void Cell::serialize(std::ofstream* stream)
{
    stream->write((char*)&m_x, 4);
    stream->write((char*)&m_y, 4);

    for (int i=0; i<NU_MAX_LAYERS; i++)
    for (int y=0; y<NU_CELL_HEIGHT; y++)
    for (int x=0; x<NU_CELL_WIDTH; x++)
    {
        Block* block = m_blocks[i][y][x];
        if (block)
        {
            char tag = World::WORLD_TAG_BLOCK;
            stream->write(&tag, 1);
            stream->write((char*)&i, 4);
            stream->write((char*)&y, 4);
            stream->write((char*)&x, 4);
            block->serialize(stream);
        }
    }
}

void Cell::deserialize(std::ifstream* stream)
{
    memset(m_blocks, 0, sizeof(Block*)*NU_MAX_LAYERS*NU_CELL_WIDTH*NU_CELL_HEIGHT);

    stream->read((char*)&m_x, 4);
    stream->read((char*)&m_y, 4);

    char tag = -1;
    stream->read((char*)&tag, 1);
    while (tag == World::WORLD_TAG_BLOCK)
    {
        int layer, blockX, blockY;

        stream->read((char*)&layer, 4);
        stream->read((char*)&blockY, 4);
        stream->read((char*)&blockX, 4);
        
        NU_ASSERT(layer >= 0 && layer < NU_MAX_LAYERS);
        NU_ASSERT(blockY >= 0 && blockY < NU_CELL_HEIGHT);
        NU_ASSERT(blockX >= 0 && blockX< NU_CELL_WIDTH);

        Block* block = new Block();
        m_blocks[layer][blockY][blockX] = block;
        block->deserialize(stream);

        stream->read((char*)&tag, 1);
    }

    stream->seekg(-1, std::ios_base::cur); 
}

void Cell::insertBlock(char type, Vec2i coords, int layer)
{
    m_blocks[layer][coords.m_y][coords.m_x] = new Block(type, coords, layer);
}

void Cell::beginRendering()
{
    Graphics* graphics = Context::get()->getGraphics();
    graphics->pushMatrix();
    graphics->translate((float)m_x*NU_CELL_WIDTH, (float)m_y*NU_CELL_HEIGHT, 0);
}

void Cell::endRendering()
{
    Graphics* graphics = Context::get()->getGraphics();
    graphics->popMatrix();
}