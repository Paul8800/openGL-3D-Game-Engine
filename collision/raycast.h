#ifndef RAYCAST_H
#define RAYCAST_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "include/glm/gtx/string_cast.hpp" // For glm::to_string

#include <string>
#include <vector>
#include <algorithm>
using namespace std;


#include "mesh.h"
#include "model.h"

class Raycast {
  public:

    glm::vec3 origin;
    glm::vec3 destination;

    Raycast(glm::vec3 origin, glm::vec3 destination) {
      this.origin = origin;
      this.destination = destination;

    }

    void transform(glm::mat4 model) {
      origin = glm::vec3(model * glm::vec4(origin, 0.0f));
      destination = glm::vec3(model * glm::vec4(destination, 0.0f));
    }

    void translate(glm::vec3 translation) {
      glm::mat4 model(1.0f);
      model = glm::translate(model, translation);

      origin = glm::vec3(model * glm::vec4(origin, 0.0f));
      destination = glm::vec3(model * glm::vec4(destination, 0.0f));
    }
    void rotate(glm::vec3 rotation) {
      glm::mat4 model(1.0f);
      model = glm::rotate(model, rotation);

      origin = glm::vec3(model * glm::vec4(origin, 0.0f));
      destination = glm::vec3(model * glm::vec4(destination, 0.0f));
    }
    void scale(glm::vec3 scale) {
      glm::mat4 model(1.0f);
      model = glm::scale(model, scale);

      origin = glm::vec3(model * glm::vec4(origin, 0.0f));
      destination = glm::vec3(model * glm::vec4(destination, 0.0f));
    }

  private:

};

#endif
