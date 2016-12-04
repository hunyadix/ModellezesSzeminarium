#ifndef CHARGED_PARTICLE_H
#define CHARGED_PARTICLE_H

#include "Particle.h"
#include <memory>
#include <algorithm>

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
		virtual float calculatePotentialEnergy() const
		{
			return charge * glm::length(getPotentialFromOtherParticles());
		}
		virtual float calculatePotentialEnergy(const vec3& potential) const
		{
			return charge * glm::length(potential);
		}
		// Runge-Kutta 4 prediction
		// Predicted quantity: p position at t + dt
		// Simpsons method for integration of f(x) between a and b:
		// \integral_a^b f(x) = (b - a) / 6 [f(a) + 4 * f((a + b) / 2) + f(b)] + Ordo((b - a)^5)
		// Here the Runge-Kutta method is used for two linked equations
		virtual void update(const float& dt)
		{
			static const float oneOverSix = 1.0f / 6.0f;
			// std::cout << "Before: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
			vec3 k1 = dt * getAccelerationAtPosition(position);
			vec3 l1 = dt * velocity;
			vec3 k2 = dt * getAccelerationAtPosition(position + 0.5f * l1);
			vec3 l2 = dt * (velocity + 0.5f * k1);
			vec3 k3 = dt * getAccelerationAtPosition(position + 0.5f * l2);
			vec3 l3 = dt * (velocity + 0.5f * k2);
			vec3 k4 = dt * getAccelerationAtPosition(position + l3);
			vec3 l4 = dt * (velocity + k3);
			velocity = velocity + oneOverSix * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
			position = position + oneOverSix * (l1 + 2.0f * l2 + 2.0f * l3 + l4);
			// std::cout << "After: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
			// std::cin.get();
		}
		static std::vector<ChargedParticle*> chargedParticleCollection;
		vec3 getPotentialFromOtherParticlesAtPosition(vec3 positionArg) const
		{
			vec3 potential(0, 0, 0);
			for(const auto& chargedParticle: chargedParticleCollection)
			{
				if(chargedParticle == this) continue;
				float distance = glm::length(positionArg - chargedParticle -> getPosition());
				potential += (coulombConstant * chargedParticle -> getCharge()) / (distance * distance) * (chargedParticle -> getPosition() - positionArg);
			}
			return potential;
		}
		vec3 getPotentialFromOtherParticles() const
		{
			return getPotentialFromOtherParticlesAtPosition(position);
		}
		vec3 getAccelerationAtPosition(vec3 positionArg)
		{
			return -charge / mass * getPotentialFromOtherParticlesAtPosition(positionArg);
		}
};

// n++;
// t = t + dt;
// t_array.push( t );

std::vector<ChargedParticle*> ChargedParticle::chargedParticleCollection;

#endif