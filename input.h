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
#include "shader.h"


class input
{
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    input() {

    }
    
    bool addBind(char* key) {
        
        return true;
    }
    

private:


};
#endif
