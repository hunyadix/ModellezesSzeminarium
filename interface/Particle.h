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

void normalize(float v[3], float length = 1.0f)
{    
   GLfloat d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]); 
   if (d == 0.0) {
      std::cout << "zero length vector" << std::endl;
      return;
   }
   v[0] *= (length / d);
   v[1] *= (length / d); 
   v[2] *= (length / d); 
}

void drawtriangle(float *v1, float *v2, float *v3) 
{ 
   glBegin(GL_TRIANGLES); 
      glNormal3fv(v1); glVertex3fv(v1);    
      glNormal3fv(v2); glVertex3fv(v2);    
      glNormal3fv(v3); glVertex3fv(v3);    
   glEnd(); 
}

void subdivide(const float& radius, float *v1, float *v2, float *v3, long depth)
{
   GLfloat v12[3], v23[3], v31[3];
   GLint i;

   if (depth == 0) {
      drawtriangle(v1, v2, v3);
      return;
   }
   for (i = 0; i < 3; i++) {
      v12[i] = v1[i]+v2[i];
      v23[i] = v2[i]+v3[i];
      v31[i] = v3[i]+v1[i];
   }
   normalize(v12, radius);
   normalize(v23, radius);
   normalize(v31, radius);
   subdivide(radius, v1, v12, v31, depth-1);
   subdivide(radius, v2, v23, v12, depth-1);
   subdivide(radius, v3, v31, v23, depth-1);
   subdivide(radius, v12, v23, v31, depth-1);
}

class Particle: public Drawable
{
	protected:
		float radius;
		float mass;
		vec3  position;
		vec3  velocity;
		// vec3  acceleration = vec3(0.0, 0.0, 0.0);
		vec3  force;
		vec3  color;
	public:
		Particle():
			radius(1.0f), mass(1.0e6f), position(vec3(0.0, 0.0, 0.0)),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) 
			{
				particleCollection.push_back(std::shared_ptr<Particle>(this));
			}
		Particle(const float& radiusArg):
			radius(radiusArg), mass(1.0e6f), position(vec3(0, 0, 0)),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) 
			{
				particleCollection.push_back(std::shared_ptr<Particle>(this));
			}
		Particle(const float& radiusArg, const float& massArg):
			radius(radiusArg), mass(massArg), position(vec3(0, 0, 0)),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) 
			{
				particleCollection.push_back(std::shared_ptr<Particle>(this));
			}
		Particle(const float& radiusArg, const vec3& positionArg):
			radius(radiusArg), mass(1e6), position(positionArg),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) 
			{
				particleCollection.push_back(std::shared_ptr<Particle>(this));
			}
		Particle(const float& radiusArg, const float& massArg, const vec3& positionArg):
			radius(radiusArg), mass(massArg), position(positionArg),
			velocity(vec3(0, 0, 0)), color(vec3(0.7, 0.7, 0.7)) 
			{
				particleCollection.push_back(std::shared_ptr<Particle>(this));
			}
		Particle(const float& radiusArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg):
			radius(radiusArg), mass(massArg), position(positionArg),
			velocity(velocityArg), color(vec3(0.7, 0.7, 0.7)) 
			{
				particleCollection.push_back(std::shared_ptr<Particle>(this));
			}
		Particle(const float& radiusArg, const float& massArg, const vec3& positionArg, const vec3& velocityArg, const vec3& colorArg):
			radius(radiusArg), mass(massArg), position(positionArg),
			velocity(velocityArg), color(colorArg) 
			{
				particleCollection.push_back(std::shared_ptr<Particle>(this));
			}
		virtual ~Particle() = default;
		vec3 getPosition() const { return position; }
		void setPosition(const vec3& positionArg) { position = positionArg; }
		vec3 getVelocity() const { return velocity; }
		void setVelocity(const vec3& velocityArg) { velocity = velocityArg; }
		float calculateKineticEnergy() const { return 0.5f * mass * glm::dot(velocity, velocity); }
		virtual void update(const float& dt)
		{
			std::cout << "Error: Update unimplemented for class Particle(). Consider calling a child's method." << std::endl;
		}
		virtual void display() const
		{
			// This creates a sphere:
			// Starts with a regular solid (icosahedron), 
			// then divides the triangles on its faces,
			// then moves them to equal distance from the central point (normalizes and scalest the vertex as vector)
			glColor3f(color.x, color.y, color.z);
			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			static const GLfloat X = .525731112119133606;
			static const GLfloat Z = .850650808352039932;
			static GLfloat vdata[12][3] = {    
			   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
			   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
			   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
			};
			static const GLuint tindices[20][3] = { 
			   {0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},    
			   {8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},    
			   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
			   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11} };
			for(int i = 0; i < 20; i++) 
			{
				subdivide(
					radius, 
					&vdata[tindices[i][0]][0],
					&vdata[tindices[i][1]][0],
					&vdata[tindices[i][2]][0], (long)(3)); 
			}
			// Simple glut version, requires glut
			// glutSolidSphere(radius, 10, 10); 
			glPopMatrix();
		}
		virtual void calculateForceFromPotential(const vec3& potential) = 0;
		virtual float calculatePotentialEnergy(const vec3& potential) const = 0;
		static std::vector<std::shared_ptr<Particle>> particleCollection;
};

std::vector<std::shared_ptr<Particle>> Particle::particleCollection;

#endif
