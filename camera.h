#ifndef CAMERA_H
#define CAMERA_H

//#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"

//Default values
const glm::vec3 POSITION(0.0f, 0.0f, 0.0f);

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float FOV         =  45.0f;

class Camera {
  public:
    glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f); // X, Y, Z
    glm::vec3 Rotation = glm::vec3(-90.0f, 0.0f, 0.0f); // YAW, PITCH, ROLL
    float Fov;

    bool MouseFocus;


    Camera(GLFWwindow* window) {
      this->window = window;
      //glfwSetCursorPosCallback(window, mouseCallback);

    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void updateCameraView() {
      updateCameraVectors();
    }

    void focusMouse(bool option) {
      if (option) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        MouseFocus = true;
      } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        MouseFocus = false;
      }
    }


  
  private:

    GLFWwindow* window;
    glm::mat4 view;

    bool firstMouse = true;
    float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch =  0.0f;
    float roll =     0.0f;
    float lastX =  800.0f / 2.0;
    float lastY =  600.0 / 2.0;

    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);


    void updateCameraVectors() {
      // calculate the new Front vector
      glm::vec3 front;
      front.x = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
      front.y = sin(glm::radians(Rotation.y));
      front.z = sin(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
      Front = glm::normalize(front);
      // also re-calculate the Right and Up vector
      glm::vec3 Right = glm::normalize(glm::cross(Front, Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
      glm::vec3 Up    = glm::normalize(glm::cross(Right, Front));
    }

    void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
      if (MouseFocus) {firstMouse=true; return;}
      if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
      }
      float xoffset = xpos - lastX;
      float yoffset = lastY - ypos;
      lastX = xpos;
      lastY = ypos;
      float sensitivity = 0.05f;
      xoffset *= sensitivity;
      yoffset *= sensitivity;
      yaw += xoffset;
      pitch += yoffset;
      if(pitch > 89.0f)
        pitch = 89.0f;
      if(pitch < -89.0f)
        pitch = -89.0f;
      glm::vec3 direction;
      direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      direction.y = sin(glm::radians(pitch));
      direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      Front = glm::normalize(direction);
    }

    
};

#endif
