#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//Default values
const glm::vec3 POSITION(0.0f, 0.0f, 0.0f);

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float FOV         =  45.0f;

class Camera {
  public:
    glm::vec3 position;
    glm::vec3 rotation; //YAW, PITCH, ROLL
    float fov;

    bool mouseFocus;

    GLFWwindow window;

    Camera(GLFWwindow window) {
      this.window = window;
      glfwSetCursorPosCallback(window, mouseCallback);
    }

    focusMouse(bool option) {
      if (option) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
    }
  
  private:
    void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
      if (mouseFocus) {firstMouse=true; return;}
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
      cameraFront = glm::normalize(direction);
    }
}

#endif
