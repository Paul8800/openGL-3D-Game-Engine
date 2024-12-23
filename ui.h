#ifndef UI_H
#define UI_H

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


struct element {
  glm::vec2 pos;

  bool show;

  Mesh mesh
};

class ui
{
public:
    Vector<element> elementList;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    ui(glm::vec2 screenSize) {

    }
    
    void draw(glm::vec3 cameraPos, glm::vec3 cameraPos, Shader shader) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cameraPos);
      model = glm::rotate(model, translation);

      for(unsigned int i = 0; i < elementList.size(); i++) {
        elementList[i].mesh.Draw(shader);
      }
    } 

    void addElement(Mesh mesh) { //add a mesh to the UI
      element newElement;
      newElement.mesh = mesh;
      elementList.push_back(meshes[i]);

    }

    void addElements(Model model) { //add a model(model contains many meshes)
      for(unsigned int i = 0; i < model.meshes.size(); i++) {
        addElement(model.meshes[i]);
      }
    }
    

private:

};
#endif
