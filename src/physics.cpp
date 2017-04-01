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
float timePerFrame = 0.003;
float radius = 0.05f;
glm::vec3 gravity = { 0, -9.8, 0 };

glm::vec3 normal = { 0,0,0 };
float d;
glm::vec3 vNormal, vTangencial;
float coefFriction = 0.f;
float coefElasticity = 0.f;
int frame =0;
int Ke = 1000, Kd = 50;
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
	glm::vec3 lastPos;
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
		ImGui::InputInt("Ke", &Ke, 10, 100, 0);
		ImGui::InputInt("Kd", &Kd, 10, 100, 0);
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void RandPosSphere() {
	srand(time(NULL));
	//sphere->pos = { rand()%8-4, 2, rand() % 8 - 4 };
	sphere->pos = { 0, 4, 0};
	sphere->radius = 1.5f;
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
		partVerts[i * 3 + 1] = 7;
		partVerts[i * 3 + 2] = zDist;
		xDist += 0.4f;
	}

	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		pC[i].pos = glm::vec3(partVerts[i * 3], partVerts[i * 3 + 1], partVerts[i * 3 + 2]);
		pC[i].vel = glm::vec3(0, 0, 0); //random
	}
}

void Srings(Particle *pC, int i) {
	glm::vec3 fIzquierda, fIzquierdaIzquierda, fDerecha, fDerechaDerecha, fArriba, fArribaArriba, fAbajo, fAbajoAbajo, fDiagonal1, fDiagonal2, fDiagonal3, fDiagonal4;
	
	bool b_fIzquierda = false, b_fIzquierdaIzquierda = false, b_fDerecha = false, b_fDerechaDerecha = false, b_fArriba = false, b_fArribaArriba = false, b_fAbajo = false,
		 b_fAbajoAbajo = false, b_fDiagonal1 = false , b_fDiagonal2 = false, b_fDiagonal3 = false, b_fDiagonal4 = false;
	
	//asignamos grupo a la particula
	
		if (i <= 13) {
			if (i == 0 || i == 13) {
				b_fIzquierda = false; b_fIzquierdaIzquierda = false; b_fDerecha = false; b_fDerechaDerecha = false;
				b_fArriba = false; b_fArribaArriba = false; b_fAbajo = false; b_fAbajoAbajo = false;
				b_fDiagonal1 = false; b_fDiagonal2 = false; b_fDiagonal3 = false; b_fDiagonal4 = false;
			}
			else if (i == 1) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = false; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = false; b_fDiagonal2 = false; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if (i == 12) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = false;
				b_fArriba = false; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = false; b_fDiagonal2 = false; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if(i != 0 && i != 13 && i !=1 && i!=12){
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = false; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = false; b_fDiagonal2 = false; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
		}

		else if (i >= 14 && i <= 27 ) {
			if (i == 14) {
				//TODO
				b_fIzquierda = false; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = false; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = false;
			}
			else if (i == 27) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = false; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = false; b_fDiagonal3 = false; b_fDiagonal4 = true;
			}
			else if (i == 15) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if (i == 26) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if (i != 14 && i != 27 && i != 15 && i != 26) {
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = false; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
		}

		else if (i >= 224 && i <= 237) {
			if (i == 224) {
			//TODO
				b_fIzquierda = false; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = false;
				b_fDiagonal1 = false; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = false;
			}
			else if (i == 237) {
			//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = false; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = false; b_fDiagonal3 = false; b_fDiagonal4 = true;
			}
			else if (i == 225) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if (i == 236) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if (i != 224 && i != 237 && i != 225 && i != 236) {
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
		}

		else if (i >= 238 && i<=251) {
			if (i == 238) {
				//TODO
				b_fIzquierda = false; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = false; b_fAbajoAbajo = false;
				b_fDiagonal1 = false; b_fDiagonal2 = true; b_fDiagonal3 = false; b_fDiagonal4 = false;
			}
			else if (i == 251) {
			//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = false; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = false; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = false; b_fDiagonal3 = false; b_fDiagonal4 = false;
			}
			else if (i == 239) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = false; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = false; b_fDiagonal4 = false;
			}
			else if (i == 250) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = false; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = false; b_fDiagonal4 = false;
			}
			else if (i != 238 && i != 251 && i != 239 && i != 250) {
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = false; b_fAbajoAbajo = false;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = false; b_fDiagonal4 = false;
			}
		}

		else if (i >= 28 && i <= 223) {
			if (i % 14 == 0) {
				//TODO
				b_fIzquierda = false; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = false; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = false;
			}
			else if (i % 14 == 1) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = false; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if (i % 14 == 12) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
			else if (i % 14 == 13) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = false; b_fDerechaDerecha = false;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = false; b_fDiagonal3 = false; b_fDiagonal4 = true;
			}
			else if (i %14 != 0 && i % 14 != 1 && i % 14 != 12 && i % 14 != 13) {
				//TODO
				b_fIzquierda = true; b_fIzquierdaIzquierda = true; b_fDerecha = true; b_fDerechaDerecha = true;
				b_fArriba = true; b_fArribaArriba = true; b_fAbajo = true; b_fAbajoAbajo = true;
				b_fDiagonal1 = true; b_fDiagonal2 = true; b_fDiagonal3 = true; b_fDiagonal4 = true;
			}
		}

		//aplicar f�rmula si es true
		if (b_fIzquierda) {
			float actualDistance = glm::distance(pC[i].pos, pC[i - 1].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i - 1].pos) / actualDistance;

			fIzquierda = -(Ke* (actualDistance - (0.4f)) + Kd * glm::dot((pC[i].vel - pC[i - 1].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		} 
		if (b_fDerecha) {
			float actualDistance = glm::distance(pC[i].pos, pC[i + 1].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i + 1].pos) / actualDistance;

			fDerecha = -(Ke* (actualDistance - (0.4f)) + Kd * glm::dot((pC[i].vel - pC[i + 1].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fArriba) {
			float actualDistance = glm::distance(pC[i].pos, pC[i -14].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i -14].pos) / actualDistance;

			fArriba = -(Ke* (actualDistance - (0.4f)) + Kd * glm::dot((pC[i].vel - pC[i - 14].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fAbajo) {
			float actualDistance = glm::distance(pC[i].pos, pC[i + 14].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i + 14].pos) / actualDistance;

			fAbajo = -(Ke* (actualDistance - (0.4f)) + Kd * glm::dot((pC[i].vel - pC[i + 14].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}

		if (b_fIzquierdaIzquierda) {
			float actualDistance = glm::distance(pC[i].pos, pC[i - 2].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i - 2].pos) / actualDistance;

			fIzquierdaIzquierda =  -(Ke* (actualDistance - (0.8f)) + Kd * glm::dot((pC[i].vel - pC[i - 2].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fDerechaDerecha) {
			float actualDistance = glm::distance(pC[i].pos, pC[i + 2].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i + 2].pos) / actualDistance;

			fDerechaDerecha = -(Ke* (actualDistance - (0.8f)) + Kd * glm::dot((pC[i].vel - pC[i + 2].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fArribaArriba) {
			float actualDistance = glm::distance(pC[i].pos, pC[i - 28].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i - 28].pos) / actualDistance;

			fArribaArriba = -(Ke* (actualDistance - (0.8f)) + Kd * glm::dot((pC[i].vel - pC[i - 28].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fAbajoAbajo) {
			float actualDistance = glm::distance(pC[i].pos, pC[i + 28].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i + 28].pos) / actualDistance;

			fAbajoAbajo = -(Ke* (actualDistance - (0.8f)) + Kd * glm::dot((pC[i].vel - pC[i + 28].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		
		if (b_fDiagonal1) {
			float actualDistance = glm::distance(pC[i].pos, pC[i - 15].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i - 15].pos) / actualDistance;

			fDiagonal1 = -(Ke* (actualDistance - (0.56f)) + Kd * glm::dot((pC[i].vel - pC[i - 15].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fDiagonal2) {
			float actualDistance = glm::distance(pC[i].pos, pC[i - 13].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i - 13].pos) / actualDistance;

			fDiagonal2 = -(Ke* (actualDistance - (0.56f)) + Kd * glm::dot((pC[i].vel - pC[i - 13].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fDiagonal3) {
			float actualDistance = glm::distance(pC[i].pos, pC[i + 15].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i + 15].pos) / actualDistance;

			fDiagonal3 = -(Ke* (actualDistance - (0.56f)) + Kd * glm::dot((pC[i].vel - pC[i + 15].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		if (b_fDiagonal4) {
			float actualDistance = glm::distance(pC[i].pos, pC[i + 13].pos);
			glm::vec3 actualDistanceNormalized = (pC[i].pos - pC[i + 13].pos) / actualDistance;

			fDiagonal4 = -(Ke* (actualDistance - (0.56f)) + Kd * glm::dot((pC[i].vel - pC[i + 13].vel), actualDistanceNormalized)) *(actualDistanceNormalized);
		}
		
		//suma total de fuerzas
		fTotal[i] = fIzquierda + fIzquierdaIzquierda + fDerecha + fDerechaDerecha + fArriba + fArribaArriba + fAbajo + fAbajoAbajo + fDiagonal1 + fDiagonal2 + fDiagonal3 + fDiagonal4 + gravity;
		
		//std::cout << "INDICE: " << i << std::endl;
		//std::cout << fTotal[i].x << " " << fTotal[i].y << " " << fTotal[i].z << std::endl;
		
}

void UpdatePosition(Particle *pC, int i) {
		
	if (i != 0 && i != 13) {

			pC[i].lastVel = pC[i].vel;

			//update vector velocity velocity with formula
			pC[i].vel = pC[i].lastVel + fTotal[i] * timePerFrame;

			//save last position 
			pC[i].lastPos = pC[i].pos;

			//update position with formula
			pC[i].pos = pC[i].lastPos + timePerFrame * pC[i].lastVel; //components x and z have 0 gravity.

		}
}


void UpdateColision(Particle *pC, int i) {
	
	//friction values
	vNormal = glm::dot(normal, pC[i].vel) * normal;
	vTangencial = pC[i].vel - vNormal;

	pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
	pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))*normal - coefFriction*vTangencial;
}

void CheckColision(Particle *pC, int i) {
		//FLOOR
		if (pC[i].pos.y <= 0 + radius) {
			normal = { 0,1,0 };
			d = 0;
			UpdateColision(pC, i);
		}
		//LEFT WALL
		else if (pC[i].pos.x <= -5 + radius) {
			normal = { 1,0,0 };
			d = 5;
			UpdateColision(pC, i);
		}
		//RIGHT WALL
		else if (pC[i].pos.x >= 5 - radius) {
			normal = { -1,0,0 };
			d = 5;
			UpdateColision(pC, i);
		}
		//FRONT WALL
		else if (pC[i].pos.z <= -5 + radius) {
			normal = { 0,0,1 };
			d = 5;
			UpdateColision(pC, i);
		}
		//BACK WALL
		else if (pC[i].pos.z >= 5 - radius) {
			normal = { 0,0,-1 };
			d = 5;
			UpdateColision(pC, i);
		}
		//TOP WALL
		else if (pC[i].pos.y >= 10 - radius) {
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


void PhysicsInit() {

	InitVerts();
}

void PhysicsUpdate(float dt) {
	
	for (int i = 0; i < LilSpheres::maxParticles; i++) {
		Srings(pC, i);
	}

	for (int i = 0; i < LilSpheres::maxParticles; i++) {

		UpdatePosition(pC,i);
		CheckColision(pC,i);

		if (frame % 400 == 0) {
			InitVerts();
		}

		//update partVerts vector with the new position
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
	//TODO
}