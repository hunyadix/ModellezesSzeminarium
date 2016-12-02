#ifndef CHARGED_PARTICLE_H
#define CHARGED_PARTICLE_H

#include "Particle.h"
#include <memory>

class ChargedParticle: public Particle
{
	protected:
		float charge;
		static constexpr float vacuumPermittivity = 0.079577f;
		// static constexpr float coulombConstant = 1.0f / (4.0f * 3.1415926f * vacuumPermittivity);
		static constexpr float coulombConstant = 1.0f;
	public:
		ChargedParticle():
		Particle(), charge(0) 
		{
			chargedParticleCollection.push_back(this);
		}
		ChargedParticle(const float& radiusArg, const float& chargeArg):
		Particle(radiusArg), charge(chargeArg) 
		{
			chargedParticleCollection.push_back(this);
		}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg):
		Particle(radiusArg, massArg), charge(chargeArg) 
		{
			chargedParticleCollection.push_back(this);
		}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg, const vec3& positionArg):
		Particle(radiusArg, massArg, positionArg), charge(chargeArg) 
		{
			chargedParticleCollection.push_back(this);
		}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg):
		Particle(radiusArg, massArg, positionArg, velocityArg), charge(chargeArg) 
		{
			chargedParticleCollection.push_back(this);
		}
		ChargedParticle(const float& radiusArg, const float& chargeArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg, const vec3& color):
		Particle(radiusArg, massArg, positionArg, velocityArg, color), charge(chargeArg) 
		{
			chargedParticleCollection.push_back(this);
		}
		virtual ~ChargedParticle() = default;
		float getCharge() const
		{
			return charge;
		}
		void setCharge(const float& chargeArg)
		{
			charge = chargeArg;
		}
		virtual void calculateForceFromPotential(const vec3& potential)
		{
			force = -charge * potential;
		}
		void calculateForceFromChargedParticleCollection(const std::vector<ChargedParticle*>& chargedParticleCollection)
		{
			vec3 potential(0, 0, 0);
			for(const auto& particle: chargedParticleCollection)
			{
				if(particle == this) continue;
				potential += particle -> getPotentialAt(position);
			}
			return calculateForceFromPotential(potential);
		}
		virtual vec3 getPotentialAt(const vec3& positionArg) const
		{
			float distance = glm::length(position - positionArg);
			return coulombConstant * charge / (distance * distance) * (position - positionArg);
		}
		virtual float calculatePotentialEnergy(const vec3& potential) const
		{
			return charge * glm::length(potential);
		}
		static std::vector<ChargedParticle*> chargedParticleCollection;
};

std::vector<ChargedParticle*> ChargedParticle::chargedParticleCollection;

#endif