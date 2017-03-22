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
float Ke, Kd;
glm::vec3 fTotal;


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

Particle *pC = new Particle[LilSpheres::maxParticles];
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
	Ke = 0.01;
	Kd = 0;

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
		pC[i].pos = glm::vec3(partVerts[i * 3], partVerts[i * 3 + 1], partVerts[i * 3 + 2]);
		pC[i].vel = glm::vec3(0, 0, 0); //random
	}

	/*particlesContainer2D = new Particle*[18];
	for (int i = 0; i < 18; i++) {
		particlesContainer2D[i] = new Particle[14];
	}

	int q = 0; 

	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {
			particlesContainer2D[i][j] = pC[q];
			q++;
		}
	}*/
}

int Srings(Particle *pC, int i) {
	glm::vec3 fIzquierda, fIzquierdaIzquierda, fDerecha, fDerechaDerecha, fArriba, fArribaArriba, fAbajo, fAbajoAbajo, fDiagonal1, fDiagonal2, fDiagonal3, fDiagonal4;
	
	//for (int i = 0; i < 252; i++) {
		
		if (i <= 13) {
			
			if (i == 1) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 12) {
				//TODO
				/*fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			//GRUPO 1
		/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
			fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
			//fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
			fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

			fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
			fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
			//fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
			fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

			//fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
			//fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
			fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
			fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

			fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
			return 0;*/
		}
		if (i >= 14 && i <= 27 ) {
			if (i == 14) {
				//TODO
			/*	//fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				//fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				//fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				//fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 27) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				//fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				//fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				//fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				//fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 15) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				//fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 26) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				//fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			//GRUPO 1
		/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
			fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
			fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
			fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

			fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
			fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
			//fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
			fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

			fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
			fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
			fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
			fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

			fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
			return 0;*/
		}
		if (i >= 224 && i <= 237) {
			if (i == 224) {
			/*	//TODO
				//fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				//fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				//fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 237) {
			/*	//TODO
				fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				//fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				//fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				//fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 225) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 236) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			//GRUPO 2
		/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
			fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
			fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
			fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

			fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
			fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
			fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
			//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

			fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
			fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
			fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
			fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

			fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
			return 0;*/
		}
		if (i >= 238) {
			if (i == 238) {
			/*	//TODO
				//fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				//fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				//fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				//fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				//fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 251) {
		/*		//TODO
				fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				//fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				//fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				//fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				//fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				//fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 239) {
			/*	//TODO
				fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				//fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				//fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				//fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i == 250) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				//fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				//fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				//fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				//fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}

			//GRUPO 2
			return 0;
		}
		if (i >= 28 && i <= 223) {
			if (i % 14 == 0) {
				//TODO
			/*	//fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				//fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				//fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i % 14 == 1) {
				//TODO
			/*	fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				//fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i % 14 == 12) {
		/*		//TODO
				fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			if (i % 14 == 13) {
			/*	//TODO
				fIzquierda = -(Ke*(glm::distance(pC[i].pos, pC[i - 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 1].vel), (pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos)))*(pC[i].pos - pC[i - 1].pos) / glm::distance(pC[i].pos, pC[i - 1].pos));
				//fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i + 1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 1].vel), (pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos)))*(pC[i].pos - pC[i + 1].pos) / glm::distance(pC[i].pos, pC[i + 1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i - 15].vel), (pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos)))*(pC[i].pos - pC[i - 15].pos) / glm::distance(pC[i].pos, pC[i - 15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i - 1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i - 1].vel - pC[i - 2].vel), (pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos)))*(pC[i - 1].pos - pC[i - 2].pos) / glm::distance(pC[i - 1].pos, pC[i - 2].pos));
				//fDerechaDerecha = -(Ke*(glm::distance(pC[i + 1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i - 15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i - 15].vel - pC[i - 30].vel), (pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos)))*(pC[i - 15].pos - pC[i - 30].pos) / glm::distance(pC[i - 15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i + 15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				//fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				//fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
				return 0;*/
			}
			else {
				//TODO
				fIzquierda = -(Ke*(glm::distance(pC[i].pos,pC[i-1].pos)-(2.8f)+Kd*glm::dot((pC[i].vel- pC[i-1].vel),(pC[i].pos- pC[i-1].pos)/glm::distance(pC[i].pos, pC[i-1].pos)))*(pC[i].pos - pC[i-1].pos) / glm::distance(pC[i].pos, pC[i-1].pos));
				fDerecha = -(Ke*(glm::distance(pC[i].pos, pC[i+1].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i+1].vel), (pC[i].pos - pC[i+1].pos) / glm::distance(pC[i].pos, pC[i+1].pos)))*(pC[i].pos - pC[i+1].pos) / glm::distance(pC[i].pos, pC[i+1].pos));
				fArriba = -(Ke*(glm::distance(pC[i].pos, pC[i - 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i -15].vel), (pC[i].pos - pC[i -15].pos) / glm::distance(pC[i].pos, pC[i-15].pos)))*(pC[i].pos - pC[i -15].pos) / glm::distance(pC[i].pos, pC[i -15].pos));
				fAbajo = -(Ke*(glm::distance(pC[i].pos, pC[i + 15].pos) - (2.8f) + Kd*glm::dot((pC[i].vel - pC[i + 15].vel), (pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos)))*(pC[i].pos - pC[i + 15].pos) / glm::distance(pC[i].pos, pC[i + 15].pos));

				fIzquierdaIzquierda = -(Ke*(glm::distance(pC[i-1].pos, pC[i - 2].pos) - (2.8f) + Kd*glm::dot((pC[i-1].vel - pC[i - 2].vel), (pC[i-1].pos - pC[i - 2].pos) / glm::distance(pC[i-1].pos, pC[i - 2].pos)))*(pC[i-1].pos - pC[i - 2].pos) / glm::distance(pC[i-1].pos, pC[i - 2].pos));
				fDerechaDerecha = -(Ke*(glm::distance(pC[i +1].pos, pC[i + 2].pos) - (2.8f) + Kd*glm::dot((pC[i + 1].vel - pC[i + 2].vel), (pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos)))*(pC[i + 1].pos - pC[i + 2].pos) / glm::distance(pC[i + 1].pos, pC[i + 2].pos));
				fArribaArriba = -(Ke*(glm::distance(pC[i-15].pos, pC[i - 30].pos) - (2.8f) + Kd*glm::dot((pC[i-15].vel - pC[i - 30].vel), (pC[i-15].pos - pC[i - 30].pos) / glm::distance(pC[i-15].pos, pC[i - 30].pos)))*(pC[i-15].pos - pC[i - 30].pos) / glm::distance(pC[i-15].pos, pC[i - 30].pos));
				fAbajoAbajo = -(Ke*(glm::distance(pC[i + 15].pos, pC[i + 30].pos) - (2.8f) + Kd*glm::dot((pC[i +15].vel - pC[i + 30].vel), (pC[i + 15].pos - pC[i + 30].pos) / glm::distance(pC[i + 15].pos, pC[i + 30].pos)))*(pC[i+ 15].pos - pC[i + 30].pos) / glm::distance(pC[i +15].pos, pC[i + 30].pos));

				fDiagonal1 = -(Ke*(glm::distance(pC[i].pos, pC[i - 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 16].vel), (pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos)))*(pC[i].pos - pC[i - 16].pos) / glm::distance(pC[i].pos, pC[i - 16].pos));
				fDiagonal2 = -(Ke*(glm::distance(pC[i].pos, pC[i - 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i - 14].vel), (pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos)))*(pC[i].pos - pC[i - 14].pos) / glm::distance(pC[i].pos, pC[i - 14].pos));
				fDiagonal3 = -(Ke*(glm::distance(pC[i].pos, pC[i + 16].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 16].vel), (pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos)))*(pC[i].pos - pC[i + 16].pos) / glm::distance(pC[i].pos, pC[i + 16].pos));
				fDiagonal4 = -(Ke*(glm::distance(pC[i].pos, pC[i + 14].pos) - (4.252f) + Kd*glm::dot((pC[i].vel - pC[i + 14].vel), (pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos)))*(pC[i].pos - pC[i + 14].pos) / glm::distance(pC[i].pos, pC[i + 14].pos));

				fTotal = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;

				std::cout << fIzquierda.x <<" " << fIzquierda.y << " " << fIzquierda.z << std::endl;

				return 0;
			}
		}
	//}
}

void UpdatePosition(Particle *pC) {
	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		if (i != 0 && i != 13) {

			Srings(pC, i);

			pC[i].lastVel = pC[i].vel;

			//update vector velocity velocity with formula
			pC[i].vel = pC[i].lastVel + fTotal * timePerFrame;

			//save last position 
			pC[i].lastPos = pC[i].pos;

			//update position with formula
			pC[i].pos = pC[i].lastPos + timePerFrame * pC[i].lastVel; //components x and z have 0 gravity.

		}
	}

}

void UpdateColision(Particle *pC, int i) {
	//friction values
	vNormal = glm::dot(normal, pC[i].vel) * normal;
	vTangencial = pC[i].vel - vNormal;

	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))*normal - coefFriction*vTangencial;
}

void CheckColision(Particle *pC) {
	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		//FLOOR
		if (pC[i].pos.y <= 0 + radius) {
			normal = { 0,1,0 };
			d = 0;
			UpdateColision(pC, i);
		}
		//LEFT WALL
		if (pC[i].pos.x <= -5 + radius) {
			normal = { 1,0,0 };
			d = 5;
			UpdateColision(pC, i);
		}
		//RIGHT WALL
		if (pC[i].pos.x >= 5 - radius) {
			normal = { -1,0,0 };
			d = 5;
			UpdateColision(pC, i);
		}
		//FRONT WALL
		if (pC[i].pos.z <= -5 + radius) {
			normal = { 0,0,1 };
			d = 5;
			UpdateColision(pC, i);
		}
		//BACK WALL
		if (pC[i].pos.z >= 5 - radius) {
			normal = { 0,0,-1 };
			d = 5;
			UpdateColision(pC, i);
		}
		//TOP WALL
		if (pC[i].pos.y >= 10 - radius) {
			normal = { 0,-1,0 };
			d = 10;
			UpdateColision(pC, i);
		}
		//SPHERE
		if (glm::pow((pC[i].pos.x - sphere->pos.x), 2) + glm::pow((pC[i].pos.y - sphere->pos.y), 2) + glm::pow((pC[i].pos.z - sphere->pos.z), 2) <= glm::pow((sphere->radius + radius), 2)) {
			normal = { pC[i].pos - sphere->pos };
			d = -(pC[i].pos.x*normal.x) - (pC[i].pos.y*normal.y) - (pC[i].pos.z*normal.z);

			//friction values
			vNormal = glm::dot(normal, pC[i].vel) * normal;
			vTangencial = pC[i].vel - vNormal;

			//elasticity and friction
			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*vTangencial;

		}
	}




}



void PhysicsInit() {

	InitVerts();
}
void PhysicsUpdate(float dt) {

	UpdatePosition(pC);
	CheckColision(pC);

	if (frame % 100 == 0) {
		InitVerts();
	}

	for (int i = 0; i < 252; ++i) {
		//update partVerts vector with the new position
		partVerts[3 * i] = pC[i].pos.x;
		partVerts[3 * i + 1] = pC[i].pos.y;
		partVerts[3 * i + 2] = pC[i].pos.z;

	}

	frame++;

	Sphere::updateSphere(sphere->pos, sphere->radius);
	LilSpheres::updateParticles(0, LilSpheres::maxParticles, partVerts);
	ClothMesh::updateClothMesh(partVerts);
}
void PhysicsCleanup() {
	//TODO
}