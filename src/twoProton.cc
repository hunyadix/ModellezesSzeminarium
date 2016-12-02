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

#include "../interface/HelperFunctionsCommon.h"

using glm::vec3;

// Constants
const float DT_STEP = 1.0e-14f;
const float HIDROGEN_PROTON_DISTANCE = 7.4e-11;

// Function declarations
void setProtonPositions();
void setElectronStartPositionVelocity();
void        initExperiment();
// void        calculateForces();
// void        updateParticles(const float& dt);
const vec3& runExperiment();
void        clearExperiment();

int main(int argc, char **argv)
{
	TApplication* theApp = new TApplication("App", &argc, argv);
	TH1D electronPositionsX ("electronPositionsX",  "Electron positions X",  200, -HIDROGEN_PROTON_DISTANCE, HIDROGEN_PROTON_DISTANCE);
	TH2D electronPositionsXY("electronPositionsXY", "Electron positions XY", 200, -HIDROGEN_PROTON_DISTANCE, HIDROGEN_PROTON_DISTANCE, 200, -HIDROGEN_PROTON_DISTANCE, HIDROGEN_PROTON_DISTANCE);
	for(int experimentNumber = 0; experimentNumber < 1000000; ++experimentNumber)
	{
		initExperiment();
		vec3 electronHitPosition = runExperiment();
		clearExperiment();
		electronPositionsX.Fill(electronHitPosition.x);
	}
	TCanvas canvas;
	canvas.cd();
	electronPositionsX.Draw();
	theApp -> Run();
	return 0;
}

void setProtonPositions()
{
	auto& proton_1 = ChargedParticle::chargedParticleCollection[0];
	auto& proton_2 = ChargedParticle::chargedParticleCollection[1];
	proton_1 -> setPosition(vec3(-HIDROGEN_PROTON_DISTANCE, 0, 0));
	proton_2 -> setPosition(vec3(+HIDROGEN_PROTON_DISTANCE, 0, 0));
}

void setElectronStartPositionVelocity()
{
	auto& electron = ChargedParticle::chargedParticleCollection[2];
	float x, y, z;
	float vx, vy, vz;
	x = rand() / static_cast<double>(RAND_MAX) * HIDROGEN_PROTON_DISTANCE - 0.5f * HIDROGEN_PROTON_DISTANCE;
	y = HIDROGEN_PROTON_DISTANCE * 2;
	z = 0;
	vx = 0;
	vy = 0;
	vz = 0;
	electron -> setPosition(vec3( x,  y,  z));
	electron -> setVelocity(vec3(vx, vy, vz));
}

// Initialize an experiment setup:
// Two fixed protons and one electron moving towards them 
// with randomized velocity and position
void initExperiment()
{
	new Proton();
	new Proton();
	new Electron();
	setProtonPositions();
	setElectronStartPositionVelocity();
}

// Calculate forces before uptdating the particles
// void calculateForces()
// {
// 	// Electrostatically interacting particles
// 	for(auto& chargedParticle: ChargedParticle::chargedParticleCollection)
// 	{
// 		chargedParticle -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
// 	}
// }

// Update particle positions, velocities
// void updateParticles(const float& dt)
// {
// 	// Electrostatically interacting particles
// 	for(const auto& particle: ChargedParticle::chargedParticleCollection)
// 	{
// 		particle -> update(dt);
// 	}
// }

// Contains calculations for the scattering processes
const vec3& runExperiment()
{
	// int i = 0;
	while(1)
	{
		auto& electron = ChargedParticle::chargedParticleCollection[2];
		// calculateForces();
		// updateParticles(DT_STEP);
		electron -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
		electron -> update(DT_STEP);
		const vec3& electronPosition = electron -> getPosition();
		// std::cout << "Electron y: " << electronPosition.y() << std::endl;
		if(electronPosition.y < -HIDROGEN_PROTON_DISTANCE)
		{
			// std::cout << i << std::endl;
			return electronPosition;
		}
		// ++i;
	}
}

// Deletes particles in the experiment
void clearExperiment()
{
	Particle::particleCollection.clear();
	ChargedParticle::chargedParticleCollection.clear();
}
