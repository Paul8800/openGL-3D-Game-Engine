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

class player
{
public:
    unsigned int ID;

    float location;

    string username;
    float health = 100;

    float acceleration = 0;
    
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    player() {
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void move() { 
      
    }
#endif
