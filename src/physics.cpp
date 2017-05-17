#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include "GL_framework.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <GLFW\glfw3.h>


#define PI 3.1416f

bool show_test_window = false;

float *partVerts;
float timePerFrame = 0.003;
float radius = 0.05f;
glm::vec3 gravity = { 0, -9.8f, 0 };


glm::vec3 vNormal, vTangencial;
float coefFriction = 0.f;
float coefElasticity = 0.5f;
int frame =0;
int Ke = 1000, Kd = 50;


//AGUITA
float k = 2 * PI / 0.5f;
glm::vec3 K = glm::vec3(0.0f,0.0f,1.0f);
glm::vec3 K2 = glm::vec3(1.0f, 0.0f, 0.0f);
float A = 0.5f;
float w = 3.0f;
//


glm::vec3 fTotal[252];


namespace LilSpheres {
	extern const int maxParticles;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}

struct Particle {
	glm::vec3 pos;
	glm::vec3 initPos;
	glm::vec3 vel;
	glm::vec3 lastVel;
};

Particle *pC = new Particle[LilSpheres::maxParticles];

namespace Sphere {
	extern glm::vec3 centro = { 0.f, 1.f, 0.f };
	extern void setupSphere(glm::vec3 pos = centro, float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();

}

struct laSphere {
	glm::vec3 pos;
	glm::vec3 lastPos;
	glm::vec3 vel;
	glm::vec3 lastVel;
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
		
		ImGui::SliderFloat("Direccion OnaA X", &K.x, -10.0f, 10.0f);
		ImGui::SliderFloat("Direccion OnaA Z", &K.z, -10.0f, 10.0f);

		ImGui::SliderFloat("Direccion OnaB X", &K2.x, -10.0f, 10.0f);
		ImGui::SliderFloat("Direccion OnaB Z", &K2.z, -10.0f, 10.0f);

		//GUI Waterfall
		ImGui::SliderFloat("Amplitud", &A, 0, 1);
		ImGui::SliderFloat("W", &w, 0, 10);
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void RandPosSphere() {
	srand(time(NULL));
	sphere->pos = { rand()%8-4, 6, rand() % 8 - 4 };
	sphere->radius = 1.2f;
	sphere->lastPos = glm::vec3(sphere->pos);
	sphere->vel = glm::vec3(0.0f, 0.0f, 0.0f);
	sphere->lastVel = sphere->vel;
}

void InitVerts() {

	partVerts = new float[LilSpheres::maxParticles * 3];
	RandPosSphere();
	float xDist = -2.8f;
	float zDist = -3.2f;
	
	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		if (i % 14 == 0) {
			zDist += 0.4f;
			xDist = -2.8f;
		}
		partVerts[i * 3 + 0] = xDist;
		partVerts[i * 3 + 1] = 6;
		partVerts[i * 3 + 2] = zDist;
		xDist += 0.4f;
	}

	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		pC[i].pos = glm::vec3(partVerts[i * 3], partVerts[i * 3 + 1], partVerts[i * 3 + 2]);
		pC[i].initPos = pC[i].pos;
		pC[i].vel = glm::vec3(0, 0, 0); 
	}
}

void UpdatePosition(Particle *pC, int i) {
	
			pC[i].pos.x = (pC[i].initPos.x - (K.x / k)*A*sin(glm::dot(K.x,pC[i].initPos.x) - w*(GLfloat)glfwGetTime()))+ (pC[i].initPos.x - (K2.x / k)*A*sin(glm::dot(K2.x, pC[i].initPos.x) - w*(GLfloat)glfwGetTime()));
			pC[i].pos.z = (pC[i].initPos.z - (K.z / k)*A*sin(glm::dot(K.z, pC[i].initPos.z) - w*(GLfloat)glfwGetTime()))+ (pC[i].initPos.z - (K2.z / k)*A*sin(glm::dot(K2.z, pC[i].initPos.z) - w*(GLfloat)glfwGetTime()));
			pC[i].pos.y = (A*(cos(glm::dot(K.z, pC[i].initPos.z) - w*(GLfloat)glfwGetTime()))+2)+ (A*(cos(glm::dot(K2.x, pC[i].initPos.x) - w*(GLfloat)glfwGetTime())) + 2);
}

void UpdateColision(Particle *pC, int i, int d, glm::vec3 normal) {
	
	//friction values


	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))*normal;

	vNormal = glm::dot(normal, pC[i].vel) * normal;
	vTangencial = pC[i].vel - vNormal;

	pC[i].vel = pC[i].vel - coefFriction*vTangencial;
}



void PhysicsInit() {

	InitVerts();
} 

void Flotability(Particle *pC, float dt) {

	float distance[4] = { 50.0f,50.0f,50.0f,50.0f };

	int index[4];

	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		
		float temp = glm::distance(glm::vec2(sphere->pos.x, sphere->pos.z), glm::vec2(pC[i].pos.x, pC[i].pos.z));

		if (temp < distance[0]) {

			distance[3] = distance[2];
			index[3] = index[2];
			distance[2] = distance[1];
			index[2] = index[1];
			distance[1] = distance[0];
			index[1] = index[0];
			distance[0] = temp;
			index[0] = i;
		}
	}

	float AVG = (pC[index[0]].pos.y + pC[index[1]].pos.y + pC[index[2]].pos.y + pC[index[3]].pos.y) / 4;
	
	if (sphere->pos.y-sphere->radius < AVG) {
		float h = glm::abs(sphere->pos.y - AVG);


		//float V_sub = (1.f / 3.f)*PI*glm::pow(h, 2)*(3 * sphere->radius - h);

		float V_sub = glm::pow(sphere->radius * 2, 2)*h;
		if (V_sub > 13.82) {
			V_sub = 13.82;
		}

		glm::vec3 F_buoyancy = 5.f * -gravity* V_sub * glm::vec3(0, 1, 0);
		std::cout << V_sub << std::endl;
		sphere->lastVel = sphere->vel;
		sphere->vel = sphere->lastVel + ((F_buoyancy+gravity)/5.f) * dt;
		sphere->lastPos = sphere->pos;
		sphere->pos = sphere->lastPos + sphere->vel*dt;
	}
	else if(sphere->pos.y - sphere->radius > AVG){
		sphere->lastVel = sphere->vel;
		sphere->vel = sphere->lastVel + (gravity/5.f) * dt;
		//std::cout << sphere->vel.y << std::endl;
		sphere->lastPos = sphere->pos;
		sphere->pos = sphere->lastPos + sphere->vel*dt;
	}




}

void PhysicsUpdate(float dt) {

	Flotability(pC,dt);

		for (int i = 0; i < LilSpheres::maxParticles; i++) {

			UpdatePosition(pC, i);
			

			if (frame % 800 == 0) {
				InitVerts();
			}

			partVerts[3 * i] = pC[i].pos.x;
			partVerts[3 * i + 1] = pC[i].pos.y;
			partVerts[3 * i + 2] = pC[i].pos.z;

		}

		Sphere::updateSphere(sphere->pos, sphere->radius);
		LilSpheres::updateParticles(0, LilSpheres::maxParticles, partVerts);
		ClothMesh::updateClothMesh(partVerts);

		frame++;
	
	
	
}
void PhysicsCleanup() {
	delete[] pC;
	delete[] partVerts;
}