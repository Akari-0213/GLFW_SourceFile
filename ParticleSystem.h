#pragma once
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
using namespace std;

struct Particle {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	float life;
	float size;
	float rotation;

	float mass;
	float initial_time;
	glm::vec3 initial_velocity;
	float theta;

	Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f), size(0.1f),
		         mass(0.1f), initial_time(0.0f), initial_velocity(0.0f), theta(0.0f){}
};


class ParticleSystem {
private:
	vector<Particle> particles;
	GLuint texture;
	int maxParticles;
	int lastUsedParticle;
	int alive_particle_count;

	glm::vec3 emitterPosition;
	glm::mat4 emitterRotation;
	glm::vec3 emitterDirection;
	float emitterRate;
	float particle_lifetime;

	float air_resistance;
	float gravity;

	mt19937 rng;
	uniform_real_distribution<float> dist;

	GLuint VAO, VBO_quadvertex, VBO_positions, VBO_colors, VBO_sizes, VBO_rotations, IBO;
	vector<glm::vec3> positionData;
	vector<glm::vec4> colorData;
	vector<float> sizeData;
	vector<float> rotationData;

public:
	ParticleSystem(int maxParticle = 1000);
	~ParticleSystem();

	void Initialize();
	void Update(float deltatime);
	void Render();

	void setEmitterPosition(const glm::vec3& pos) { emitterPosition = pos; }
	void setEmitterRotation(const glm::mat4& rot) { emitterRotation = rot; }
	void setEmitterDirection(const glm::vec3& dir) { emitterDirection = glm::normalize(dir); }
	void setEmitterRate(float rate) { emitterRate = rate; }
	void setParticleLifeTime(float lifetime) { particle_lifetime = lifetime; }
	void setAirResistance(float air) { air_resistance = air; }
	void setGravity(float g) { gravity = g; }
	void setTexture(GLuint tex) { texture = tex; }
	GLuint getTexture() const { return texture; }

private:
	int FindUnusedParticle();
	void RespawnParticle(Particle& particle);
};
