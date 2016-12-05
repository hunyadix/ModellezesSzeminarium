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
#include <TText.h>
#include <TError.h>

#include "../interface/Electron.h"
#include "../interface/Proton.h"

#include "../interface/Pbar.h"

#include "../interface/HelperFunctionsCommon.h"

// Constants

// const float COULOMB_CONSTANT                     = ChargedParticle::coulombConstant; // 1.00f
constexpr float SPEED_OF_LIGHT                        = 137;
constexpr float ELECTRON_CHARGE                       = Electron::electronCharge;                // -1.0f
constexpr float ELECTRON_MASS                         = Electron::electronMass;                  // 1.0f
constexpr float PROTON_CHARGE                         = Proton::protonCharge;                    // +1.0f
constexpr float PROTON_MASS                           = Proton::protonMass;                      // 1836.153f;
constexpr float HARTREE_ENERGY_IN_EV                  = 27.211386f;
constexpr float EV_TO_VELOCITY_SQUARED                = 2.0f / HARTREE_ENERGY_IN_EV;
constexpr float HIDROGEN_BOND_LENGTH                  = 1.3983899f;                              // in Bohrs
constexpr float DT_STEP                               = 1.0e-2f;
constexpr int   NUM_EXPERIMENTS_PER_SETUP             = 1000;
constexpr int   NUM_UPDATES_BEFORE_ABSORBTION_TESTING = 1e3;
constexpr int   NUM_ITERATIONS_BETWEEN_ABS_TESTS      = 1e2;
constexpr int   NUMBER_OF_PROTONS                     = 20;                                       // should be even
constexpr float PROTON_PROTON_DISTANCE                = HIDROGEN_BOND_LENGTH;
constexpr float ELECTRON_START_X_POS_MIN              = -4.0f * HIDROGEN_BOND_LENGTH;
constexpr float ELECTRON_START_X_POS_MAX              = +4.0f * HIDROGEN_BOND_LENGTH;
constexpr float ELECTRON_START_PLANE_DISTANCE         = 50.0f  * HIDROGEN_BOND_LENGTH;
constexpr float ELECTRON_END_PLANE_DISTANCE           = 100.0f * HIDROGEN_BOND_LENGTH;
constexpr float ELECTRON_START_KINETIC_ENERGY_EV_MIN  = 1000;
constexpr float ELECTRON_START_KINETIC_ENERGY_EV_MAX  = 1000;
constexpr float ELECTRON_START_KINETIC_ENERGY_EV_STEP = 100;
constexpr int   ELECTRON_START_KIN_EN_NUM_MEAS_POINTS = std::floor((ELECTRON_START_KINETIC_ENERGY_EV_MAX - ELECTRON_START_KINETIC_ENERGY_EV_MIN) / ELECTRON_START_KINETIC_ENERGY_EV_STEP) + 1;

// Histogram definition
constexpr int   SAVE_POSITION_DISTRIBUTIONS           = 1;
constexpr float END_POS_MIN_RANGE                     = -30.0f * PROTON_PROTON_DISTANCE; 
constexpr float END_POS_MAX_RANGE                     = +30.0f * PROTON_PROTON_DISTANCE;
constexpr float END_POS_NUM_BINS                      = 600;

// Screenshots/video
constexpr int   SAVE_2D_SCREENSHOTS                   = 1;
constexpr float ELECTRON_START_Z_POS_MIN              = -0.05f * HIDROGEN_BOND_LENGTH;                 // only matters when SAVE_2D_SCREENSHOTS is set
constexpr float ELECTRON_START_Z_POS_MAX              = +0.05f * HIDROGEN_BOND_LENGTH;                 // only matters when SAVE_2D_SCREENSHOTS is set
constexpr float SCREENSHOTS_X_POS_MIN_RANGE           = END_POS_MIN_RANGE;
constexpr float SCREENSHOTS_X_POS_MAX_RANGE           = END_POS_MAX_RANGE;
constexpr float SCREENSHOTS_X_BIN_NUMBER              = 6000;
constexpr float SCREENSHOTS_Z_POS_MIN_RANGE           = END_POS_MIN_RANGE;
constexpr float SCREENSHOTS_Z_POS_MAX_RANGE           = END_POS_MAX_RANGE;
constexpr float SCREENSHOTS_Z_BIN_NUMBER              = 6000;
constexpr float SCREENSHOTS_MARKERSIZES               = 0.5;


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
void       drawSampleElectronPaths(const int& numPaths);
const vec3 runExperiment(int& numUpdates, int& experimentSuccesful);
void       clearExperiment();

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
	std::vector<std::shared_ptr<TH1D>> electronPositionsX_V;
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
		int errorTriggered = 0;
		electronPositionsX_V.emplace_back(std::make_shared<TH1D>(("electronPositionsX" + std::to_string(measurementPointIndex)).c_str(),  "Electron positions X",  END_POS_NUM_BINS, END_POS_MIN_RANGE, END_POS_MAX_RANGE));
		const auto& startKineticEnergyFillPos = ELECTRON_START_KIN_ENERGIES[measurementPointIndex];
		Pbar experimentProgress;
		TH2D screenshots_H(("screenshots_" + std::to_string(measurementPointIndex)).c_str(),  "Electron hit positions on plane ;x pos(bohr);z pos (bohr)",  
			SCREENSHOTS_X_BIN_NUMBER, SCREENSHOTS_X_POS_MIN_RANGE, SCREENSHOTS_X_POS_MAX_RANGE, SCREENSHOTS_Z_BIN_NUMBER, SCREENSHOTS_Z_POS_MIN_RANGE, SCREENSHOTS_Z_POS_MAX_RANGE);
		screenshots_H.SetMarkerStyle(8);
		screenshots_H.SetMarkerSize (SCREENSHOTS_MARKERSIZES);
		screenshots_H.SetMarkerColor(kOrange + 1);
		for(int experimentNumber = 0; experimentNumber < NUM_EXPERIMENTS_PER_SETUP; ++experimentNumber)
		{
			int numUpdates = 0;
			int experimentSuccesful = 0;
			initExperiment(measurementPointIndex);
			vec3 electronHitPosition = runExperiment(numUpdates, experimentSuccesful);
			clearExperiment();
			if(!experimentSuccesful)
			{
				electronsAbsorbed.back()++;
				if(!errorTriggered && NUM_EXPERIMENTS_PER_SETUP / 2 < electronsAbsorbed.back())
				{
					errorTriggered = 1;
					std::cout << "Warning: more than half of the electrons were already absorbed in this configuration." << std::endl;
				}
				experimentNumber -= 1;
				continue;
			}
			if(measurementPointIndex == 0 && SAVE_2D_SCREENSHOTS)
			{
				screenshots_H.Fill(electronHitPosition.x, electronHitPosition.z);
				if(experimentNumber == 900)
				{
					gROOT -> SetBatch(kTRUE);
					TCanvas screenshotsCanvas;
					screenshotsCanvas.cd();
					screenshots_H.Draw();
					gErrorIgnoreLevel = kWarning;
					screenshotsCanvas.Print(("screenshots_2/" + std::to_string(experimentNumber + 1) + ".eps").c_str());
					gROOT -> SetBatch(kFALSE);
				}
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
			electronPositionsX_V.back() -> Fill(electronHitPosition.x);
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
	if(SAVE_POSITION_DISTRIBUTIONS)
	{
		TCanvas canvas;
		canvas.cd();
		electronEnergyEndPositionsX_H.Draw("COLZ");
		canvas.Print("matrix.eps");
		gROOT -> SetBatch(kTRUE);
		for(auto electronHitPosition: electronPositionsX_V)
		{
			TCanvas xPosCanvas(electronHitPosition -> GetName(), electronHitPosition -> GetTitle(), 200, 200, 400, 300);
			xPosCanvas.cd();
			electronHitPosition -> SetFillColor(38);
			electronHitPosition -> Draw("BHIST");
			xPosCanvas.Print((xPosCanvas.GetName() + std::string(".eps")).c_str());
		}
		gROOT -> SetBatch(kFALSE);
	}
	std::cout << "\nThe program terminated succesfully (exit with Ctrl-c)." << std::endl;
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
	x = rand() / static_cast<double>(RAND_MAX) * (ELECTRON_START_X_POS_MAX - ELECTRON_START_X_POS_MIN) - ELECTRON_START_X_POS_MAX;
	y = ELECTRON_START_PLANE_DISTANCE;
	z = 0.0f;
	vx = 0.0f;
	vy = -ELECTRON_START_VELOCITIES[numSetup];
    if(SAVE_2D_SCREENSHOTS) vz = rand() / static_cast<double>(RAND_MAX) * (ELECTRON_START_Z_POS_MAX - ELECTRON_START_Z_POS_MIN) + ELECTRON_START_Z_POS_MIN;
    else                    vz = 0.0f;
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

void drawSampleElectronPaths(const int& numPaths)
{
	TH2D sampleElectron_path_H ("a",  "Electron path;x pos(bohr);y pos(bohr)",  
		2000, END_POS_MIN_RANGE, END_POS_MAX_RANGE,
		2000, -ELECTRON_END_PLANE_DISTANCE, ELECTRON_START_PLANE_DISTANCE);
	TH2D proton_positons_H("b",  "",  
		2000, END_POS_MIN_RANGE, END_POS_MAX_RANGE,
		2000, -ELECTRON_END_PLANE_DISTANCE, ELECTRON_START_PLANE_DISTANCE);
	for(const auto& protonPosition: PROTONPOSITIONS)
	{
		proton_positons_H.Fill(protonPosition, 0);
	}
	int numUpdates = 0;
	for(int pathIndex = 0; pathIndex < numPaths; pathIndex++)
	{
		initExperiment(0);
		while(1)
		{
			numUpdates++;
			auto& electron = ChargedParticle::chargedParticleCollection[NUMBER_OF_PROTONS];
			electron -> calculateForceFromChargedParticleCollection(ChargedParticle::chargedParticleCollection);
			electron -> update(DT_STEP);
			const vec3& electronPosition = electron -> getPosition();
			if(electronPosition.y < -ELECTRON_END_PLANE_DISTANCE)
			{
				break;
			}
			if(NUM_UPDATES_BEFORE_ABSORBTION_TESTING <= numUpdates)
			{
				// Check if the electron is absorbed
				if(numUpdates % NUM_ITERATIONS_BETWEEN_ABS_TESTS == 0)
				{
					if(electron -> calculatePotentialEnergy() + electron -> calculateKineticEnergy() < 0)
					{
						std::cout << "Warning: one of the sample electron paths has an absorbed electron." << std::endl;
						break;
					}
				}
			}
			sampleElectron_path_H.Fill(electronPosition.x, electronPosition.y);
		}
		clearExperiment();
	}
	proton_positons_H.SetMarkerStyle(8);
	proton_positons_H.SetMarkerSize (1.5);
	proton_positons_H.SetMarkerColor(kRed);
	TCanvas canvas;
	canvas.cd();
	sampleElectron_path_H.Draw();
	proton_positons_H.Draw("SAME");
	canvas.Print("Paths.eps");
	std::cout << "Done." << std::endl;
	std::cin.get();
}

// Contains calculations for the scattering processes
const vec3 runExperiment(int& numUpdates, int& experimentSuccesful)
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
			experimentSuccesful = 1; // No errors
			return electronPosition;
		}
		if(NUM_UPDATES_BEFORE_ABSORBTION_TESTING <= numUpdates)
		{
			// Check if the electron is absorbed
			if(numUpdates % NUM_ITERATIONS_BETWEEN_ABS_TESTS == 0)
			{
				if(electron -> calculatePotentialEnergy() + electron -> calculateKineticEnergy() < 0)
				{
					// std::cout << "Electron kin. energy: " << electron -> calculateKineticEnergy()   << std::endl;
					// std::cout << "Electron pot. energy: " << electron -> calculatePotentialEnergy() << std::endl;
					experimentSuccesful = 0; // When the experiment is not succesful: repeat it with different starting conditions
					return electronPosition;
					// std::cout << "Electron absorbed: " << electronPosition.x << ", " << electronPosition.y << ", " << electronPosition.z << std::endl;
				}
			}
		}
	}
}

// Deletes particles in the experiment
void clearExperiment()
{
	Particle::particleCollection.clear();
	ChargedParticle::chargedParticleCollection.clear();
}
