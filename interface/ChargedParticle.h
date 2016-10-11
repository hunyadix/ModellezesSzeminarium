#ifndef CHARGED_PARTICLE_H
#define CHARGED_PARTICLE_H

#include "Particle.h"

class ChargedParticle: public Particle
{
	protected:
		float charge;
		static constexpr float coulombConstant = 8.99e9;
	public:
		ChargedParticle():                                                                                                      Particle()                                           { charge = 0; }
		ChargedParticle(float radiusArg, float chargeArg):                                                                      Particle(radiusArg)                                  { charge = chargeArg; }
		ChargedParticle(float radiusArg, float chargeArg, const vec3& positionArg):                                             Particle(radiusArg, positionArg)                     { charge = chargeArg; }
		ChargedParticle(float radiusArg, float chargeArg, const vec3& positionArg, const vec3& velocityArg):                    Particle(radiusArg, positionArg, velocityArg)        { charge = chargeArg; }
		ChargedParticle(float radiusArg, float chargeArg, const vec3& positionArg, const vec3& velocityArg, const vec3& color): Particle(radiusArg, positionArg, velocityArg, color) { charge = chargeArg; }
		virtual ~ChargedParticle() = default;
		virtual void applyForce(vec3 potential)
		{
			force = charge * potential;
		}
		virtual void getPotentialAt(vec3 positionArg)
		{
			float distance = glm::length(position - positionArg);
			force = coulombConstant * charge / (distance * distance) * (position - positionArg);
		}
};

#endif