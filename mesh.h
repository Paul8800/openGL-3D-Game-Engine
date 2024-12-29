#ifndef MESH_H
#define MESH_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"


#include "Shader.h"

#include <string>
#include <vector>
using namespace std;


struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

struct Texture {
  unsigned int id;
  string type;
  string path;
};

class Mesh {
  public:
    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    glm::mat4 modelTrans;

    //BB saves
    glm::vec3 center;
    float radius = -1.0f;

    float length = -1.0f;
    float width = -1.0f;
    float height = -1.0f;

    glm::vec3 rotationOBB = glm::vec3(-1.0f, -1.0f, -1.0f);

    glm::vec3 halfExtents = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 localCenter;
     glm::vec3 axes[3] = {glm::vec3(1.0f, 0.0f, 0.0f),  // X-axis
                         glm::vec3(0.0f, 1.0f, 0.0f),  // Y-axis
                         glm::vec3(0.0f, 0.0f, 1.0f)}; // Z-axis

    std::vector<glm::vec3> convexHullVertices;
    std::vector<glm::vec3> faceNormals;


    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
      this->vertices = vertices;
      this->indices = indices;
      this->textures = textures;
      setupMesh();
    }

    void Draw(Shader &shader)
    {
      unsigned int diffuseNr = 1;
      unsigned int specularNr = 1;
      for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
          number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
          number = std::to_string(specularNr++);

        shader.setFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
      }
      glActiveTexture(GL_TEXTURE0);
      // draw mesh
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    }

    void updateTransformations(glm::mat4 modelTrans2) {
      /*for (auto& vertex : vertices) {
        vertex.transformedPos = glm::vec3(modelTrans * glm::vec4(vertex.Position, 1.0f));
      }*/
      modelTrans = modelTrans2;
    }

  private:
    // render data
    unsigned int VAO, VBO, EBO;
    void setupMesh() {
      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glGenBuffers(1, &EBO);
      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

      // vertex positions
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

      // vertex normals
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

      // vertex texture coords
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
      glBindVertexArray(0);
    }
};

#endif
