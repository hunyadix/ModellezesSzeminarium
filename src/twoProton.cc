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

using glm::vec3;

////////////////////////
// CALLBACK functions //
////////////////////////

void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void timerConsoleCB(int millisec);
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
const int   SCREEN_WIDTH            = 400;
const int   SCREEN_HEIGHT           = 300;
const int   TEXT_WIDTH              = 8;
const int   TEXT_HEIGHT             = 13;
const float FOV_Y                   = 60.0f;
const int   CUBE_ROWS               = 3;
const int   CUBE_COLS               = 4;
const int   CUBE_SLICES             = 3;
const float CAMERA_DISTANCE         = 17.0f;
const float FRAME_RATE              = 50.0f;
const float UPDATE_INTERVAL         = 1000.0f / FRAME_RATE;
const float CONSOLE_UPDATE_INTERVAL = 2000.0f;

namespace Globals
{
	void                                          *font = GLUT_BITMAP_8_BY_13;
	int                                           screenWidth;
	int                                           screenHeight;
	bool                                          mouseLeftDown;
	bool                                          mouseRightDown;
	float                                         mouseX;
	float                                         mouseY;
	float                                         cameraDistance;
	float                                         cameraAngleX;
	float                                         cameraAngleY;
	int                                           drawMode;
	bool initGlobals()
	{
		screenWidth = SCREEN_WIDTH;
		screenHeight = SCREEN_HEIGHT;
		cameraAngleX = cameraAngleY = 0.0f;
		cameraDistance = CAMERA_DISTANCE;
		mouseLeftDown = mouseRightDown = false;
		mouseX = mouseY = 0;
		drawMode = 0;
		return true;
	}
}

int main(int argc, char **argv)
{
	Globals::initGlobals();
	atexit(exitCB);
	initGLUT(argc, argv);
	initGL();
	glutMainLoop();
	return 0;
}

int initGLUT(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(Globals::screenWidth, Globals::screenHeight);
	glutInitWindowPosition(100, 100);
	int handle = glutCreateWindow(argv[0]);
	glutDisplayFunc(displayCB);
	glutTimerFunc(UPDATE_INTERVAL, timerCB, UPDATE_INTERVAL);
	glutTimerFunc(CONSOLE_UPDATE_INTERVAL, timerConsoleCB, CONSOLE_UPDATE_INTERVAL);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);
	glutPassiveMotionFunc(mousePassiveMotionCB);
	return handle;
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

// display info messages
void showInfo()
{
	// backup current model-view matrix
	glPushMatrix();                                                 // save current modelview matrix
	glLoadIdentity();                                               // reset modelview matrix
	// set to 2D orthogonal projection
	glMatrixMode(GL_PROJECTION);                                    // switch to projection matrix
	glPushMatrix();                                                 // save current projection matrix
	glLoadIdentity();                                               // reset projection matrix
	gluOrtho2D(0, Globals::screenWidth, 0, Globals::screenHeight);  // set to orthogonal projection
	float color[4] = {1, 1, 0, 1};
	// for print infos
	std::stringstream ss;
	ss << "Mouse: (" << Globals::mouseX << ", " << Globals::mouseY << ")";
	drawString(ss.str().c_str(),  2, Globals::screenHeight-TEXT_HEIGHT, color, Globals::font);
	ss.str("");
	// ss << "Hit Cube ID: " << "...";
	drawString(ss.str().c_str(), 2, Globals::screenHeight-(TEXT_HEIGHT*2), color, Globals::font);
	ss.str("");
	ss << "Click and drag to pan.";
	drawString(ss.str().c_str(), 2, 2, color, Globals::font);
	ss.str("");
	// unset floating format
	ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	// restore projection matrix
	glPopMatrix();                   // restore to previous projection matrix
	// restore modelview matrix
	glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
	glPopMatrix();                   // restore to previous modelview matrix
}

// set projection matrix as orthogonal
void toOrtho()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)Globals::screenWidth, (GLsizei)Globals::screenHeight);
	// set orthographic viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Globals::screenWidth, 0, Globals::screenHeight, -1, 1);
	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// set the projection matrix as perspective
void toPerspective()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)Globals::screenWidth, (GLsizei)Globals::screenHeight);
	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV_Y, (float)(Globals::screenWidth)/Globals::screenHeight, 1.0f, 10000.0f); // FOV, AspectRatio, NearClip, FarClip
	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void initParticles()
{
	// Movement on plane z = 0
	new Electron(vec3(-1, 0, 0), vec3(0, 5, 0));
	new Proton(vec3( 1,  2, 0));
	new Proton(vec3( 1, -2, 0));
}

void calculateForces()
{
	// Electrostatically interacting particles
	for(auto& chargedParticle: ChargedParticle::chargedParticleCollection)
	{
		chargedParticle -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
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

///////////////
// CALLBACKS //
///////////////

void displayCB()
{
	// clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();
	// tramsform camera
	glTranslatef(0, 0, -Globals::cameraDistance);
	glRotatef(Globals::cameraAngleX, 1, 0, 0);   // pitch
	glRotatef(Globals::cameraAngleY, 0, 1, 0);   // heading
	for(const auto& particle: Particle::particleCollection)
	{
		particle -> display();
	}
	showInfo();
	glPopMatrix();
	glutSwapBuffers();
}

void reshapeCB(int width, int height)
{
	Globals::screenWidth  = width;
	Globals::screenHeight = height;
	toPerspective();
}

void timerCB(int millisec)
{
	glutTimerFunc(millisec, timerCB, millisec);
	calculateForces();
	updateParticles(millisec * 1e-3);
	// Reset electron position
	auto& electron_1 = *(ChargedParticle::chargedParticleCollection[0]);
	if(glm::length(electron_1.getPosition()) > 10)
	{
		electron_1.setPosition(vec3(-10, rand() / static_cast<double>(RAND_MAX) * 12 - 6, 0));
		electron_1.setVelocity(vec3( 5, rand() / static_cast<double>(RAND_MAX) * 8 - 4, rand() / static_cast<double>(RAND_MAX) * 8 - 4));
	}
	// Reset proton position
	auto& proton_1 = *(ChargedParticle::chargedParticleCollection[1]);
	auto& proton_2 = *(ChargedParticle::chargedParticleCollection[2]);
	proton_1.setPosition(vec3( 1,  2, 0));
	proton_2.setPosition(vec3( 1, -2, 0));
	glutPostRedisplay();
}

void timerConsoleCB(int millisec)
{
	glutTimerFunc(millisec, timerConsoleCB, millisec);
	for(const auto& particle: ChargedParticle::chargedParticleCollection)
	{
		if(particle -> getCharge() == Electron::electronCharge)
		{
			std::cout << "Kinetic energy: " << particle -> getKineticEnergy() << std::endl;
		}
	}
}


void idleCB()
{
	glutPostRedisplay();
}

void keyboardCB(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27: // ESCAPE
			exit(0);
			break;
		// switch rendering modes (fill -> wire -> point)
		case 'd': 
		case 'D':
			Globals::drawMode = (Globals::drawMode + 1) % 3;
			if(Globals::drawMode == 0)        // fill mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			}
			else if(Globals::drawMode == 1)  // wireframe mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
			else                             // point mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
			break;
	}
}


void mouseCB(int button, int state, int x, int y)
{
	Globals::mouseX = x;
	Globals::mouseY = y;
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
			Globals::mouseLeftDown = true;
		else if(state == GLUT_UP)
			Globals::mouseLeftDown = false;
	}
	else if(button == GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
			Globals::mouseRightDown = true;
		else if(state == GLUT_UP)
			Globals::mouseRightDown = false;
	}
}

void mouseMotionCB(int x, int y)
{
	if(Globals::mouseLeftDown)
	{
		Globals::cameraAngleY += (x - Globals::mouseX);
		Globals::cameraAngleX += (y - Globals::mouseY);
		Globals::mouseX = x;
		Globals::mouseY = y;
	}
	if(Globals::mouseRightDown)
	{
		Globals::cameraDistance -= (y - Globals::mouseY) * 0.2f;
		Globals::mouseY = y;
	}
}


void mousePassiveMotionCB(int x, int y)
{
	Globals::mouseX = x;
	Globals::mouseY = y;
	// draw cubes with GL_SELECT mode and select a cube
	if(!Globals::mouseLeftDown && !Globals::mouseRightDown)
	{
		// Globals::hitId = selectCube();
	}
}

void exitCB() 
{
	std::cout << "Process exited normally. Terminating..." << std::endl;
}