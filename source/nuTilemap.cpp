#include "nuDefs.h"
#include "nuTilemap.h"
#include "nuFileUtils.h"
#include "nuLog.h"
#include "rapidjson/document.h"

#include <string>
#include <iostream>

//#if NU_OS == NU_OS_WINDOWS
//#include <Windows.h>
//#endif

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut/include/GL/glut.h"
#endif

#define STBI_HEADER_FILE_ONLY
#include "stb/stb_image.c"

using namespace Nultima;

Tilemap::Tilemap()
{
    m_tiles.resize(512);
    for (int i = 0; i < 512; i++) {
        BlockInfo b;
        b.textureId = -1;
        m_tiles[i] = b;
    }
}

Tilemap::~Tilemap()
{
}

void Tilemap::load(std::string fname)
{
    rapidjson::Document doc;

    char* jsonBlob = FileUtils::readFile(fname);
    assert(jsonBlob != NULL);
    doc.Parse<0>(jsonBlob);
    delete jsonBlob;
    assert(!doc.HasParseError());

    // parse config
    m_tileSize = doc["config"]["tilesize"].GetInt();
    
    std::string tilemapFile = doc["config"]["tilemap"].GetString();

    // load tilemap texture
    unsigned char* tilemapData; 
    int tilemapWidth, tilemapHeight, tilemapBpp;
    int tilesPerLine;
    tilemapData = stbi_load(tilemapFile.c_str(), &tilemapWidth, &tilemapHeight, &tilemapBpp, 0);
    tilesPerLine = tilemapWidth / m_tileSize;
    Log::msg("Tilemap size = %dx%d, bpp=%d, tilesPerLine=%d\n", tilemapWidth, tilemapHeight, tilemapBpp, tilesPerLine);

    unsigned char* tempTexture = new unsigned char[m_tileSize*m_tileSize*3];

    // TODO [sampo] move to graphics (buildTexture())
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // process tile info 
    // for each tile we'll create a m_tileSize^2 texture
    const rapidjson::Value& tiles = doc["tiles"];
    
    Log::msg("Processing %d tiles...\n", tiles.Size());
    for (rapidjson::SizeType i = 0; i < tiles.Size(); i++)
    {
        const rapidjson::Value& tile = tiles[i];
        int idx = tile["id"].GetInt();
        std::string textureId = tile["name"].GetString();
        bool isSolid = tile["solid"].GetBool();

        // (x1,y1)-(x2,y2) define tile data in tilemap
        int x1 = idx%tilesPerLine * m_tileSize;
        int y1 = idx/tilesPerLine * m_tileSize;
        int x2 = x1+m_tileSize;
        int y2 = y1+m_tileSize;

        // TODO this is shit
        unsigned int sRed = 0;
        unsigned int sGreen = 0;
        unsigned int sBlue = 0;

        for (int y = y1, ofs=0; y < y2; y++)
        {
            for (int x = x1; x < x2; x++)
            {
                tempTexture[ofs] = tilemapData[y*tilemapWidth*3 + x*3];
                tempTexture[ofs+1] = tilemapData[y*tilemapWidth*3 + x*3+1];
                tempTexture[ofs+2] = tilemapData[y*tilemapWidth*3 + x*3+2];

                sRed += tempTexture[ofs];
                sGreen += tempTexture[ofs+1];
                sBlue += tempTexture[ofs+2];

                ofs += 3;
            }
        }

        sRed /= (m_tileSize*m_tileSize);
        sGreen /= (m_tileSize*m_tileSize);
        sBlue /= (m_tileSize*m_tileSize);
        Vec3ui tileColor = Vec3ui(sRed, sGreen, sBlue);

        unsigned int tempTextureId;
        glGenTextures(1, &tempTextureId);
        glBindTexture(GL_TEXTURE_2D, tempTextureId);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_tileSize, m_tileSize, GL_RGB, GL_UNSIGNED_BYTE, tempTexture);

        BlockInfo info;
        info.textureId   = tempTextureId;
        info.isSolid     = isSolid;
        info.color       = tileColor;
        info.name        = tile["name"].GetString();
        m_tiles[idx]     = info;

        // store name<->id mapping
        m_tileNameIdMap[tile["name"].GetString()] = idx; //tile["id"].GetInt();

        Log::msg("\t%s -> %d\n", tile["name"].GetString(), idx);

    }
    Log::msg("done\n");
    Log::msg("tilemap numtiles = %d\n", (int)m_tiles.size());

    delete[] tempTexture;

    free(tilemapData);
}

int Tilemap::getNextTile(int n, int d)
{
    int ret = n+d;
    while (ret != n)
    {
        if (ret < 0) 
            ret = m_tiles.size()-1;
        if (ret >= (int)m_tiles.size())
            ret = 0;
        if (m_tiles[ret].textureId >= 0)
            break;

        ret += d;
    }
    return ret;
}
