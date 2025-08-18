#include "ParticleSystem.h"
#include <algorithm>
#include <chrono>
#include <GLFW/glfw3.h>


ParticleSystem::ParticleSystem(int maxParticles)
	: maxParticles(maxParticles), lastUsedParticle(0), emitterPosition(0.0f), emitterRotation(1.0f), emitterDirection(0.0f, 1.0f, 0.0f),
	emitterRate(10.0f), particle_lifetime(3.0f), rng(chrono::steady_clock::now().time_since_epoch().count()),
	dist(-1.0f, 1.0f), air_resistance(0.5f), gravity(9.8f), texture(0)
{
	particles.resize(maxParticles);
	positionData.resize(maxParticles);
	colorData.resize(maxParticles);
	sizeData.resize(maxParticles);
    rotationData.resize(maxParticles);
}


ParticleSystem::~ParticleSystem() {
    glDeleteBuffers(1, &VBO_quadvertex);
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_colors);
	glDeleteBuffers(1, &VBO_sizes);
    glDeleteBuffers(1, &VBO_rotations);
    glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VAO);
}


void ParticleSystem::Initialize() {

	//VAO作成
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


    float quad_vertices[]{
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f, 0.5f,
        -0.5f, 0.5f,
    };
    
    unsigned int quad_indices[]{
        0, 1, 2,
        2, 3, 0
    };

	//VBO作成
    glGenBuffers(1, &VBO_quadvertex);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_quadvertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //VBO(インスタンスごと)
	glGenBuffers(1, &VBO_positions);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

	glGenBuffers(1, &VBO_colors);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

	glGenBuffers(1, &VBO_sizes);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_sizes);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &VBO_rotations);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_rotations);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    //IBO作成
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);


	glBindVertexArray(0);
}


int ParticleSystem::FindUnusedParticle() {
	for (int i = lastUsedParticle; i < maxParticles; i++) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < maxParticles; i++) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	lastUsedParticle = 0;
	return 0;
}


void ParticleSystem::RespawnParticle(Particle& particle) {
	particle.position = emitterPosition + glm::vec3(dist(rng) * 0.5f, 0.0f, dist(rng) * 0.5);
    particle.theta = (15.0f + dist(rng) * 15.0f) * 3.1415f / 180.0f;
    float initial_speed = 7.0f;
	float spread = 0.7f;
    particle.initial_velocity = glm::vec3(dist(rng) * spread,
                                          sin(particle.theta) * initial_speed,
                                          cos(particle.theta) * initial_speed + dist(rng) * spread);

    particle.velocity = particle.initial_velocity;
	particle.color = glm::vec4(0.2f + dist(rng) * 0.1f, 0.6f + dist(rng) * 0.3f, 0.1f + dist(rng) * 0.1f, 1.0f);
	particle.life = 1.0f;
	particle.size = 0.4f * dist(rng) + 0.5f;
    particle.rotation = dist(rng) * 3.1415f * 2.0f;
    particle.mass = 0.3f;
    particle.initial_time = static_cast<float>(glfwGetTime());
}


void ParticleSystem::Update(float deltaTime) {
    // 新しいパーティクルを生成
    static float particles_create = 0.0f;
    particles_create += emitterRate * deltaTime;
    int newParticles = static_cast<int>(floor(particles_create));
    particles_create -= newParticles;

    for (int i = 0; i < newParticles; i++) {
        int unused_particle = FindUnusedParticle();
        RespawnParticle(particles[unused_particle]);
    }

    // 既存のパーティクルを更新
    alive_particle_count = 0;
    float current_time = static_cast<float>(glfwGetTime());
    for (int i = 0; i < maxParticles; i++) {
        Particle& p = particles[i];

        if (p.life > 0.0f) {
            // ライフタイムを減少
            p.life -= deltaTime / particle_lifetime;

            if (p.life > 0.0f) {
                float t = current_time - p.initial_time;

                float k = air_resistance;
                float m = p.mass;
                float g = gravity;
                float v0_x = p.initial_velocity.x;
                float v0_y = p.initial_velocity.y;
                float v0_z = p.initial_velocity.z;

                // 減衰項
                float exp_term = exp(-k * t / m);
                p.position.x = emitterPosition.x + (m * v0_x / k) * (1.0f - exp_term);
     
                p.position.y = emitterPosition.y - (m * g / k) * t +
                    (m / k) * (v0_y + (m * g / k)) * (1.0f - exp_term);
                p.position.z = emitterPosition.z + (m * v0_z / k) * (1.0f - exp_term);

                // 速度の更新
                p.velocity.x = v0_x * exp_term;
                p.velocity.y = -m * g / k + (v0_y + m * g / k) * exp_term;
                p.velocity.z = v0_z * exp_term;

                // 回転の更新
                p.rotation += (1.0f + abs(p.velocity.y)) * deltaTime;
                //cout <<"x="<< p.position.x << "y="<<p.position.y <<"z="<< p.position.z << endl;

                // アルファ値をライフタイムに応じて変更
                p.color.w = p.life;

                // データをバッファ用配列にコピー
                positionData[alive_particle_count] = p.position;
                colorData[alive_particle_count] = p.color;
                sizeData[alive_particle_count] = p.size * p.life;
                rotationData[alive_particle_count] = p.rotation;

                alive_particle_count++;
            }
        }
    }

    // GPUバッファを更新
    if (alive_particle_count > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, alive_particle_count * sizeof(glm::vec3), positionData.data());

        glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
        glBufferSubData(GL_ARRAY_BUFFER, 0, alive_particle_count * sizeof(glm::vec4), colorData.data());

        glBindBuffer(GL_ARRAY_BUFFER, VBO_sizes);
        glBufferSubData(GL_ARRAY_BUFFER, 0, alive_particle_count * sizeof(float), sizeData.data());

        glBindBuffer(GL_ARRAY_BUFFER, VBO_rotations);
        glBufferSubData(GL_ARRAY_BUFFER, 0, alive_particle_count * sizeof(float), rotationData.data());
    }
}

void ParticleSystem::Render() {
    if (alive_particle_count == 0) return;
    //アルファブレンディング有効
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // デプステストは有効
    glDepthMask(GL_FALSE);

    // VAOをバインドして描画
    glBindVertexArray(VAO);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, alive_particle_count);
    glBindVertexArray(0);

    // 状態を元に戻す
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

