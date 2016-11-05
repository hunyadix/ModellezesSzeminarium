#ifndef PROTON_H
#define PROTON_H

#include "ChargedParticle.h"

class Proton: public ChargedParticle
{
	public:
		static constexpr float protonCharge = 1.602176e-19f;
		static constexpr float protonMass   = 1.672e-27;
	public:
		Proton()                                                 :ChargedParticle(1.0f, protonCharge, protonMass, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1.0, 0.1, 0.1)) {}
		Proton(const vec3& positionArg)                          :ChargedParticle(1.0f, protonCharge, protonMass, positionArg,   vec3(0, 0, 0), vec3(1.0, 0.1, 0.1)) {}
		Proton(const vec3& positionArg, const vec3& velocityArg) :ChargedParticle(1.0f, protonCharge, protonMass, positionArg,   velocityArg,   vec3(1.0, 0.1, 0.1)) {}
		virtual ~Proton() = default;
};

constexpr float Proton::protonCharge;
constexpr float Proton::protonMass;

#endif