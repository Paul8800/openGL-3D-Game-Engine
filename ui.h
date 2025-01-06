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
#include "shader.h"
#include "camera.h"


struct element {
  glm::vec3 Pos;

  bool Show;

  Mesh mesh;

  element(glm::vec3 position, bool show, const Mesh& m)
        : Pos(position), Show(show), mesh(m) {}
};

class Ui
{
public:
    vector<element> elementList;
    Camera* camera = nullptr;

    Ui(Camera* camera) {
      this->camera = camera;
    }
    
    void draw(Shader shader) {

      for (auto element : elementList) {
        if (element.Show == false) continue;


        float left = -14.0f;    // left bound of the viewing volume
        float right = 14.0f;    // right bound of the viewing volume
        float bottom = -14.0f;  // bottom bound of the viewing volume
        float top = 14.0f;      // top bound of the viewing volume
        float near = 0.1f;     // near bound of the viewing volume
        float far = 100.0f;    // far bound of the viewing volume

        // Create an orthographic projection matrix
        glm::mat4 projection = glm::ortho(left, right, bottom, top, near, far);

        // Assuming you have a shader program `shader`
        shader.setMat4("projection", projection);  // Set the projection matrix in the shader

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, camera->Position);
        model = glm::rotate(model, glm::radians(-camera->Rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(camera->Rotation.y), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(-camera->Rotation.z), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, element.Pos);

        shader.setMat4("model", model);

        element.mesh.updateTransformations(model);


        element.mesh.Draw(shader);
      }
    } 

    void addElement(Mesh mesh, glm::vec2 position, float zIndex) { //add a mesh to the UI
      element newElement(glm::vec3(zIndex, position), true, mesh);
      elementList.push_back(newElement);

    }

    void addElements(Model model, glm::vec2 position, float zIndex) { //add a model(model contains many meshes)
      for(unsigned int i = 0; i < model.meshes.size(); i++) {
        addElement(model.meshes[i], position, zIndex);
      }
    }
    

private:

};
#endif
