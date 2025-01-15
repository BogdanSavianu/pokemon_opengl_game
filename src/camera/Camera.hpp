#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps {
    extern int retina_width;
    extern int retina_height;
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT};
    
    class Camera {

    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction, float speed);
        void rotate(float pitch, float yaw);
        void zoom(float factor);
        glm::mat4 getProjectionMatrix();
        
        glm::vec3 getCameraPosition() const { return cameraPosition; }
        
        void setPosition(const glm::vec3& position);
        void lookAt(const glm::vec3& target);
        
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        float fov;
        const float MIN_FOV = 10.0f;
        const float MAX_FOV = 45.0f;
        const float DEFAULT_FOV = 45.0f;
    };    
}

#endif
