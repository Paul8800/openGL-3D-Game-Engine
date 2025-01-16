#ifndef SHADER_H
#define SHADER_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "collision.h"

class player
{
public:
    unsigned int ID;

    float position;
    glm::vec3 momentum = glm::vec3(0);
    bool isOnGround = false;
    
    // Constructor
    player() {
    }

    void move(glm::vec3 movement, bool momentumBased) { 
        if (momentumBased) movement *= momentum;

      
    }
public:
};
#endif
