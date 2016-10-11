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
		vec3  position;
		vec3  velocity;
		vec3  force;
		float mass;
		float radius;
	public:
		Particle()                                                                                                    { radius   = 1; position = vec3(0, 0, 0); velocity = vec3(0, 0, 0); }
		Particle(float radiusArg):                                                   Particle()                       { radius   = radiusArg; }
		Particle(float radiusArg, const vec3& positionArg):                          Particle(radiusArg)              { position = positionArg; velocity = vec3(0, 0, 0); }
		Particle(float radiusArg, const vec3& positionArg, const vec3& velocityArg): Particle(radiusArg, positionArg) { velocity = velocityArg;}
		virtual ~Particle() = default;
		virtual void update(float dt)
		{
			position += dt * velocity;
			velocity += dt * force / mass;
		}
		virtual void display()
		{
			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			glutSolidSphere(radius, 10, 10);
			glPopMatrix();
		}
		virtual void applyForce(vec3 potential) = 0;
};

#endif