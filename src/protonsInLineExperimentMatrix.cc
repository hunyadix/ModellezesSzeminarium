// Hunyadi Adam, 2015
// MIT. license
#include <GL/glew.h>
#include <GL/glu.h> 

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <memory>
#include <utility>
#include <glm/glm.hpp>

#include <TROOT.h>
#include <TFile.h>
#include <TH2D.h>
#include <TApplication.h>
#include <TCanvas.h>

#include "../interface/Electron.h"
#include "../interface/Proton.h"

#include "../interface/Pbar.h"

#include "../interface/HelperFunctionsCommon.h"


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

using glm::vec3;


// This program solves equations of motions for an electrons bombarding protons ordered in a single line.
// Constants are defined in the following lines

// Constants

// const float COULOMB_CONSTANT                     = ChargedParticle::coulombConstant; // 8.99e9
const float SPEED_OF_LIGHT                        = 299792458;
const float ELECTRON_CHARGE                       = Electron::electronCharge;         // -1.602176e-19f
const float ELECTRON_MASS                         = Electron::electronMass;           // 9.109e-31f
const float PROTON_CHARGE                         = Proton::protonCharge;             // +1.602176e-19f;
const float PROTON_MASS                           = Proton::protonMass;               // 1.672e-27;
const float EV_TO_VELOCITY_SQUARED                = -2.0f * ELECTRON_CHARGE / ELECTRON_MASS;
const float HIDROGEN_BOND_LENGTH                  = 7.4e-11;

const int   NUM_EXPERIMENTS_PER_SETUP             = 10000;
// const float DT_STEP                               = 1.0e-14f;
const float DT_STEP                               = 1.0e-18f;
const int   NUMBER_OF_PROTONS                     = 6;                                // should be even
const float PROTON_PROTON_DISTANCE                = HIDROGEN_BOND_LENGTH;
const float ELECTRON_START_PLANE_DISTANCE         = 5.0f * HIDROGEN_BOND_LENGTH;
const float ELECTRON_END_PLANE_DISTANCE           = 5.0f * HIDROGEN_BOND_LENGTH;
const float ELECTRON_START_KINETIC_ENERGY_EV_MIN  = 1;
const float ELECTRON_START_KINETIC_ENERGY_EV_MAX  = 10;
// const float ELECTRON_START_KINETIC_ENERGY_EV_MAX  = 4.0e-9f;
const float ELECTRON_START_KINETIC_ENERGY_EV_STEP = 1;
const int   ELECTRON_START_KIN_EN_NUM_MEAS_POINTS = std::ceil((ELECTRON_START_KINETIC_ENERGY_EV_MAX - ELECTRON_START_KINETIC_ENERGY_EV_MIN) / ELECTRON_START_KINETIC_ENERGY_EV_STEP) + 1;
// const float ELECTRON_START_SPEED                  = sqrt(ELECTRON_START_KINETIC_ENERGY_EV * EV_TO_VELOCITY_SQUARED);
// const float ELECTRON_START_SPEED_IN_C             = ELECTRON_START_SPEED / SPEED_OF_LIGHT;

// Video options
const int   ENABLE_VIDEO                          = 0;
const int   VIDEO_NUM_TESTS                       = 1;
const int   VIDEO_SCREEN_WIDTH                    = 400;                              // resizeable
const int   VIDEO_SCREEN_HEIGHT                   = 300;                              // resizeable
const float VIDEO_TARGET_FRAME_RATE               = 20.0f;
const float VIDEO_UPDATE_INTERVAL_MS              = 1000.0f / VIDEO_TARGET_FRAME_RATE;
const float VIDEO_CAMERA_ANGLEX                   = 0.0f;
const float VIDEO_CAMERA_ANGLEY                   = 0.0f;
const float VIDEO_CAMERA_DISTANCE                 = 20.0f;
const float VIDEO_CAMERA_FIELD_OF_VIEW            = 60.0f;
const float VIDEO_CAMERA_NEAR_CLIP                = 1.0f;
const float VIDEO_CAMERA_FAR_CLIP                 = 10000.0f;
const int   VIDEO_DRAWMODE                        = 0;

// Test for 6 protons; positions in p-p distances: -2.5, -1.5, -0.5, 0.5, 1.5, 2.5
constexpr float protonPosition(int index) { return (-0.5f * NUMBER_OF_PROTONS + index + 0.5f) * PROTON_PROTON_DISTANCE; } 
const std::vector<float> PROTONPOSITIONS(NUMBER_OF_PROTONS, 0);
constexpr float electronStartVelocity(int index) { return sqrt((ELECTRON_START_KINETIC_ENERGY_EV_MIN + (index * ELECTRON_START_KINETIC_ENERGY_EV_STEP)) * EV_TO_VELOCITY_SQUARED); } 
const std::vector<float> ELECTRON_START_VELOCITIES(ELECTRON_START_KIN_EN_NUM_MEAS_POINTS, 0);
constexpr float electronStartKineticEnergy(int index) { return ELECTRON_START_KINETIC_ENERGY_EV_MIN + index * ELECTRON_START_KINETIC_ENERGY_EV_STEP; } 
const std::vector<float> ELECTRON_START_KIN_ENERGIES(ELECTRON_START_KIN_EN_NUM_MEAS_POINTS, 0);

void initConsts()
{
	for(auto index: range(PROTONPOSITIONS.size()))
	{
		float& unconst_element = const_cast<float&>(PROTONPOSITIONS[index]);
		unconst_element = protonPosition(index);
	}
	for(auto index: range(ELECTRON_START_VELOCITIES.size()))
	{
		float& unconst_element = const_cast<float&>(ELECTRON_START_VELOCITIES[index]);
		unconst_element = electronStartVelocity(index);
	}
	for(auto index: range(ELECTRON_START_KIN_ENERGIES.size()))
	{
		float& unconst_element = const_cast<float&>(ELECTRON_START_KIN_ENERGIES[index]);
		unconst_element = electronStartKineticEnergy(index);
	}
}

void printPhysicsInfo()
{
	std::cout << "---------------------------\n";
	std::cout << "Experiment setup data:\n";
	std::cout << "Num. experiments per measurement points:     " << NUM_EXPERIMENTS_PER_SETUP << "\n";
	std::cout << "DT step:                                     " << DT_STEP                   << "\n";
	std::cout << "Number of fixed protons:                     " << NUMBER_OF_PROTONS         << "\n";
	std::cout << "Proton arrangement:                          " << "SINGLE_LINE"             << "\n";
	std::cout << "Proton-proton distance:                      " << PROTON_PROTON_DISTANCE    << "\n";
	std::cout << "Proton-proton distance in H bond lengths:    " << std::fixed << std::setprecision(2) << PROTON_PROTON_DISTANCE / HIDROGEN_BOND_LENGTH << "\n";
	std::cout << "Number of measurement points:                " << ELECTRON_START_KIN_EN_NUM_MEAS_POINTS << "\n";
	std::cout << "Measurement point list:\n";
	int i = 0;
	while(1)
	{
		float energy   = ELECTRON_START_KINETIC_ENERGY_EV_MIN + i * ELECTRON_START_KINETIC_ENERGY_EV_STEP;
		if(ELECTRON_START_KINETIC_ENERGY_EV_MAX < energy) break;
		float speed    = ELECTRON_START_VELOCITIES[i];
		float speedInC = speed / SPEED_OF_LIGHT;
		std::cout << "Starting e- kinetic energy, speed: " << std::resetiosflags(std::ios::fixed) << std::setw(8) << energy << " eV | " << std::setprecision(5) << std::setw(8) << speed << " m/s | " << std::fixed << std::setprecision(2) << speedInC * 100.0f << "% of * c\n";
		++i;
	}
	std::cout << "---------------------------\n";
	std::cout << std::flush;
}

// Function declarations
void        physicsMain(int);
void        setProtonPositions();
void        setElectronStartPositionVelocity(const int& numSetup);
void        initExperiment(const int& numSetup);
// void        calculateForces();
// void        updateParticles(const float& dt);
const vec3& runExperiment();
const vec3& runExperiment(int & numUpdates);
void        clearExperiment();

// Video
void initGL();
int  initGLUT(int argc, char **argv);
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void toOrtho();
void toPerspective();
void idleCB();
void displayCB();

TApplication* theApp = nullptr;

int main(int argc, char **argv)
{
	std::cout << argv[0] << " started..." << std::endl;
	initConsts();
	theApp = new TApplication("App", &argc, argv);
	if(ENABLE_VIDEO)
	{
		initGL();
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
		glutInitWindowSize(VIDEO_SCREEN_WIDTH, VIDEO_SCREEN_HEIGHT);
		glutInitWindowPosition(100, 100);
		glutCreateWindow(argv[0]);
		glutIdleFunc(idleCB);
		glutDisplayFunc(displayCB);
		toPerspective();
		glutTimerFunc(0.0, physicsMain, 0);
		glutMainLoop();
	}
	else
	{
		physicsMain(0);
	}
	return 0;
}

void physicsMain(int)
{
	std::cout << "\n";
	printPhysicsInfo();
	std::cout << "\n";
	// TH1D electronPositionsX ("electronPositionsX",  "Electron positions X",  200, -PROTON_PROTON_DISTANCE, PROTON_PROTON_DISTANCE);
	// TH2D electronPositionsXY("electronPositionsXY", "Electron positions XY", 200, -PROTON_PROTON_DISTANCE, PROTON_PROTON_DISTANCE, 200, -PROTON_PROTON_DISTANCE, PROTON_PROTON_DISTANCE);
	TH2D electronEnergyEndPositionsX_H ("electronEnergyEndPositionsX",  "Electron end position distribution vs starting kin. energy;x pos(m);starting kin. energy (eV)",  
		200,                                   -PROTON_PROTON_DISTANCE,               PROTON_PROTON_DISTANCE, 
		ELECTRON_START_KIN_EN_NUM_MEAS_POINTS,  ELECTRON_START_KINETIC_ENERGY_EV_MIN - 0.5 * ELECTRON_START_KINETIC_ENERGY_EV_STEP, ELECTRON_START_KINETIC_ENERGY_EV_MAX + 0.5 * ELECTRON_START_KINETIC_ENERGY_EV_STEP);
	std::vector<long long> totalNumUpdates;
	static_assert(64 <= 8 * sizeof(long long), "WARNING: Progress sizes might not be calculated properly, since your architecture implements long long with size less than 64 bits.");
	Pbar totalProgress;
	auto start = time(NULL);
	for(int measurementPointIndex = 0; measurementPointIndex < ELECTRON_START_KIN_EN_NUM_MEAS_POINTS; ++measurementPointIndex)
	{
		totalNumUpdates.push_back(0);
		const auto& startKineticEnergyFillPos = ELECTRON_START_KIN_ENERGIES[measurementPointIndex];
		Pbar experimentProgress;
		for(int experimentNumber = 0; experimentNumber < NUM_EXPERIMENTS_PER_SETUP; ++experimentNumber)
		{
			int numUpdates = 0;
			if(experimentNumber % (NUM_EXPERIMENTS_PER_SETUP / 100) == 0)
			{
				totalProgress.update(1.0f / ELECTRON_START_KIN_EN_NUM_MEAS_POINTS);
				experimentProgress.update(1);
				totalProgress.print();
				experimentProgress.printNoUpdate();
			// 	std::cout << experimentNumber * 100.0f / NUM_EXPERIMENTS << '%' << std::endl;
			}
			initExperiment(measurementPointIndex);
			vec3 electronHitPosition = runExperiment(numUpdates);
			clearExperiment();
			electronEnergyEndPositionsX_H.Fill(electronHitPosition.x, startKineticEnergyFillPos);
			totalNumUpdates.back() += numUpdates;
		}
	}
	std::cout << "\n\n";
	auto end = time(NULL);
	std::cout << "Took about: " << end - start << " second(s)." << std::endl;
	std::cout << "Average number of updates per experiment: \n";
	for(auto i: range(totalNumUpdates.size()))
		std::cout << "\t" << totalNumUpdates[i] / static_cast<double>(NUM_EXPERIMENTS_PER_SETUP) << std::endl;
	TCanvas canvas;
	canvas.cd();
	electronEnergyEndPositionsX_H.Draw("COLZ");
	canvas.Print("matrix.eps");
	theApp -> Run();
}

void setProtonPositions()
{
	for(int protonIndex = 0; protonIndex < NUMBER_OF_PROTONS; ++protonIndex)
	{
		const auto& proton = ChargedParticle::chargedParticleCollection[protonIndex];
		proton -> setPosition(vec3(PROTONPOSITIONS[protonIndex], 0, 0));
	}
}

// Electron x position: between two protons in the middle
void setElectronStartPositionVelocity(const int& numSetup)
{
	auto& electron = ChargedParticle::chargedParticleCollection[NUMBER_OF_PROTONS];
	float x, y, z;
	float vx, vy, vz;
	// Example for 6 protons:
	// -2.5  -1.5  -0.5   0.5   1.5   2.5
	//   p     p     p*****p     p     p
	// Calc: -0.5 + uniform_rand(0, 1)
	x = rand() / static_cast<double>(RAND_MAX) * PROTON_PROTON_DISTANCE - 0.5f * PROTON_PROTON_DISTANCE;
	y = ELECTRON_START_PLANE_DISTANCE;
	z = 0;
	vx = 0;
	vy = -ELECTRON_START_VELOCITIES[numSetup];
	vz = 0;
	electron -> setPosition(vec3( x,  y,  z));
	electron -> setVelocity(vec3(vx, vy, vz));
}

// Initialize an experiment setup:
// Two fixed protons and one electron moving towards them 
// with randomized velocity and position
void initExperiment(const int& numSetup)
{
	for(int protonIndex = 0; protonIndex < NUMBER_OF_PROTONS; ++protonIndex)
	{
		new Proton();
	}
	new Electron();
	setProtonPositions();
	setElectronStartPositionVelocity(numSetup);
}

// Contains calculations for the scattering processes
const vec3& runExperiment()
{
	while(1)
	{
		auto& electron = ChargedParticle::chargedParticleCollection[NUMBER_OF_PROTONS];
		electron -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
		electron -> update(DT_STEP);
		const vec3& electronPosition = electron -> getPosition();
		// std::cout << "Electron y: " << electronPosition.y() << std::endl;
		if(electronPosition.y < -ELECTRON_END_PLANE_DISTANCE)
		{
			return electronPosition;
		}
	}
}
const vec3& runExperiment(int& numUpdates)
{
	static int saved = 0;
	numUpdates = 0;
	while(1)
	{
		numUpdates++;
		auto& electron = ChargedParticle::chargedParticleCollection[NUMBER_OF_PROTONS];
		electron -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
		electron -> update(DT_STEP);
		const vec3& electronPosition = electron -> getPosition();
		if(ENABLE_VIDEO && saved < 20 && numUpdates % 300 == 0)
		{
			saved++;
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			// save the initial ModelView matrix before modifying ModelView matrix
			glPushMatrix();
			// tramsform camera
			glTranslatef(0, 0, -VIDEO_CAMERA_DISTANCE);
			glRotatef(VIDEO_CAMERA_ANGLEX, 1, 0, 0);   // pitch
			glRotatef(VIDEO_CAMERA_ANGLEY, 0, 1, 0);   // heading
			for(const auto& particle: ChargedParticle::chargedParticleCollection)
			{
				particle -> display();
			}
			glPopMatrix();
			// glFlush();
			displayCB();
			GLubyte *pixels = NULL;
			std::string filename = "tmp" + std::to_string(saved) + ".ppm";
			screenshot_ppm(filename.c_str(), VIDEO_SCREEN_WIDTH, VIDEO_SCREEN_HEIGHT, &pixels);
			free(pixels);
			std::cout << "Saved." << std::endl;
		}
		// std::cout << "Electron y: " << electronPosition.y << std::endl;
		if(electronPosition.y < -ELECTRON_END_PLANE_DISTANCE)
		{
			return electronPosition;
		}
	}
}

// Deletes particles in the experiment
void clearExperiment()
{
	Particle::particleCollection.clear();
	ChargedParticle::chargedParticleCollection.clear();
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

// set projection matrix as orthogonal
void toOrtho()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)VIDEO_SCREEN_HEIGHT, (GLsizei)VIDEO_SCREEN_HEIGHT);
	// set orthographic viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, VIDEO_SCREEN_WIDTH, 0, VIDEO_SCREEN_HEIGHT, -1, 1);
	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// set the projection matrix as perspective
void toPerspective()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)VIDEO_SCREEN_WIDTH, (GLsizei)VIDEO_SCREEN_HEIGHT);
	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(VIDEO_CAMERA_FIELD_OF_VIEW, (float)(VIDEO_SCREEN_WIDTH) / VIDEO_SCREEN_HEIGHT, VIDEO_CAMERA_NEAR_CLIP, VIDEO_CAMERA_FAR_CLIP); // FOV, AspectRatio, NearClip, FarClip
	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightKd);
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

void idleCB()
{
	glutPostRedisplay();
}

void displayCB()
{
	// clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();
	// tramsform camera
	glTranslatef(0, 0, -VIDEO_CAMERA_DISTANCE);
	glRotatef(VIDEO_CAMERA_ANGLEX, 1, 0, 0);   // pitch
	glRotatef(VIDEO_CAMERA_ANGLEY, 0, 1, 0);   // heading
	for(const auto& particle: Particle::particleCollection)
	{
		particle -> display();
	}
	glPopMatrix();
	glutSwapBuffers();
}