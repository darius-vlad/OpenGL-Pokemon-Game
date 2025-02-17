#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = glm::normalize(cameraUp);

        //TODO - Update the rest of camera parameters

        glm::vec3 forward = glm::normalize(cameraTarget - cameraPosition);
        glm::vec3 right = glm::normalize(glm::cross(forward, this->cameraUpDirection));
        glm::vec3 up = glm::cross(right, forward);

        this->cameraFrontDirection = forward;
        this->cameraRightDirection = right;

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed , bool flying) {
        //TODO
        if (direction == MOVE_FORWARD)
        {
            this->cameraPosition += (this->cameraFrontDirection * speed);

        }

        if (direction == MOVE_BACKWARD)
        {
            this->cameraPosition -= (this->cameraFrontDirection * speed);
        }

        if (direction == MOVE_RIGHT)
        {

            this->cameraPosition += (glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed);
        }
        if (direction == MOVE_LEFT)
        {
            this->cameraPosition -= (glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed);
        }
        if (flying == false)
        {
            this->cameraPosition.y = 2.0f;
        }
        this->cameraTarget = this->cameraPosition + this->cameraFrontDirection;

    }


    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraFrontDirection = glm::normalize(direction);

        const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, worldUp));
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraRightDirection, this->cameraFrontDirection));
        this->cameraTarget = this->cameraPosition + this->cameraFrontDirection;
    }


    //modificarile mele

    glm::vec3 Camera::getCameraPosition()
    {
        return this->cameraPosition;
    }

    glm::vec3 Camera::getCameraFront()
    {
        return this->cameraFrontDirection;
    }

    glm::vec3 Camera::getCameraUp()
    {
        return this->cameraUpDirection;
    }

    void Camera::setPosition(glm::vec3 position)
    {
        this->cameraPosition = position;
    }

}