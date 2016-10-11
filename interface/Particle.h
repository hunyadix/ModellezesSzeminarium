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
		vec3  position;
		vec3  velocity;
		vec3  force;
		float mass;
		vec3  color;
	public:
		Particle():                                                                                        radius(1),         position(vec3(0, 0, 0)), velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(float radiusArg):                                                                         radius(radiusArg), position(vec3(0, 0, 0)), velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(float radiusArg, const vec3& positionArg):                                                radius(radiusArg), position(positionArg),   velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) {}
		Particle(float radiusArg, const vec3& positionArg, const vec3& velocityArg):                       radius(radiusArg), position(positionArg),   velocity(velocityArg),   color(vec3(0.7, 0.7, 0.7)) {}
		Particle(float radiusArg, const vec3& positionArg, const vec3& velocityArg, const vec3& colorArg): radius(radiusArg), position(positionArg),   velocity(velocityArg),   color(colorArg)            {}
		virtual ~Particle() = default;
		virtual void update(float dt)
		{
			position += dt * velocity;
			velocity += dt * force / mass;
		}
		virtual void display()
		{
			glColor3f(color.x, color.y, color.z);
			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			glutSolidSphere(radius, 10, 10);
			glPopMatrix();
		}
		virtual void applyForce(vec3 potential) = 0;
};

#endif