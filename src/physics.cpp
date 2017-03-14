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
	float life;
};

Particle *particlesContainer = new Particle[LilSpheres::maxParticles];

namespace ClothMesh {
	extern void setupClothMesh();
	extern void cleanupClothMesh();
	extern void updateClothMesh(float* array_data);
	extern void drawClothMesh();
}

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void InitVerts() {

	partVerts = new float[LilSpheres::maxParticles * 3];
	float dist = -2.8f;
	float zDist = 0.f;
	for (int i =0 ; i < 252; ++i) {
		if (i % 14 == 0) {
			zDist += 0.4f;
			dist = -2.8f;
		}
		partVerts[i * 3 + 0] = dist;
		partVerts[i * 3 + 1] = 5;
		partVerts[i * 3 + 2] = zDist;
		dist += 0.4f;
	}
}

void PhysicsInit() {
	//TODO

	InitVerts();
	//ClothMesh::setupClothMesh();
}
void PhysicsUpdate(float dt) {
	//TODO

	LilSpheres::updateParticles(0, LilSpheres::maxParticles, partVerts);
	ClothMesh::updateClothMesh(partVerts);
}
void PhysicsCleanup() {
	//TODO
}

