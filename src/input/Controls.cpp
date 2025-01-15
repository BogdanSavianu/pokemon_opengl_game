#include "Controls.hpp"
#include <iostream>
#include <iomanip>

Controls* Controls::instance = nullptr;

Controls::Controls(GLFWwindow* window, gps::Camera& camera, std::vector<Pokemon*>& pokemons, 
                  Rain* rainSystem, AudioManager& audioManager, gps::Shader& customShader, 
                  gps::Shader& lightShader, GLuint& projLoc, glm::vec3& lightDir)
    : window(window), camera(camera), pokemons(pokemons), 
      rainSystem(rainSystem), audioManager(audioManager),
      myCustomShader(customShader), lightShader(lightShader),
      projectionLoc(projLoc), lightDir(lightDir) {
    
    instance = this;
    
    for (int i = 0; i < 1024; i++) {
        pressedKeys[i] = false;
    }
    
    // Initialize control states
    cursorLocked = true;
    angleY = 0.0f;
    lightAngle = 0.0f;
    keyUPressed = false;
    keyRPressed = false;
    firstMouse = true;
    lastX = 800.0f / 2.0f;
    lastY = 600.0f / 2.0f;
    yaw = -90.0f;
    pitch = 0.0f;
    cameraSpeed = 0.03f;
    
    // Initialize wind parameters
    windDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    windStrength = 5.0f;
    windEnabled = true;
    currentWindAngle = 90.0f;
    
    rainSoundPlaying = false;
    spinSoundPlaying = false;
    
    if (!audioManager.loadSound("sounds/rain.wav", rainBuffer)) {
        std::cerr << "Failed to load rain sound" << std::endl;
    }
    if (!audioManager.loadSound("sounds/oiia_cat.wav", spinSoundBuffer)) {
        std::cerr << "Failed to load spin sound" << std::endl;
    }

    showDepthMap = false;
    
    // Initialize presentation mode
    presentationMode = false;
    presentationAngle = 0.0f;
    presentationRadius = INITIAL_RADIUS;
    presentationCenter = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Controls::setupCallbacks() {
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Controls::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (instance) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (key >= 0 && key < 1024) {
            if (action == GLFW_PRESS)
                instance->pressedKeys[key] = true;
            else if (action == GLFW_RELEASE)
                instance->pressedKeys[key] = false;
        }
    }
}

void Controls::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!instance || !instance->cursorLocked) return;
    
    if (instance->firstMouse) {
        instance->lastX = xpos;
        instance->lastY = ypos;
        instance->firstMouse = false;
        return;
    }

    float xoffset = xpos - instance->lastX;
    float yoffset = instance->lastY - ypos;
    instance->lastX = xpos;
    instance->lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    instance->yaw += xoffset;
    instance->pitch += yoffset;

    if (instance->pitch > 89.0f) instance->pitch = 89.0f;
    if (instance->pitch < -89.0f) instance->pitch = -89.0f;

    instance->camera.rotate(instance->pitch, instance->yaw);
}

void Controls::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (instance && button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        instance->cursorLocked = !instance->cursorLocked;
        
        if (instance->cursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            instance->firstMouse = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void Controls::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (instance) {
        float zoomSpeed = 1.0f;
        instance->camera.zoom(yoffset * zoomSpeed);
        
        instance->updateProjectionMatrix(instance->myCustomShader, instance->projectionLoc);
        instance->updateProjectionMatrix(instance->lightShader, 
            glGetUniformLocation(instance->lightShader.shaderProgram, "projection"));
    }
}

void Controls::printWindInfo() {
    float degrees = glm::degrees(atan2(windDirection.z, windDirection.x));
    std::string direction;

    if (degrees >= -22.5 && degrees < 22.5) direction = "East";
    else if (degrees >= 22.5 && degrees < 67.5) direction = "Southeast";
    else if (degrees >= 67.5 && degrees < 112.5) direction = "South";
    else if (degrees >= 112.5 && degrees < 157.5) direction = "Southwest";
    else if (degrees >= 157.5 || degrees < -157.5) direction = "West";
    else if (degrees >= -157.5 && degrees < -112.5) direction = "Northwest";
    else if (degrees >= -112.5 && degrees < -67.5) direction = "North";
    else if (degrees >= -67.5 && degrees < -22.5) direction = "Northeast";

    std::cout << "Wind direction: " << direction
              << " (" << std::fixed << std::setprecision(1) << degrees << "°)"
              << " Strength: " << windStrength << std::endl;
}

void Controls::processMovement() {
    static bool wireframeMode = false;
    static bool pointMode = false;

    if (pressedKeys[GLFW_KEY_I]) {
        wireframeMode = !wireframeMode;
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        pressedKeys[GLFW_KEY_I] = false;
    }

    if (pressedKeys[GLFW_KEY_O]) {
        pointMode = !pointMode;
        if (pointMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        pressedKeys[GLFW_KEY_O] = false;
    }

    // Pokemon controls
    if (pressedKeys[GLFW_KEY_Q]) {
        angleY -= 1.0f;
        bool anyPokemonSpinning = false;
        for (auto pokemon : pokemons) {
            pokemon->spin(-0.1f);
            if (pokemon->isSpinning()) anyPokemonSpinning = true;
        }
        if (anyPokemonSpinning && !spinSoundPlaying) {
            audioManager.playSound(spinSoundBuffer, true);
            spinSoundPlaying = true;
        }
    } else if (pressedKeys[GLFW_KEY_E]) {
        angleY += 1.0f;
        bool anyPokemonSpinning = false;
        for (auto pokemon : pokemons) {
            pokemon->spin(0.1f);
            if (pokemon->isSpinning()) anyPokemonSpinning = true;
        }
        if (anyPokemonSpinning && !spinSoundPlaying) {
            audioManager.playSound(spinSoundBuffer, true);
            spinSoundPlaying = true;
        }
    } else {
        for (auto pokemon : pokemons) {
            pokemon->stopJumping();
        }
        if (spinSoundPlaying) {
            audioManager.stopSound();
            spinSoundPlaying = false;
        }
    }

    // Scene presentation mode
    if (pressedKeys[GLFW_KEY_T]) {
        presentationMode = !presentationMode;
        pressedKeys[GLFW_KEY_T] = false;
        
        if (presentationMode) {
            std::cout << "Presentation mode enabled" << std::endl;
        } else {
            std::cout << "Presentation mode disabled" << std::endl;
        }
    }
    
    if (presentationMode) {
        float progress = (presentationRadius - FINAL_RADIUS) / (INITIAL_RADIUS - FINAL_RADIUS);
        float currentHeight = FINAL_HEIGHT + (INITIAL_HEIGHT - FINAL_HEIGHT) * progress;
        
        float x = presentationCenter.x + presentationRadius * cos(glm::radians(presentationAngle));
        float z = presentationCenter.z + presentationRadius * sin(glm::radians(presentationAngle));
        float y = presentationCenter.y + currentHeight;
        
        camera.setPosition(glm::vec3(x, y, z));
        camera.lookAt(presentationCenter);
        
        presentationAngle += PRESENTATION_SPEED;
        if (presentationAngle >= 360.0f) {
            presentationAngle = 0.0f;
            
            if (presentationRadius > FINAL_RADIUS) {
                presentationRadius -= RADIUS_DECREASE_SPEED;
                if (presentationRadius < FINAL_RADIUS) {
                    presentationRadius = FINAL_RADIUS;
                }
            }
        }
        
        // End presentation mode
        if (presentationRadius <= FINAL_RADIUS && presentationAngle < 0.1f) {
            presentationMode = false;
            std::cout << "Presentation complete" << std::endl;
        }
    }
    
    if (!presentationMode) {
        if (pressedKeys[GLFW_KEY_W]) {
            camera.move(gps::MOVE_FORWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_S]) {
            camera.move(gps::MOVE_BACKWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_A]) {
            camera.move(gps::MOVE_LEFT, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_D]) {
            camera.move(gps::MOVE_RIGHT, cameraSpeed);
        }
    }

    // Light controls
    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 1.0f;
        glm::mat4 lightRotation = glm::rotate(glm::mat4(1.0f), 
                                             glm::radians(lightAngle), 
                                             glm::vec3(0.0f, 1.0f, 0.0f));
        lightDir = glm::vec3(lightRotation * glm::vec4(0.0f, 1.0f, 1.0f, 0.0f));
    }
    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 1.0f;
        glm::mat4 lightRotation = glm::rotate(glm::mat4(1.0f), 
                                             glm::radians(lightAngle), 
                                             glm::vec3(0.0f, 1.0f, 0.0f));
        lightDir = glm::vec3(lightRotation * glm::vec4(0.0f, 1.0f, 1.0f, 0.0f));
    }

    // Wind controls
    if (pressedKeys[GLFW_KEY_U] && !keyUPressed) {
        windEnabled = !windEnabled;
        keyUPressed = true;
        std::cout << "Wind " << (windEnabled ? "enabled" : "disabled") << std::endl;
    }
    if (!pressedKeys[GLFW_KEY_U]) {
        keyUPressed = false;
    }

    if (windEnabled) {
        if (pressedKeys[GLFW_KEY_1]) {
            currentWindAngle = glm::clamp(currentWindAngle - 1.0f, MIN_WIND_ANGLE, MAX_WIND_ANGLE);
            windDirection.x = cos(glm::radians(currentWindAngle));
            windDirection.z = sin(glm::radians(currentWindAngle));
            windDirection = glm::normalize(windDirection);
            printWindInfo();
        }
        if (pressedKeys[GLFW_KEY_2]) {
            currentWindAngle = glm::clamp(currentWindAngle + 1.0f, MIN_WIND_ANGLE, MAX_WIND_ANGLE);
            windDirection.x = cos(glm::radians(currentWindAngle));
            windDirection.z = sin(glm::radians(currentWindAngle));
            windDirection = glm::normalize(windDirection);
            printWindInfo();
        }
        if (pressedKeys[GLFW_KEY_3]) {
            windStrength = glm::min(windStrength + 0.2f, 15.0f);
            printWindInfo();
        }
        if (pressedKeys[GLFW_KEY_4]) {
            windStrength = glm::max(windStrength - 0.2f, 0.0f);
            printWindInfo();
        }
    }

    // Rain controls
    if (pressedKeys[GLFW_KEY_R] && !keyRPressed) {
        bool wasEnabled = rainSystem->isEnabled();
        rainSystem->toggleEnabled();
        keyRPressed = true;
        
        if (!wasEnabled && rainSystem->isEnabled()) {
            audioManager.playSound(rainBuffer, true);
            rainSoundPlaying = true;
            std::cout << "Rain enabled" << std::endl;
        } else if (wasEnabled && !rainSystem->isEnabled()) {
            audioManager.stopSound();
            rainSoundPlaying = false;
            std::cout << "Rain disabled" << std::endl;
        }
    }
    if (!pressedKeys[GLFW_KEY_R]) {
        keyRPressed = false;
    }

    // Depth map toggle
    if (pressedKeys[GLFW_KEY_M]) {
        showDepthMap = !showDepthMap;
        pressedKeys[GLFW_KEY_M] = false;  
    }
}

glm::mat4 Controls::getProjectionMatrix() const {
    return camera.getProjectionMatrix();
}

void Controls::updateProjectionMatrix(gps::Shader& shader, GLuint location) {
    shader.useShaderProgram();
    glm::mat4 projMatrix = camera.getProjectionMatrix();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projMatrix));
} 