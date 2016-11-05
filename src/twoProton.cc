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
const float DT_STEP = 1.0f;

// Function declarations
void        initExperiment();
void        calculateForces();
void        updateParticles(const float& dt);
const vec3& runExperiment();
void        clearExperiment();

int main(int argc, char **argv)
{
	TApplication* theApp = new TApplication("App", &argc, argv);
	TH1D electronPositionsX ("electronPositionsX",  "Electron positions X",  200, -100, 100);
	TH2D electronPositionsXY("electronPositionsXY", "Electron positions XY", 200, -100, 100, 2000, -100, 100);
	for(int experimentNumber: range(100))
	{
		initExperiment();
		vec3 electronHitPosition = runExperiment();
		std::cout << electronHitPosition.x << std::endl;
		clearExperiment();
		electronPositionsX.Fill(electronHitPosition.x);
	}
	TCanvas canvas;
	canvas.cd();
	electronPositionsX.Draw();
	theApp -> Run();
	return 0;
}

// Initialize an experiment setup:
// Two fixed protons and one electron moving towards them 
// with randomized velocity and position
void initExperiment()
{
	new Proton(vec3( 1,  2, 0));
	new Proton(vec3( 1, -2, 0));
	float electronX, electronY, electronZ;
	float electronVX, electronVY, electronVZ;
	electronX = rand() / static_cast<double>(RAND_MAX) * 2.0;
	electronY = rand() / static_cast<double>(RAND_MAX) * 2.0;
	electronZ = rand() / static_cast<double>(RAND_MAX) * 2.0;
	electronVX = electronVY = electronVZ = 0;
	new Electron(vec3(electronX, electronY, electronZ), vec3(electronVX, electronVY, electronVZ));
}

// Calculate forces before uptdating the particles
void calculateForces()
{
	// Electrostatically interacting particles
	for(auto& chargedParticle: ChargedParticle::chargedParticleCollection)
	{
		chargedParticle -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
	}
}

// Update particle positions, velocities
void updateParticles(const float& dt)
{
	// Electrostatically interacting particles
	for(const auto& particle: ChargedParticle::chargedParticleCollection)
	{
		particle -> update(dt);
	}
}

// Contains calculations for the scattering processes
const vec3& runExperiment()
{
	while(1)
	{
		auto& chargedParticleCollection = ChargedParticle::chargedParticleCollection;
		// auto& proton_1 = chargedParticleCollection[0];
		// auto& proton_2 = chargedParticleCollection[1];
		auto& electron = chargedParticleCollection[2];
		calculateForces();
		updateParticles(DT_STEP);
		const vec3& electronPosition = electron -> getPosition();
		if(glm::length(electronPosition) > 1) return electronPosition;
	}
}

// Deletes particles in the experiment
void clearExperiment()
{
	Particle::particleCollection.clear();
	ChargedParticle::chargedParticleCollection.clear();
}
