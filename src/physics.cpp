#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include "GL_framework.h"
#include <stdlib.h>
#include <time.h>

bool show_test_window = false;

float *partVerts;
float timePerFrame = 0.033;
float radius = 0.05f;
glm::vec3 gravity = { 0, -9.8, 0 };

glm::vec3 normal = { 0,0,0 };
float d;
glm::vec3 vNormal, vTangencial;
float coefFriction = 0.f;
float coefElasticity = 0.9f;
int frame;


namespace LilSpheres {
	extern const int maxParticles;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}

struct Particle {
	glm::vec3 pos;
	glm::vec3 lastPos;
	glm::vec3 vel;
	glm::vec3 lastVel;
};

Particle *particlesContainer = new Particle[LilSpheres::maxParticles];
//Particle **particlesContainer2D;

namespace Sphere {
	extern glm::vec3 centro = { 0.f, 1.f, 0.f };
	extern void setupSphere(glm::vec3 pos = centro, float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();

}

struct laSphere {
	glm::vec3 pos;
	float radius;
};

laSphere *sphere = new laSphere();

namespace ClothMesh {
	extern void setupClothMesh();
	extern void cleanupClothMesh();
	extern void updateClothMesh(float* array_data);
	extern void drawClothMesh();
}

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::SliderFloat("Gravity", &gravity.y, -15, 15);

		//GUI Waterfall
		ImGui::SliderFloat("Coef.Elasticity", &coefElasticity, 0, 1);
		ImGui::SliderFloat("Coef.Friction", &coefFriction, 0, 1);
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void RandPosSphere() {
	srand(time(NULL));
	sphere->pos = { rand()%8-4, 2, rand() % 8 - 4 };
	sphere->radius = 1.0f;
}

void InitVerts() {

	partVerts = new float[LilSpheres::maxParticles * 3];
	RandPosSphere();
	float dist = -2.8f;
	float zDist = -3.2f;
	for (int i = 0; i < 252; ++i) {
		if (i % 14 == 0) {
			zDist += 0.4f;
			dist = -2.8f;
		}
		partVerts[i * 3 + 0] = dist;
		partVerts[i * 3 + 1] = 5;
		partVerts[i * 3 + 2] = zDist;
		dist += 0.4f;
	}

	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		particlesContainer[i].pos = glm::vec3(partVerts[i * 3], partVerts[i * 3 + 1], partVerts[i * 3 + 2]);
		particlesContainer[i].vel = glm::vec3(0, 0, 0); //random
	}

	/*particlesContainer2D = new Particle*[18];
	for (int i = 0; i < 18; i++) {
		particlesContainer2D[i] = new Particle[14];
	}

	int q = 0; 

	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {
			particlesContainer2D[i][j] = particlesContainer[q];
			q++;
		}
	}*/
}

void Srings(Particle *particlesContainer) {
	for (int i = 0; i < 252; i++) {
		
		if (i <= 13) {
			if (i == 0 || i == 13) {
				break;
			}
			if (i == 1 || i == 12) {
				//TODO
				break;
			}
			//GRUPO 1
			break;
		}
		if (i >= 14 && i <= 27 ) {
			if (i == 14 || i == 27) {
				//TODO
				break;
			}
			if (i == 15 || i == 26) {
				//TODO
				break;
			}
			//GRUPO 1
			break;
		}
		if (i >= 224 && i <= 237) {
			if (i == 224 || i == 237) {
				//TODO
				break;
			}
			if (i == 225 || i == 236) {
				//TODO
				break;
			}
			//GRUPO 2
			break;
		}
		if (i >= 238) {
			if (i == 238 || i == 251) {
				//TODO
				break;
			}
			if (i == 239 || i == 250) {
				//TODO
				break;
			}

			//GRUPO 2
			break;
		}
		if (i >= 28 && i <= 223) {
			if (i % 14 == 0) {
				//TODO
				break;
			}
			if (i % 14 == 1) {
				//TODO
				break;
			}
			if (i % 14 == 12) {
				//TODO
				break;
			}
			if (i % 14 == 13) {
				//TODO
				break;
			}
			else {
				//TODO
				break;
			}
		}
	}
}

void UpdatePosition(Particle *particlesContainer) {
	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		if (i != 0 && i != 13) {

			particlesContainer[i].lastVel = particlesContainer[i].vel;

			//update vector velocity velocity with formula
			particlesContainer[i].vel = particlesContainer[i].lastVel + gravity * timePerFrame;

			//save last position 
			particlesContainer[i].lastPos = particlesContainer[i].pos;

			//update position with formula
			particlesContainer[i].pos = particlesContainer[i].lastPos + timePerFrame * particlesContainer[i].lastVel; //components x and z have 0 gravity.

		}
	}

}

void UpdateColision(Particle *particlesContainer, int i) {
	//friction values
	vNormal = glm::dot(normal, particlesContainer[i].vel) * normal;
	vTangencial = particlesContainer[i].vel - vNormal;

	particlesContainer[i].pos = particlesContainer[i].pos - (1 + coefElasticity) * (glm::dot(normal, particlesContainer[i].pos) + d)*normal;
	particlesContainer[i].vel = particlesContainer[i].vel - (1 + coefElasticity) * (glm::dot(normal, particlesContainer[i].vel))*normal - coefFriction*vTangencial;
}

void CheckColision(Particle *particlesContainer) {
	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		//FLOOR
		if (particlesContainer[i].pos.y <= 0 + radius) {
			normal = { 0,1,0 };
			d = 0;
			UpdateColision(particlesContainer, i);
		}
		//LEFT WALL
		if (particlesContainer[i].pos.x <= -5 + radius) {
			normal = { 1,0,0 };
			d = 5;
			UpdateColision(particlesContainer, i);
		}
		//RIGHT WALL
		if (particlesContainer[i].pos.x >= 5 - radius) {
			normal = { -1,0,0 };
			d = 5;
			UpdateColision(particlesContainer, i);
		}
		//FRONT WALL
		if (particlesContainer[i].pos.z <= -5 + radius) {
			normal = { 0,0,1 };
			d = 5;
			UpdateColision(particlesContainer, i);
		}
		//BACK WALL
		if (particlesContainer[i].pos.z >= 5 - radius) {
			normal = { 0,0,-1 };
			d = 5;
			UpdateColision(particlesContainer, i);
		}
		//TOP WALL
		if (particlesContainer[i].pos.y >= 10 - radius) {
			normal = { 0,-1,0 };
			d = 10;
			UpdateColision(particlesContainer, i);
		}
		//SPHERE
		if (glm::pow((particlesContainer[i].pos.x - sphere->pos.x), 2) + glm::pow((particlesContainer[i].pos.y - sphere->pos.y), 2) + glm::pow((particlesContainer[i].pos.z - sphere->pos.z), 2) <= glm::pow((sphere->radius + radius), 2)) {
			normal = { particlesContainer[i].pos - sphere->pos };
			d = -(particlesContainer[i].pos.x*normal.x) - (particlesContainer[i].pos.y*normal.y) - (particlesContainer[i].pos.z*normal.z);

			//friction values
			vNormal = glm::dot(normal, particlesContainer[i].vel) * normal;
			vTangencial = particlesContainer[i].vel - vNormal;

			//elasticity and friction
			particlesContainer[i].pos = particlesContainer[i].pos - (1 + coefElasticity) * (glm::dot(normal, particlesContainer[i].pos) + d)*normal;
			particlesContainer[i].vel = particlesContainer[i].vel - (1 + coefElasticity) * (glm::dot(normal, particlesContainer[i].vel))* normal - coefFriction*vTangencial;

		}
	}




}



void PhysicsInit() {

	InitVerts();
}
void PhysicsUpdate(float dt) {

	UpdatePosition(particlesContainer);
	CheckColision(particlesContainer);

	if (frame % 100 == 0) {
		InitVerts();
	}

	for (int i = 0; i < 252; ++i) {
		//update partVerts vector with the new position
		partVerts[3 * i] = particlesContainer[i].pos.x;
		partVerts[3 * i + 1] = particlesContainer[i].pos.y;
		partVerts[3 * i + 2] = particlesContainer[i].pos.z;

	}

	frame++;

	Sphere::updateSphere(sphere->pos, sphere->radius);
	LilSpheres::updateParticles(0, LilSpheres::maxParticles, partVerts);
	ClothMesh::updateClothMesh(partVerts);
}
void PhysicsCleanup() {
	//TODO
}