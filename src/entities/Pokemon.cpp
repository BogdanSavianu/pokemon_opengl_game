#include "Pokemon.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

ALuint Pokemon::spinSoundBuffer = 0;
bool Pokemon::spinSoundLoaded = false;

Pokemon::Pokemon(const std::string& modelPath, const glm::vec3& startPos, float scale) 
    : position(startPos), initialPosition(startPos), scale(scale), angleY(0.0f), 
      isFlying(false), flightRadius(0.0f), flightHeight(0.0f), flightSpeed(0.0f), 
      flightPattern(0), currentTime(0.0f), spinAngle(0.0f), jumpHeight(0.0f),
      jumpTime(0.0f), isJumping(false) {
    model.LoadModel(modelPath);
    
    if (modelPath.find("pikachu") != std::string::npos) {
        MAX_JUMP_HEIGHT = 0.5f;
    } else {
        MAX_JUMP_HEIGHT = 3.0f; 
    }
}

void Pokemon::setCircularFlight(float radius, float height, float speed) {
    flightPattern = 1;
    flightRadius = radius;
    flightHeight = height;
    flightSpeed = speed;
    isFlying = true;
    position = initialPosition; 
}

void Pokemon::setFigureEightFlight(float radius, float height, float speed) {
    flightPattern = 2;
    flightRadius = radius;
    flightHeight = height;
    flightSpeed = speed;
    isFlying = true;
    position = initialPosition; 
}

void Pokemon::spin(float angle) {
    if (!isFlying) { 
        spinAngle += angle;
        if (!isJumping) {
            isJumping = true;
            jumpTime = 0.0f;
        }
    }
}

void Pokemon::jump(float deltaTime) {
    if (!isFlying && isJumping) {
        jumpTime += deltaTime;
        
        if (jumpTime >= JUMP_DURATION) {
            jumpTime = 0.0f;
        }
        
        // sinusoidal jump motion
        float jumpProgress = (jumpTime / JUMP_DURATION) * 2.0f * M_PI;
        jumpHeight = MAX_JUMP_HEIGHT * sin(jumpProgress);
        position.y = initialPosition.y + jumpHeight;
    }
}

void Pokemon::stopJumping() {
    isJumping = false;
    jumpHeight = 0.0f;
    position.y = initialPosition.y;
}

void Pokemon::update(float deltaTime) {
    currentTime += deltaTime;
    
    if (isFlying) {
        switch (flightPattern) {
            case 1: // Circular
                position.x = initialPosition.x + flightRadius * cos(currentTime * flightSpeed);
                position.y = initialPosition.y + flightHeight * sin(currentTime * 0.2f);
                position.z = initialPosition.z + flightRadius * sin(currentTime * flightSpeed);
                break;
                
            case 2: // Figure-8
                position.x = initialPosition.x + flightRadius * cos(currentTime * flightSpeed);
                position.y = initialPosition.y + flightHeight * sin(currentTime * 0.1f);
                position.z = initialPosition.z + flightRadius * sin(currentTime * flightSpeed * 2.0f) * 0.5f;
                break;
        }
    } else {
        jump(deltaTime);
    }
}

void Pokemon::draw(gps::Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(scale));
    model = glm::translate(model, position);
    
    if (isFlying) {
        float angle;
        
        switch (flightPattern) {
            case 1: // Circular
                angle = -currentTime * flightSpeed;
                break;
                
            case 2: // Figure-8
                angle = -currentTime * flightSpeed;
                break;
                
            default:
                angle = 0.0f;
        }
        
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    } else {
        // Apply spin rotation
        model = glm::rotate(model, spinAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 
                      1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "isSkydome"), 0);
    
    this->model.Draw(shader);
} 