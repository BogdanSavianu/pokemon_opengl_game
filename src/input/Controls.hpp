#ifndef Controls_hpp
#define Controls_hpp

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../camera/Camera.hpp"
#include "../entities/Pokemon.hpp"
#include "../graphics/effects/Rain.hpp"
#include "../audio/AudioManager.hpp"
#include <vector>

class Controls {
public:
    Controls(GLFWwindow* window, gps::Camera& camera, std::vector<Pokemon*>& pokemons, 
             Rain* rainSystem, AudioManager& audioManager, gps::Shader& customShader, 
             gps::Shader& lightShader, GLuint& projLoc, glm::vec3& lightDir);
    
    void processMovement();
    void setupCallbacks();
    
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    glm::vec3 getWindDirection() const { return windDirection; }
    float getWindStrength() const { return windStrength; }
    float getLightAngle() const { return lightAngle; }
    
    glm::mat4 getProjectionMatrix() const;
    void updateProjectionMatrix(gps::Shader& shader, GLuint location);
    
    bool isShowingDepthMap() const { return showDepthMap; }
    
private:
    GLFWwindow* window;
    gps::Camera& camera;
    std::vector<Pokemon*>& pokemons;
    Rain* rainSystem;
    AudioManager& audioManager;
    
    // Control states
    bool pressedKeys[1024];
    bool cursorLocked;
    float angleY;
    float lightAngle;
    bool keyUPressed;
    bool keyRPressed;
    bool firstMouse;
    float lastX, lastY;
    float yaw, pitch;
    float cameraSpeed;
    
    // Wind control
    glm::vec3 windDirection;
    float windStrength;
    bool windEnabled;
    float currentWindAngle;
    const float MIN_WIND_ANGLE = -180.0f;
    const float MAX_WIND_ANGLE = 180.0f;
    
    // Sound states
    ALuint rainBuffer;
    ALuint spinSoundBuffer;
    bool rainSoundPlaying;
    bool spinSoundPlaying;
    
    void printWindInfo();
    
    static Controls* instance;
    
    // Add these members
    gps::Shader& myCustomShader;
    gps::Shader& lightShader;
    GLuint& projectionLoc;
    
    glm::vec3& lightDir;  
    
    bool showDepthMap;
    
    // Scene presentation
    bool presentationMode;
    float presentationAngle;
    float presentationRadius; 
    const float INITIAL_RADIUS = 50.0f;
    const float FINAL_RADIUS = 10.0f;  
    const float INITIAL_HEIGHT = 30.0f;   
    const float FINAL_HEIGHT = 5.0f;    
    const float PRESENTATION_SPEED = 0.2f; 
    const float RADIUS_DECREASE_SPEED = 10.0f;
    glm::vec3 presentationCenter;
};

#endif