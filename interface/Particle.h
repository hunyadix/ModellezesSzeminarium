#ifndef PARTICLE_H
#define PARTICLE_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>

#include "Drawable.h"

using glm::vec3;

class Particle: public Drawable
{
	protected:
		float radius;
		float mass;
		vec3  position;
		vec3  velocity;
		vec3  force;
		vec3  color;
	public:
		Particle():
			radius(1.0f), mass(1.0e6f), position(vec3(0.0, 0.0, 0.0)),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(const float& radiusArg):
			radius(radiusArg), mass(1.0e6f), position(vec3(0, 0, 0)),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(const float& radiusArg, const float& massArg):
			radius(radiusArg), mass(massArg), position(vec3(0, 0, 0)),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(const float& radiusArg, const vec3& positionArg):
			radius(radiusArg), mass(1e6), position(positionArg),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(const float& radiusArg, const float& massArg, const vec3& positionArg):
			radius(radiusArg), mass(massArg), position(positionArg),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(const float& radiusArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg):
			radius(radiusArg), mass(massArg), position(positionArg),
			velocity(velocityArg), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(const float& radiusArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg, const vec3& colorArg):
			radius(radiusArg), mass(massArg), position(positionArg),
			velocity(velocityArg), color(colorArg) {}
		virtual ~Particle() = default;
		vec3 getPosition() const { return position; }
		void setPosition(const vec3& positionArg) { position = positionArg; }
		vec3 getVelocity() const { return velocity; }
		void setVelocity(const vec3& velocityArg) { velocity = velocityArg; }
		virtual void update(const float& dt)
		{
			position += dt * velocity;
			velocity += dt * force / mass;
		}
		virtual void display() const
		{
			glColor3f(color.x, color.y, color.z);
			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			glutSolidSphere(radius, 10, 10);
			glPopMatrix();
		}
		virtual void calculateForceFromPotential(const vec3& potential) = 0;
};

#endif