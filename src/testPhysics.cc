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

namespace Globals
{
	void              *font = GLUT_BITMAP_8_BY_13;
	int               screenWidth;
	int               screenHeight;
	bool              mouseLeftDown;
	bool              mouseRightDown;
	float             mouseX;
	float             mouseY;
	float             cameraDistance;
	float             cameraAngleX;
	float             cameraAngleY;
	int               drawMode;
	int               hitId;
	bool initGlobals()
	{
		screenWidth = SCREEN_WIDTH;
		screenHeight = SCREEN_HEIGHT;
		cameraAngleX = cameraAngleY = 0.0f;
		cameraDistance = CAMERA_DISTANCE;
		mouseLeftDown = mouseRightDown = false;
		mouseX = mouseY = 0;
		drawMode = 0;
		hitId = -1;
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
	glutTimerFunc(33, timerCB, 33);
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
	drawString(ss.str().c_str(), 2, Globals::screenHeight-TEXT_HEIGHT, color, Globals::font);
	ss.str("");
	ss << "Hit Cube ID: " << Globals::hitId;
	drawString(ss.str().c_str(), 2, Globals::screenHeight-(TEXT_HEIGHT*2), color, Globals::font);
	ss.str("");
	ss << "Move the mouse over a cube.";
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

// // draw cubes with GL_SELECT mode, and return the ID of the hit cube
// int selectCube()
// {
// 	const int BUFFER_SIZE = 512;
// 	GLuint selectionBuffer[BUFFER_SIZE];
// 	memset(selectionBuffer, 0, sizeof(int) * BUFFER_SIZE);
// 	int viewport[4] = {0, 0, Globals::screenWidth, Globals::screenHeight};
// 	// tell OpenGL to use selection buffer
// 	glSelectBuffer(BUFFER_SIZE, selectionBuffer);
// 	// enter selection mode
// 	glRenderMode(GL_SELECT);
// 	// init name stack
// 	glInitNames();
// 	// set matrix
// 	glMatrixMode(GL_PROJECTION);
// 	glPushMatrix();
// 	glLoadIdentity();
// 	gluPickMatrix(Globals::mouseX, Globals::screenHeight - Globals::mouseY, 3, 3, viewport);
// 	gluPerspective(FOV_Y,  (float)Globals::screenWidth / Globals::screenHeight, 0.1f, 1000.0f);
// 	// view transform
// 	glMatrixMode(GL_MODELVIEW);
// 	glPushMatrix();
// 	glTranslatef(0, 0, -Globals::cameraDistance);
// 	glRotatef(Globals::cameraAngleX, 1, 0, 0);   // pitch
// 	glRotatef(Globals::cameraAngleY, 0, 1, 0);   // heading
// 	glDisable(GL_BLEND);
// 	glDisable(GL_LIGHTING);
// 	glShadeModel(GL_FLAT);
// 	// drawSphere();
// 	glEnable(GL_BLEND);
// 	glEnable(GL_LIGHTING);
// 	glShadeModel(GL_SMOOTH);
// 	glMatrixMode(GL_PROJECTION);
// 	glPopMatrix();
// 	glMatrixMode(GL_MODELVIEW);
// 	glPopMatrix();
// 	glFlush();
// 	// leave selection mode
// 	int hitCount = glRenderMode(GL_RENDER);
// 	// process hits
// 	if(hitCount > 0)
// 	{
// 		return processHits(hitCount, selectionBuffer);
// 	}
// 	else
// 	{
// 		return -1; // not hit
// 	}
// }

// int processHits(int hitCount, GLuint* buffer)
// {
// 	if(hitCount <= 0 || !buffer)
// 		return -1;
// 	GLuint nameCount, minZ, maxZ;
// 	GLuint hitZ = 0xffffffff;   // init with biggest value
// 	int hitId = 0;
// 	for(int i = 0; i < hitCount; ++i)
// 	{
// 		nameCount = *buffer++;
// 		minZ = *buffer++;
// 		maxZ = *buffer++;
// 		if(minZ < hitZ)
// 		{
// 			hitZ = minZ;
// 			hitId = (int)*buffer;
// 		}
// 		// consume other entries in the buffer
// 		for(int j = 0; j < (int)nameCount; ++j, ++buffer)
// 		{
// 			//cout << "Name: " << *buffer << endl;
// 			;
// 		}
// 	}
// 	//DEBUG
// 	//std::cout << "HIT ID: " << hitId << std::endl;
// 	return hitId;
// }

void drawSphere()
{
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glutSolidSphere(1.2, 20, 50);
	glPopMatrix();
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
	std::vector<std::shared_ptr<Drawable>> objectsToDraw;
	std::shared_ptr<Drawable> electron_1 = std::make_shared<Electron>(vec3(-1, 0, 0));
	std::shared_ptr<Drawable> electron_2 = std::make_shared<Proton>(  vec3( 1, 0, 0));
	objectsToDraw.push_back(electron_1);
	objectsToDraw.push_back(electron_2);
	for(const auto& object: objectsToDraw)
	{
		object -> display();
	}
	showInfo();
	glPopMatrix();
	glutSwapBuffers();
}

void reshapeCB(int width, int height)
{
	Globals::screenWidth = width;
	Globals::screenHeight = height;
	toPerspective();
}

void timerCB(int millisec)
{
	glutTimerFunc(millisec, timerCB, millisec);
	glutPostRedisplay();
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