#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Default camera values
constexpr float DEFAULT_SENSITIVITY =  0.15f;
constexpr float DEFAULT_ZOOM        =  45.0f;
constexpr float DEFAULT_YAW         = 45.0f;
constexpr float DEFAULT_PITCH       =  0.0f;


enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    glm::vec3 Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch;
    float MouseSensitivity, Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    glm::mat4 GetViewMatrix();
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

private:
    void updateCameraVectors();
};

#endif
