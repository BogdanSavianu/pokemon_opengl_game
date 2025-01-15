#include "Camera.hpp"

namespace gps {
    int retina_width;
    int retina_height;

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        this->fov = DEFAULT_FOV;
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += cameraFrontDirection * speed;
                break;
                
            case MOVE_BACKWARD:
                cameraPosition -= cameraFrontDirection * speed;
                break;
                
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * speed;
                break;
                
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * speed;
                break;
        }
    }

    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        cameraFrontDirection = glm::normalize(direction);
        
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    void Camera::zoom(float factor) {
        fov -= factor;
        if (fov < MIN_FOV)
            fov = MIN_FOV;
        if (fov > MAX_FOV)
            fov = MAX_FOV;
    }

    glm::mat4 Camera::getProjectionMatrix() {
        return glm::perspective(glm::radians(fov), 
                              (float)retina_width / (float)retina_height,
                              0.1f, 1000.0f);
    }

    void Camera::setPosition(const glm::vec3& position) {
        this->cameraPosition = position;
    }

    void Camera::lookAt(const glm::vec3& target) {
        this->cameraTarget = target;
        this->cameraFrontDirection = glm::normalize(target - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}
