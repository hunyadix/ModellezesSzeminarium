#ifndef ELECTRON_H
#define ELECTRON_H

#include "ChargedParticle.h"

class Electron: public ChargedParticle
{
	public:
		static constexpr float electronCharge = -1.602176e-19f;
		static constexpr float electronMass   = 9.109e-31f;
	public:
		Electron()                                                 :ChargedParticle(0.20f, electronCharge, electronMass, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.1, 0.1, 1.0)) {}
		Electron(const vec3& positionArg)                          :ChargedParticle(0.20f, electronCharge, electronMass, positionArg,   vec3(0, 0, 0), vec3(0.1, 0.1, 1.0)) {}
		Electron(const vec3& positionArg, const vec3& velocityArg) :ChargedParticle(0.20f, electronCharge, electronMass, positionArg,   velocityArg,   vec3(0.1, 0.1, 1.0)) {}
		virtual ~Electron() = default;
};

constexpr float Electron::electronCharge;
constexpr float Electron::electronMass;

#endif