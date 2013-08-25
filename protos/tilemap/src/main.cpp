#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "stb_image.c"

#include "world.h"
#include "gl_utils.h"
#include "camera.h"
#include "light.h"
#include "Cell.h"
#include "textures.h"
#include "Player.h"
#include "keyboard.h"

#define FRAND() ((float)rand() / RAND_MAX)

using namespace std;

void cleanup(void);	  
void buildWorld(void);
void movePlayer(int x, int y);
void handleKeys();

static int wWidth = 800;
static int wHeight = 600;
static float angle = .0f;

double timer = 0.f;
double fps = 12.f;
double timerDelta = 0.f;
double lastFrame = 0.f;

Camera *cam;
Light *light;
GLuint worldList;
Player *player;

TextureManager *textures;

std::vector<Cell> cells; //[WORLD_SIZE];

static void drawStats()
{
    char string[128];
    sprintf(string, "fps: %4.0f (%4.1f ms)", 1000/fps, timerDelta);
    int i, len; 
   
    glColor4f(1, 1, 1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0.0, wWidth, 0.0, wHeight);
    //glTranslatef(0.0f, 0.0f, -5.0f);
    
    glRasterPos2i(20, wHeight-20);
    
    glDisable(GL_TEXTURE);
    glDisable(GL_TEXTURE_2D);

    for (i = 0, len = strlen(string); i < len; i++)
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)string[i]);

    glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void setCameraMatrices()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(0, 1, 1, 0, 0, 1); 
    gluPerspective( 70.0, (GLfloat)(wWidth)/(GLfloat)(wHeight), 0.1f, 50000.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display(void) {

    handleKeys();

    static const double ratio = 0.98f;
    timer = clock() / (CLOCKS_PER_SEC / 1000.0);
    timerDelta = timer - lastFrame;
    lastFrame = timer;
    fps = timerDelta * (1.f-ratio) + fps * ratio;

    setCameraMatrices();

    glColor4f(1, 1, 1, 1);
    glClearColor(0.1f, 0.2f, 0.4f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    cam->setView();

    // render world
    for (int i = 0; i < WORLD_SIZE; i++) {
	if (cam->cubeInFrustum(cells[i].x+CELL_SIZE/2.0f, cells[i].y+CELL_SIZE/2.0f, 0.0f, CELL_SIZE/2.0f)) {
	    cells[i].render();
	}
    }

    // render player
    player->render();

    drawStats();
   
    glutSwapBuffers();
    glutPostRedisplay();
}

void idle(void) {
    glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y) {
    setKeyPressed(key, true);
}

void keyReleased(unsigned char key, int x, int y) {
    setKeyPressed(key, false);
}

void handleKeys() {
    float dist = timerDelta * 0.03f;

    if (isKeyDown('a')) cam->moveX(dist); 
    if (isKeyDown('d')) cam->moveX(-dist); 
    if (isKeyDown('w')) cam->moveY(dist); 
    if (isKeyDown('s')) cam->moveY(-dist); 
    if (isKeyDown('q')) cam->moveZ(-dist); 
    if (isKeyDown('z')) cam->moveZ(dist); 

    if (isKeyDown('y')) cam->rotateZ(dist);
    if (isKeyDown('h')) cam->rotateZ(-dist);
    if (isKeyDown('g')) cam->rotateX(-dist);
    if (isKeyDown('j')) cam->rotateX(dist);
    if (isKeyDown('t')) cam->rotateY(-dist);
    if (isKeyDown('b')) cam->rotateY(dist);
}

void specialKeyPressed(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) movePlayer(-1, 0);
    if (key == GLUT_KEY_RIGHT) movePlayer(1, 0);
    if (key == GLUT_KEY_UP) movePlayer(0, 1);
    if (key == GLUT_KEY_DOWN) movePlayer(0, -1); 
}

void specialKeyReleased(int key, int x, int y) {
}

void click(int button, int updown, int x, int y)  {
    button; updown; x; y;
}

void motion (int x, int y)  {
    x; y;
    glutPostRedisplay();
}

void reshape(int x, int y) {
    wWidth = x; 
    wHeight = y;

    glViewport(0, 0, x, y);
    setCameraMatrices(); 
    glutPostRedisplay();
}

void cleanup(void) {
    // TODO
}


void buildWorld() {
    for (int i = 0; i < WORLD_SIZE; i++) {
	Cell c = Cell(2);
	int x = i % WORLD_WIDTH;
	int y = i / WORLD_WIDTH;
	c.move((float)x*CELL_SIZE, (float)y*CELL_SIZE);
	c.load("worldmap.png", 0);
	c.load("worldmap2.png", 1);
	cells.push_back(c);
    }
}

void loadTextures() {
    textures = new TextureManager();
    textures->addTexture("rock", "rock.jpg");
    textures->addTexture("wood", "wood.jpg");
    textures->addTexture("grass", "grass.jpg");
    textures->addTexture("water", "water.gif");
}


bool playerCanMoveTo(int p) {
    if (p == BLOCK_ROCK || p == BLOCK_WATER) 
	return false;
    return true;
}

void movePlayer(int x, int y) {
    int newX = player->x + x;
    int newY = player->y + y;

    // clip
    if (newX < 0 || newY < 0 || newX >= CELL_SIZE*WORLD_WIDTH || newY >= CELL_SIZE*WORLD_WIDTH) 
	return;

    int cellId = newY/CELL_SIZE*WORLD_WIDTH + newX/CELL_SIZE;
    Cell c = cells.at(cellId);
    if (playerCanMoveTo(c.getBlockAt(newX % CELL_SIZE, newY % CELL_SIZE, player->layer)))
	player->setPosition(newX, newY);
}

// TODO fix memleaks :-)
int main(int argc, char** argv) {

    srand((unsigned)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(wWidth, wHeight);
    int win = glutCreateWindow("tilemap proto");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutSpecialFunc(specialKeyPressed);
    glutSpecialUpFunc(specialKeyReleased);
    glutMouseFunc(click);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    //glutIdleFunc(idle);

    atexit(cleanup);

    //cout << "Running OpenGL version: " << glGetString(GL_VERSION) << endl;
    //cout << "Extensions:" << endl << glGetString(GL_EXTENSIONS) << endl;

    // textures
    glShadeModel(GL_FLAT);
    glEnable(GL_TEXTURE_2D);
    loadTextures();

    buildWorld();

    // enable culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    cam = new Camera();
    cam->pos.x = CELL_SIZE / 2.0f; //3.5f;
    cam->pos.y = CELL_SIZE / 2.0f; //3.5f;
    cam->pos.z = 10.0f;

    light = new Light();
    player = new Player();

    initKeyboard();

    glutMainLoop();

    deinitKeyboard();
    delete cam;
    delete light;
    delete player;

    return 0;
}
