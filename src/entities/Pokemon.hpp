#ifndef Pokemon_hpp
#define Pokemon_hpp

#include "../graphics/models/Model3D.hpp"
#include <glm/glm.hpp>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../audio/AudioManager.hpp"

class Pokemon {
public:
    Pokemon(const std::string& modelPath, const glm::vec3& startPos, float scale);
    void update(float deltaTime);
    void draw(gps::Shader& shader);
    
    void setCircularFlight(float radius, float height, float speed);
    void setFigureEightFlight(float radius, float height, float speed);
    
    void spin(float angle);  
    void jump(float deltaTime);  
    void stopJumping();  
    
    glm::vec3 position;
    float scale;
    float angleY;
    
    bool isSpinning() const { return isJumping; }  
    
private:
    gps::Model3D model;
    bool isFlying;
    float flightRadius;
    float flightHeight;
    float flightSpeed;
    int flightPattern;  // 0 = static, 1 = circular, 2 = figure-8
    glm::vec3 initialPosition;
    float currentTime;  // for rotation
    float spinAngle;
    float jumpHeight;
    float jumpTime;
    bool isJumping;
    const float JUMP_DURATION = 0.5f;
    float MAX_JUMP_HEIGHT;
    static ALuint spinSoundBuffer;
    static bool spinSoundLoaded;
};

#endif 