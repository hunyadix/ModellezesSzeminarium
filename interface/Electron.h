#ifndef ELECTRON_H
#define ELECTRON_H

#include "ChargedParticle.h"

class Electron: public ChargedParticle
{
	public:
		Electron()                                                 :ChargedParticle(0.1, -1, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.1, 0.1, 1.0)) {}
		Electron(const vec3& positionArg)                          :ChargedParticle(0.1, -1, positionArg,   vec3(0, 0, 0), vec3(0.1, 0.1, 1.0)) {}
		Electron(const vec3& positionArg, const vec3& velocityArg) :ChargedParticle(0.1, -1, positionArg,   velocityArg,   vec3(0.1, 0.1, 1.0)) {}
};

#endif