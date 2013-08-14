#include <assert.h>
#include "Cell.h"
#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"
#include "textures.h"
#include "gl_utils.h"

extern TextureManager *textures;

Cell::Cell() {
    inMemory = false;
    x = y = 0.0f;
}

void Cell::load(char *fname) {
    int mapWidth, mapHeight, bpp;    
    unsigned char *mapData;

    mapData = stbi_load(fname, &mapWidth, &mapHeight, &bpp, 3);

    assert(mapWidth==CELL_SIZE && mapHeight==CELL_SIZE);

    displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);

    for (int y = 0, ofs = 0; y < CELL_SIZE; y++) {
	for (int x = 0; x < CELL_SIZE; x++) {
	    int r = mapData[ofs++];
	    int g = mapData[ofs++];
	    int b = mapData[ofs++];
	    int p = (r << 16) | (g << 8) | b;

	    switch (p) {

		case BLOCK_WATER: 
		    glBindTexture(GL_TEXTURE_2D, textures->get("water")); 
		    break;
		case BLOCK_GRASS: 
		    glBindTexture(GL_TEXTURE_2D, textures->get("grass")); 
		    break;
		case BLOCK_ROCK: 
		    glBindTexture(GL_TEXTURE_2D, textures->get("rock")); 
		    break;
		case BLOCK_WOOD: 
		    glBindTexture(GL_TEXTURE_2D, textures->get("wood")); 
		    break;

	    default: continue;
	    }

	    glPushMatrix();
	    glTranslatef((float)x, (float)y, 0.0f);
	    texturedCube(1.0);
	    glPopMatrix();
	}
    }
    stbi_image_free(mapData);
    glEndList();

    inMemory = true;
}

void Cell::unload() {
    glDeleteLists(displayList, 1);
    inMemory = false;
}

void Cell::render() {
    // TODO should assert?
    if (inMemory) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glCallList(displayList);
	glPopMatrix();
    }
}

void Cell::move(float _x, float _y) {
    x = _x;
    y = _y;
}
