#include "Engine.hpp"
#include <iostream>

glm::mat3 calculateNormalMatrix(const glm::mat4& modelView) {
    glm::mat3 normalMatrix = glm::mat3(modelView);
    normalMatrix = glm::transpose(glm::inverse(normalMatrix));
    return normalMatrix;
}

Engine::Engine() : 
    glWindowWidth(1200), 
    glWindowHeight(900),
    glWindow(nullptr),
    camera(nullptr),
    controls(nullptr),
    rainSystem(nullptr) {
}

Engine::~Engine() {
    cleanup();
}

bool Engine::init() {
    if (!initOpenGLWindow()) {
        return false;
    }

    camera = new gps::Camera(
        glm::vec3(5.0f, 15.0f, 30.5f),
        glm::vec3(5.0f, 0.0f, -5.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    initOpenGLState();
    initObjects();
    initShaders();
    initUniforms();
    initFBO();

    controls = new Controls(glWindow, *camera, pokemons, rainSystem, 
                          audioManager, myCustomShader, lightShader, 
                          projectionLoc, lightDir);
    controls->setupCallbacks();

    return true;
}

void Engine::run() {
    while (!glfwWindowShouldClose(glWindow)) {
        controls->processMovement();
        renderScene();
        
        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }
}

bool Engine::initOpenGLWindow() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(glWindow);
    glfwSwapInterval(1);

#if not defined (__APPLE__)
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);
    gps::retina_width = retina_width;
    gps::retina_height = retina_height;

    return true;
}

void Engine::initOpenGLState() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_FRAMEBUFFER_SRGB);

    if (!audioManager.initialize()) {
        std::cerr << "Failed to initialize audio manager" << std::endl;
    }
}

void Engine::initObjects() {
    pokemons.push_back(new Pokemon("objects/pokemon/pikachu/pikachu2.obj", 
                                 glm::vec3(20.3f, 22.70f, 35.5f), 0.25f));
    
    pokemons.push_back(new Pokemon("objects/pokemon/umbreon/UmbreonHighPoly.obj", 
                                 glm::vec3(500.0f, 598.0f, 857.0f), 0.01f));
    
    auto yveltal = new Pokemon("objects/pokemon/yveltal/yveltal.obj", 
                              glm::vec3(520.0f, 998.0f, 857.0f), 0.01f);
    yveltal->setCircularFlight(700.0f, 100.0f, 0.1f);
    pokemons.push_back(yveltal);
    
    auto zapdos = new Pokemon("objects/pokemon/zapdos/zapdos.obj", 
                             glm::vec3(20.0f, 50.0f, 20.0f), 0.2f);
    zapdos->setFigureEightFlight(80.0f, 10.0f, 0.12f);
    pokemons.push_back(zapdos);
    
    ground.LoadModel("objects/world/world3.obj");
    lightCube.LoadModel("objects/cube/cube.obj");
    screenQuad.LoadModel("objects/quad/quad.obj");
}

void Engine::initShaders() {
    myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    myCustomShader.useShaderProgram();
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
    rainShader.loadShader("shaders/rain.vert", "shaders/rain.frag");
    rainShader.useShaderProgram();
}

void Engine::initUniforms() {
    myCustomShader.useShaderProgram();

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    view = camera->getViewMatrix();
    viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = calculateNormalMatrix(view * model);
    normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    projection = glm::perspective(glm::radians(45.0f),
                                (float)retina_width / (float)retina_height,
                                0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), 
                               glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    glm::mat3 normalMatrixLight = calculateNormalMatrix(view * lightRotation);
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(normalMatrixLight * lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 
                      1, GL_FALSE, glm::value_ptr(projection));
}

void Engine::initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    rainSystem = new Rain(100000);
}

glm::mat4 Engine::computeLightSpaceTrMatrix() {
    glm::mat4 lightRotationMatrix = glm::rotate(glm::mat4(1.0f), 
                                               glm::radians(controls->getLightAngle()), 
                                               glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 rotatedLightDir = glm::vec3(lightRotationMatrix * glm::vec4(lightDir, 0.0f));

    glm::mat4 lightView = glm::lookAt(rotatedLightDir * 5.0f, 
                                     glm::vec3(0.0f), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    
    return lightProjection * lightView;
}

void Engine::drawObjects(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    
    float deltaTime = 1.0f/60.0f;
    for (auto pokemon : pokemons) {
        pokemon->update(deltaTime);
        pokemon->draw(shader);
    }
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.03f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 
                      1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "isSkydome"), 1);
    ground.Draw(shader);

    if (!depthPass) {
        normalMatrix = calculateNormalMatrix(view * model);
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
}

void Engine::renderScene() {
    // depth maps creation pass
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
                       1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    drawObjects(depthMapShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render depth map on screen
    if (controls->isShowingDepthMap()) {
        glViewport(0, 0, retina_width, retina_height);
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuadShader.useShaderProgram();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);
        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    } else {
        // final scene rendering pass (with shadows)
        glViewport(0, 0, retina_width, retina_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        myCustomShader.useShaderProgram();

        view = camera->getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), 
                                  glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, 
                    glm::value_ptr(calculateNormalMatrix(view * lightRotation) * lightDir));

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
                          1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

        drawObjects(myCustomShader, false);

        lightShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 
                          1, GL_FALSE, glm::value_ptr(view));

        model = lightRotation;
        model = glm::translate(model, 1.0f * lightDir + glm::vec3(10.0f, 20.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 
                          1, GL_FALSE, glm::value_ptr(model));

        lightCube.Draw(lightShader);

        if (rainSystem->isEnabled()) {
            static float lastFrame = 0.0f;
            float currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            rainSystem->update(deltaTime, controls->getWindDirection(), controls->getWindStrength());
            rainSystem->updateBuffer();

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_PROGRAM_POINT_SIZE);

            rainShader.useShaderProgram();
            glUniformMatrix4fv(glGetUniformLocation(rainShader.shaderProgram, "view"), 
                             1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
            glUniformMatrix4fv(glGetUniformLocation(rainShader.shaderProgram, "projection"), 
                             1, GL_FALSE, glm::value_ptr(projection));
            glUniform3fv(glGetUniformLocation(rainShader.shaderProgram, "windDirection"), 
                        1, glm::value_ptr(controls->getWindDirection()));
            glUniform1f(glGetUniformLocation(rainShader.shaderProgram, "windStrength"), 
                       controls->getWindStrength());

            rainSystem->render();

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);
            glDisable(GL_PROGRAM_POINT_SIZE);
        }
    }
}

void Engine::cleanup() {
    for (auto pokemon : pokemons) {
        delete pokemon;
    }
    pokemons.clear();
    
    delete camera;
    delete controls;
    delete rainSystem;
    
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    
    audioManager.cleanup();
    glfwDestroyWindow(glWindow);
    glfwTerminate();
} 