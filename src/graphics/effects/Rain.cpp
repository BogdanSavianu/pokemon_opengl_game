#include "Rain.hpp"
#include <cstdlib>
#include <glm/gtc/random.hpp>

Rain::Rain(int numParticles) : numParticles(numParticles), rainEnabled(false) {
    initialize();
}

Rain::~Rain() {
    glDeleteVertexArrays(1, &rainVAO);
    glDeleteBuffers(1, &rainVBO);
}

void Rain::initialize() {
    rainParticles.clear();
    rainParticles.reserve(numParticles);

    const float xRange = 400.0f;
    const float yRange = 200.0f;
    const float scale = 1.0f;

    for (int i = 0; i < numParticles; ++i) {
        RainParticle particle;

        float startingHeight = (rand() % (int)(yRange * 2));
        
        particle.position = glm::vec3(
            (rand() % (int)xRange - xRange/2) / scale,
            startingHeight,
            (rand() % (int)xRange - xRange/2) / scale
        );

        particle.velocity = glm::vec3(
            0.0f,
            -25.0f,
            0.0f
        );

        particle.lifetime = 10.0f + (startingHeight / 15.0f);
        particle.size = 2.0f + (rand() % 20) / 10.0f;
        rainParticles.push_back(particle);
    }

    setupBuffers();
}

void Rain::setupBuffers() {
    glGenVertexArrays(1, &rainVAO);
    glGenBuffers(1, &rainVBO);

    glBindVertexArray(rainVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rainVBO);
    glBufferData(GL_ARRAY_BUFFER, rainParticles.size() * (sizeof(glm::vec3) + sizeof(float)), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(float), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Rain::update(float deltaTime, const glm::vec3& windDirection, float windStrength) {
    if (!rainEnabled) return;

    const float windFactor = windStrength * 0.2f;
    const float velocityDamping = 0.95f;
    static bool wasWindEnabled = false;
    bool isWindEnabled = windStrength > 0.0f;
    
    if (wasWindEnabled && !isWindEnabled) {
        for (auto& particle : rainParticles) {
            particle.velocity = glm::vec3(0.0f, -25.0f, 0.0f);
        }
    }
    wasWindEnabled = isWindEnabled;
    
    for (auto& particle : rainParticles) {
        if (isWindEnabled) {
            particle.velocity.x = particle.velocity.x * velocityDamping + windDirection.x * windFactor;
            particle.velocity.z = particle.velocity.z * velocityDamping + windDirection.z * windFactor;
        } else {
            particle.velocity.x = 0.0f;
            particle.velocity.z = 0.0f;
            particle.velocity.y = -25.0f;
        }
        
        particle.position += particle.velocity * deltaTime;
        particle.lifetime -= deltaTime;

        if (particle.lifetime <= 0.0f || particle.position.y < -20.0f) {
            float newHeight = (rand() % 200) + 50.0f;
            
            particle.position = glm::vec3(
                (rand() % 400 - 200),
                newHeight,
                (rand() % 400 - 200)
            );
            
            if (isWindEnabled) {
                particle.velocity = glm::vec3(
                    windDirection.x * windStrength * 0.1f,
                    -25.0f,
                    windDirection.z * windStrength * 0.1f
                );
            } else {
                particle.velocity = glm::vec3(0.0f, -25.0f, 0.0f);
            }
            
            particle.lifetime = 20.0f + (newHeight / 15.0f);
        }
    }
}

void Rain::updateBuffer() {
    static std::vector<float> data;
    data.clear();
    data.reserve(rainParticles.size() * 4);

    for (const auto& particle : rainParticles) {
        data.push_back(particle.position.x);
        data.push_back(particle.position.y);
        data.push_back(particle.position.z);
        data.push_back(particle.size);
    }

    glBindBuffer(GL_ARRAY_BUFFER, rainVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
}

void Rain::render() {
    glBindVertexArray(rainVAO);
    glDrawArrays(GL_POINTS, 0, rainParticles.size());
} 