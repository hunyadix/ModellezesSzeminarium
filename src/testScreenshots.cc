#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "../interface/Electron.h"
#include "../interface/Proton.h"

#include <SDL2/SDL.h>

using glm::vec3;

static void screenshot_ppm(const char *filename, unsigned int width, unsigned int height, GLubyte **pixels)
{
    size_t i, j, cur;
    const size_t format_nchannels = 3;
    FILE *f = fopen(filename, "w");
    fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
    *pixels = (GLubyte*)realloc(*pixels, format_nchannels * sizeof(GLubyte) * width * height);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++) 
        {
            cur = format_nchannels * ((height - i - 1) * width + j);
            fprintf(f, "%3d %3d %3d ", (*pixels)[cur], (*pixels)[cur + 1], (*pixels)[cur + 2]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

////////////////////////
// CALLBACK functions //
////////////////////////

void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void idleCB();
void keyboardCB(unsigned char key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);
void mousePassiveMotionCB(int x, int y);
void exitCB();

void initGL();
int  initGLUT(int argc, char **argv);
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void showInfo();
void toOrtho();
void toPerspective();

void initParticles();
void calculateForces();
void updateParticles(const float& dt);

// constants
const int   SCREEN_WIDTH    = 400;
const int   SCREEN_HEIGHT   = 300;
const int   TEXT_WIDTH      = 8;
const int   TEXT_HEIGHT     = 13;
const float FOV_Y           = 60.0f;
const int   CUBE_ROWS       = 3;
const int   CUBE_COLS       = 4;
const int   CUBE_SLICES     = 3;
const float CAMERA_DISTANCE = 17.0f;
const float FRAME_RATE      = 50.0f;
const float UPDATE_INTERVAL = 1000.0f / FRAME_RATE;

void displayCB()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0, 0, -CAMERA_DISTANCE);
	glRotatef(0.0f, 1, 0, 0);   // pitch
	glRotatef(0.0f, 0, 1, 0);   // heading
	for(const auto& particle: Particle::particleCollection)
	{
		particle -> display();
	}
	glPopMatrix();
	glutSwapBuffers();
}

void physicsMain(int)
{
	int numIteration = 0;
	while(1)
	{
		calculateForces();
		updateParticles(1000 * 1e-5);
		numIteration++;
		if(numIteration % 10000 == 0)
		{
			displayCB();
			std::cout << numIteration / 10000 << std::endl;
		}
	}
}

int main(int argc, char **argv)
{
	initGL();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(displayCB);
	toPerspective();
	glutTimerFunc(0.0, physicsMain, 0);
	glutMainLoop();
	return 0;
}

void initGL()
{
	glShadeModel(GL_SMOOTH);               // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // 4-byte pixel alignment
	// enable/disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	// glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	// glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0, 0, 0, 0); // background color
	glClearStencil(0);        // clear stencil buffer
	glClearDepth(1.0f);       // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);
	initLights();
	initParticles();
	float white[] = {1, 1, 1, 1};
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
}

// The projection matrix must be set to orthogonal before call this function.
void drawString(const char *str, int x, int y, float color[4], void *font)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);                         // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);
	glDepthFunc(GL_ALWAYS);
	glColor4fv(color);                              // set text color
	glRasterPos2i(x, y);                            // place text position
	while(*str)
	{
		glutBitmapCharacter(font, *str);
		++str;
	}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
	glPopAttrib();
}

// draw a string in 3D space
void drawString3D(const char *str, float pos[3], float color[4], void *font)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);                         // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);
	glColor4fv(color);                              // set text color
	glRasterPos3fv(pos);                            // place text position
	while(*str)
	{
		glutBitmapCharacter(font, *str);
		++str;
	}
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

void initLights()
{
	GLfloat lightKa[] = {0.0f, 0.0f, 0.0f, 1.0f}; // ambient light
	GLfloat lightKd[] = {1.0f, 1.0f, 1.0f, 1.0f}; // diffuse light
	GLfloat lightKs[] = {1, 1, 1, 1};             // specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
	float lightPos[4] = {0, 1, 1, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}

void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}

// set projection matrix as orthogonal
void toOrtho()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);
	// set orthographic viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// set the projection matrix as perspective
void toPerspective()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);
	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV_Y, (float)(SCREEN_WIDTH)/SCREEN_HEIGHT, 1.0f, 10000.0f); // FOV, AspectRatio, NearClip, FarClip
	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void initParticles()
{
	new Electron(vec3(-1, 0, 0), vec3(0, 5, 0));
	// Movement on plane z = 0
	new Proton(vec3( 1,  2, 0));
	new Proton(vec3( 1, -2, 0));
}

void calculateForces()
{
	// Electrostatically interacting particles
	for(auto firstParticle = ChargedParticle::chargedParticleCollection.begin(); firstParticle != ChargedParticle::chargedParticleCollection.end(); ++firstParticle)
	{
		vec3 potential(0, 0, 0);
		for(auto secondParticle = ChargedParticle::chargedParticleCollection.begin(); secondParticle != ChargedParticle::chargedParticleCollection.end(); ++secondParticle)
		{
			if(firstParticle == secondParticle) continue;
			potential += (*secondParticle) -> getPotentialAt((*firstParticle) -> getPosition());
		}
		(*firstParticle) -> calculateForceFromPotential(potential);
	}
}

void updateParticles(const float& dt)
{
	// Electrostatically interacting particles
	for(const auto& particle: ChargedParticle::chargedParticleCollection)
	{
		particle -> update(dt);
	}
}