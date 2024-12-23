#ifndef INPUT_H
#define INPUT_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "mesh.h"
#include "model.h"
#include "Shader.h"


class input
{
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    input() {

    }
    
    bool addBind(char* key) {
      
    }
    

private:
  void processInput(GLFWwindow *window){

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, false);

    float globalSpeedMultiplier = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      globalSpeedMultiplier = 25;

    float cameraSpeed = 12.5f * deltaTime * globalSpeedMultiplier;
    float verticalSpeed = 15.0f * deltaTime * globalSpeedMultiplier;

    float gravity = -9.81f/20;
    float jumpHeight = 10.0f/40;


    bool isOnGround = false;
    if (keySwitches["M"] == 1) {
      Cube playerBox({static_cast<float>(cameraPos.x), static_cast<float>(cameraPos.y), static_cast<float>(cameraPos.z)}, {1, 1, 1});
      playerBox.location[1] += -verticalSpeed * deltaTime; //apply movement to player
      if (checkCollision(cubeObjectList[0], playerBox) || checkCollision(cubeObjectList[1], playerBox) || checkCollision(cubeObjectList[2], playerBox)) {
        playerVelocityY = 0;
      } else {
        isOnGround = false;
        if (playerMove(glm::vec3(0.0f, playerVelocityY, 0.0f))) isOnGround = true;
        playerVelocityY += gravity *deltaTime;
      }
      playerBox.location[1] += verticalSpeed * deltaTime*50; //apply movement to player
    }
    
    
    

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
      //playerBox.location += (cameraFront.x, 0.0f,cameraFront.z) * cameraSpeed;
      //if (!checkCollision(cubeObjectList[0], playerBox)) cameraPos += glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed;}
      playerMove(glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
      //playerBox.location -= glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed;
      //if (!checkCollision(cubeObjectList[0], playerBox)) cameraPos -= glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed;}
      playerMove(-glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
      //playerBox.location -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      //if (!checkCollision(cubeObjectList[0], playerBox)) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;}
      playerMove(-glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
      //playerBox.location += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      //if (!checkCollision(cubeObjectList[0], playerBox)) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;}
      playerMove(glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      //playerBox.location[1] -= verticalSpeed;
      //if (!checkCollision(cubeObjectList[2], playerBox)) cameraPos -= glm::vec3(0.0f, verticalSpeed, 0.0f);
      playerMove(-glm::vec3(0.0f, verticalSpeed, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      if (isOnGround && keySwitches["M"] == 1) playerVelocityY = jumpHeight;
      if (keySwitches["M"]!= 1) cameraPos += glm::vec3(0.0f, verticalSpeed, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
      std::cout << std::round(cameraPos[0]) << ", " << std::round(cameraPos[1]) << ", " << std::round(cameraPos[2]) << std::endl;



    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keyHeld["M"]) {
      keyHeld["M"] = true;
      keySwitches["M"] += 1;
      if (keySwitches["M"] > 2) keySwitches["M"] = 0;
    } else if (glfwGetKey(window, GLFW_KEY_M) != GLFW_PRESS) {
      keyHeld["M"] = false;
    }


    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !keyHeld["F11"]) {
      if (keySwitches["F11"] != 1) {
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
      keySwitches["F11"]= 1;
      keyHeld["F11"] = true;
    } else {
      glfwSetWindowMonitor(window, NULL, 400, 300, 800, 600, 0);
      keySwitches["F11"] = 0;
    }keyHeld["F11"] = true;
    }else if(glfwGetKey(window, GLFW_KEY_F11) != GLFW_PRESS){keyHeld["F11"] = false;}

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !keyHeld["ESCAPE"]) {
      if (!keySwitches["ESCAPE"]) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        keySwitches["ESCAPE"] = 1;
      } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        keySwitches["ESCAPE"] = 0;
      }
      keyHeld["ESCAPE"] = true;
    } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {keyHeld["ESCAPE"] = false;}

  }

};
#endif
