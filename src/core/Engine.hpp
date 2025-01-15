#ifndef Engine_hpp
#define Engine_hpp

#include <GLFW/glfw3.h>
#include "../camera/Camera.hpp"
#include "../graphics/effects/Rain.hpp"
#include "../audio/AudioManager.hpp"
#include "../entities/Pokemon.hpp"
#include "../input/Controls.hpp"
#include "../graphics/shaders/Shader.hpp"
#include <vector>

class Engine {
public:
    Engine();
    ~Engine();
    
    bool init();
    void run();
    void cleanup();
    
private:
    bool initOpenGLWindow();
    void initOpenGLState();
    void initObjects();
    void initShaders();
    void initUniforms();
    void initFBO();
    void renderScene();
    
    // Window properties
    GLFWwindow* glWindow;
    int glWindowWidth;
    int glWindowHeight;
    int retina_width, retina_height;
    
    // Core components
    gps::Camera* camera;
    Controls* controls;
    Rain* rainSystem;
    AudioManager audioManager;
    std::vector<Pokemon*> pokemons;
    
    // Shaders
    gps::Shader myCustomShader;
    gps::Shader lightShader;
    gps::Shader screenQuadShader;
    gps::Shader depthMapShader;
    gps::Shader rainShader;
    
    // Models
    gps::Model3D ground;
    gps::Model3D lightCube;
    gps::Model3D screenQuad;
    
    // Matrices and uniforms
    glm::mat4 model;
    GLuint modelLoc;
    glm::mat4 view;
    GLuint viewLoc;
    glm::mat4 projection;
    GLuint projectionLoc;
    glm::mat3 normalMatrix;
    GLuint normalMatrixLoc;
    glm::mat4 lightRotation;
    
    // Lighting
    glm::vec3 lightDir;
    GLuint lightDirLoc;
    glm::vec3 lightColor;
    GLuint lightColorLoc;
    GLfloat lightAngle;
    
    // Shadow mapping
    GLuint shadowMapFBO;
    GLuint depthMapTexture;
    const unsigned int SHADOW_WIDTH = 2048;
    const unsigned int SHADOW_HEIGHT = 2048;
    
    glm::mat4 computeLightSpaceTrMatrix();
    void drawObjects(gps::Shader shader, bool depthPass);
};

#endif /* Engine_hpp */ 