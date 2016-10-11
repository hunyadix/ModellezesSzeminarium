#ifndef PROTON_H
#define PROTON_H

#include "ChargedParticle.h"

class Proton: public ChargedParticle
{
	public:
		Proton()                                                 :ChargedParticle(1.0, 1, vec3(0, 0, 0), vec3(0, 0, 0)) {}
		Proton(const vec3& positionArg)                          :ChargedParticle(1.0, 1, positionArg,   vec3(0, 0, 0)) {}
		Proton(const vec3& positionArg, const vec3& velocityArg) :ChargedParticle(1.0, 1, positionArg,   velocityArg  ) {}
};

#endif