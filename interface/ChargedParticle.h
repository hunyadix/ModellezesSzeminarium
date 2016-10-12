#ifndef CHARGED_PARTICLE_H
#define CHARGED_PARTICLE_H

#include "Particle.h"

class ChargedParticle: public Particle
{
	protected:
		float charge;
		static constexpr float coulombConstant = 8.99e9;
	public:
		ChargedParticle():
		Particle(), charge(0) {}
		ChargedParticle(const float& radiusArg, const float& chargeArg):
		Particle(radiusArg), charge(chargeArg) {}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg):
		Particle(radiusArg, massArg), charge(chargeArg) {}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg, const vec3& positionArg):
		Particle(radiusArg, massArg, positionArg), charge(chargeArg) {}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg):
		Particle(radiusArg, massArg, positionArg, velocityArg), charge(chargeArg) {}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg, const vec3& color):
		Particle(radiusArg, massArg, positionArg, velocityArg, color), charge(chargeArg) {}
		virtual ~ChargedParticle() = default;
		virtual void calculateForceFromPotential(const vec3& potential)
		{
			force = -charge * potential;
		}
		virtual vec3 getPotentialAt(const vec3& positionArg) const
		{
			float distance = glm::length(position - positionArg);
			return coulombConstant * charge / (distance * distance) * (position - positionArg);
		}
};

#endif