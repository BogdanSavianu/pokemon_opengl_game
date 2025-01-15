#ifndef RAIN_HPP
#define RAIN_HPP

#include <vector>
#include <glm/glm.hpp>
#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
#else
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

class Rain {
public:
    struct RainParticle {
        glm::vec3 position;
        glm::vec3 velocity;
        float lifetime;
        float size;
    };

    Rain(int numParticles = 100000);
    ~Rain();

    void initialize();
    void setupBuffers();
    void update(float deltaTime, const glm::vec3& windDirection, float windStrength);
    void updateBuffer();
    void render();

    bool isEnabled() const { return rainEnabled; }
    void toggleEnabled() { rainEnabled = !rainEnabled; }

    std::vector<RainParticle>& getParticles() { return rainParticles; }

private:
    std::vector<RainParticle> rainParticles;
    int numParticles;
    GLuint rainVAO, rainVBO;
    bool rainEnabled;
    GLuint instanceVBO;
};

#endif 