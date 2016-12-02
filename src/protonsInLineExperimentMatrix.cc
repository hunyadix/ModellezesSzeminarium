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

// Constants

// const float COULOMB_CONSTANT                     = ChargedParticle::coulombConstant; // 1.00f
const float SPEED_OF_LIGHT                        = 137;
const float ELECTRON_CHARGE                       = Electron::electronCharge;                // -1.0f
const float ELECTRON_MASS                         = Electron::electronMass;                  // 1.0f
const float PROTON_CHARGE                         = Proton::protonCharge;                    // +1.0f
const float PROTON_MASS                           = Proton::protonMass;                      // 1836.153f;
const float HARTREE_ENERGY_IN_EV                  = 27.211386f;
const float EV_TO_VELOCITY_SQUARED                = 2.0f / HARTREE_ENERGY_IN_EV;
const float HIDROGEN_BOND_LENGTH                  = 1.3983899f;                              // in Bohrs

const int   NUM_EXPERIMENTS_PER_SETUP             = 10000;
const float DT_STEP                               = 1.0e-2f;
const float NUM_UPDATES_TO_CONSIDER_ELEC_ABSORBED = 1e4;
// const float DT_STEP                               = 1.0e-18f;
const int   NUMBER_OF_PROTONS                     = 2;                                       // should be even
const float PROTON_PROTON_DISTANCE                = HIDROGEN_BOND_LENGTH;
const float ELECTRON_START_PLANE_DISTANCE         = 5.0f * HIDROGEN_BOND_LENGTH;
const float ELECTRON_END_PLANE_DISTANCE           = 5.0f * HIDROGEN_BOND_LENGTH;
const float ELECTRON_START_KINETIC_ENERGY_EV_MIN  = 1;
const float ELECTRON_START_KINETIC_ENERGY_EV_MAX  = 15;
const float ELECTRON_START_KINETIC_ENERGY_EV_STEP = 1;
const int   ELECTRON_START_KIN_EN_NUM_MEAS_POINTS = std::floor((ELECTRON_START_KINETIC_ENERGY_EV_MAX - ELECTRON_START_KINETIC_ENERGY_EV_MIN) / ELECTRON_START_KINETIC_ENERGY_EV_STEP) + 1;


// Histogram definition
const float END_POS_MIN_RANGE = -4.0f * PROTON_PROTON_DISTANCE; 
const float END_POS_MAX_RANGE = +4.0f * PROTON_PROTON_DISTANCE;
const float END_POS_NUM_BINS  = 200;

// Test for 6 protons; positions in p-p distances: -2.5, -1.5, -0.5, 0.5, 1.5, 2.5
constexpr float protonPosition(int index) { return (-0.5f * NUMBER_OF_PROTONS + index + 0.5f) * PROTON_PROTON_DISTANCE; } 
const std::vector<float> PROTONPOSITIONS(NUMBER_OF_PROTONS, 0);
constexpr float electronStartVelocity(int index) { return sqrt((ELECTRON_START_KINETIC_ENERGY_EV_MIN + (index * ELECTRON_START_KINETIC_ENERGY_EV_STEP)) * EV_TO_VELOCITY_SQUARED); } 
const std::vector<float> ELECTRON_START_VELOCITIES(ELECTRON_START_KIN_EN_NUM_MEAS_POINTS, 0);
constexpr float electronStartKineticEnergy(int index) { return (ELECTRON_START_KINETIC_ENERGY_EV_MIN + (index * ELECTRON_START_KINETIC_ENERGY_EV_STEP)); } 
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
		std::cout << "Starting e- kinetic energy, speed: " << std::resetiosflags(std::ios::fixed) << std::setw(8) << energy << " eV | " << std::setprecision(5) << std::setw(8) << energy / HARTREE_ENERGY_IN_EV << " hartree | " << std::setw(8) << speed << " alpha * c\n";
		++i;
	}
	std::cout << "---------------------------\n";
	std::cout << std::flush;
}

// Function declarations
void        physicsMain();
void        setProtonPositions();
void        setElectronStartPositionVelocity(const int& numSetup);
void        initExperiment(const int& numSetup);
// void        calculateForces();
// void        updateParticles(const float& dt);
const vec3& runExperiment(int & numUpdates);
void        clearExperiment();

TApplication* theApp = nullptr;

int main(int argc, char **argv)
{
	std::cout << argv[0] << " started..." << std::endl;
	initConsts();
	theApp = new TApplication("App", &argc, argv);
	physicsMain();
	return 0;
}

void physicsMain()
{
	std::cout << "\n";
	printPhysicsInfo();
	std::cout << "\n";
	// TH1D electronPositionsX ("electronPositionsX",  "Electron positions X",  200, -PROTON_PROTON_DISTANCE, PROTON_PROTON_DISTANCE);
	// TH2D electronPositionsXY("electronPositionsXY", "Electron positions XY", 200, -PROTON_PROTON_DISTANCE, PROTON_PROTON_DISTANCE, 200, -PROTON_PROTON_DISTANCE, PROTON_PROTON_DISTANCE);
	TH2D electronEnergyEndPositionsX_H ("electronEnergyEndPositionsX",  "Electron end position distribution vs starting kin. energy;x pos(bohr);starting kin. energy (eV)",  
		END_POS_NUM_BINS,                      END_POS_MIN_RANGE,                                                                  END_POS_MAX_RANGE,
		ELECTRON_START_KIN_EN_NUM_MEAS_POINTS, ELECTRON_START_KINETIC_ENERGY_EV_MIN - 0.5 * ELECTRON_START_KINETIC_ENERGY_EV_STEP, ELECTRON_START_KINETIC_ENERGY_EV_MAX + 0.5 * ELECTRON_START_KINETIC_ENERGY_EV_STEP);
	std::vector<long long> totalNumUpdates;
	static_assert(64 <= 8 * sizeof(long long), "WARNING: Progress sizes might not be calculated properly, since your architecture implements long long with size less than 64 bits.");
	std::vector<int> electronsAbsorbed;
	Pbar totalProgress;
	auto start = time(NULL);
	for(int measurementPointIndex = 0; measurementPointIndex < ELECTRON_START_KIN_EN_NUM_MEAS_POINTS; ++measurementPointIndex)
	{
		totalNumUpdates  .push_back(0);
		electronsAbsorbed.push_back(0);
		const auto& startKineticEnergyFillPos = ELECTRON_START_KIN_ENERGIES[measurementPointIndex];
		Pbar experimentProgress;
		for(int experimentNumber = 0; experimentNumber < NUM_EXPERIMENTS_PER_SETUP; ++experimentNumber)
		{
			int numUpdates = 0;
			initExperiment(measurementPointIndex);
			vec3 electronHitPosition = runExperiment(numUpdates);
			clearExperiment();
			if(numUpdates == -1)
			{
				electronsAbsorbed.back()++;
				experimentNumber -= 1;
				continue;
			}
			if(experimentNumber % (NUM_EXPERIMENTS_PER_SETUP / 100) == 0)
			{
				totalProgress.update(1.0f / ELECTRON_START_KIN_EN_NUM_MEAS_POINTS);
				experimentProgress.update(1);
				totalProgress.print();
				experimentProgress.printNoUpdate();
			// 	std::cout << experimentNumber * 100.0f / NUM_EXPERIMENTS << '%' << std::endl;
			}
			// std::cout << "Filling at: " << electronHitPosition.x << ", " << startKineticEnergyFillPos << std::endl; std::cin.get();
			electronEnergyEndPositionsX_H.Fill(electronHitPosition.x, startKineticEnergyFillPos);
			totalNumUpdates.back() += numUpdates;
		}
	}
	std::cout << "\n\n";
	auto end = time(NULL);
	std::cout << "Took about: " << end - start << " second(s)." << std::endl;
	std::cout << "Average number of updates per experiment: \n";
	for(auto i: range(totalNumUpdates.size()))
		std::cout << "\t" << std::resetiosflags(std::ios::fixed) << std::setw(8) << totalNumUpdates[i] / static_cast<double>(NUM_EXPERIMENTS_PER_SETUP) << " with " << std::setw(5) << electronsAbsorbed[i] << " experiment fails because of electron absorbtion" << std::endl;
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
	x = rand() / static_cast<double>(RAND_MAX) * 4.0f * PROTON_PROTON_DISTANCE - 2.0f * PROTON_PROTON_DISTANCE;
	y = ELECTRON_START_PLANE_DISTANCE;
	z = 0.0f;
	vx = 0.0f;
	vy = -ELECTRON_START_VELOCITIES[numSetup];
	vz = 0.0f;
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
const vec3& runExperiment(int& numUpdates)
{
	numUpdates = 0;
	while(1)
	{
		numUpdates++;
		auto& electron = ChargedParticle::chargedParticleCollection[NUMBER_OF_PROTONS];
		electron -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
		electron -> update(DT_STEP);
		const vec3& electronPosition = electron -> getPosition();
		if(electronPosition.y < -ELECTRON_END_PLANE_DISTANCE)
		{
			return electronPosition;
		}
		if(NUM_UPDATES_TO_CONSIDER_ELEC_ABSORBED < numUpdates)
		{
			numUpdates = -1;
			// std::cout << "Electron absorbed: " << electronPosition.x << ", " << electronPosition.y << ", " << electronPosition.z << std::endl;
			// std::cin.get();
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
