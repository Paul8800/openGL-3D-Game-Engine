#ifndef COLLISION_H
#define COLLISION_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"

#include <string>
#include <vector>
using namespace std;



#include "mesh.h"
#include "model.h"

class Collision {
  public:

    bool checkCollision(Model modelA, Model modelB, int precisionLvl) {
      //Checks collision between 2 model.h objects
      //
      //precisionLvl - 0 means weakest, 3 means greatest, anything ouside 0-3 does greatest
      
      vector<Mesh> meshesA = modelA.meshes;
      vector<Mesh> meshesB = modelB.meshes;

      for (int a = 0; a < meshesA.size(); a++) {
        for (int b = 0; b < meshesB.size(); b++) {
      
          if (meshesA[a].vertices.empty() || meshesB[b].vertices.empty()) break;

          if (sphere(meshesA[a], meshesB[b])) {
              return true;
            if (precisionLvl == 0) return true;
            if (AABB(meshesA[a], meshesB[b])) {
              if (precisionLvl == 1) return true;
              if (OBB(meshesA[a], meshesB[b])) {
                if (precisionLvl == 2) return true;
                return convexHull(meshesA[a], meshesB[b]);
              }
            }
          }

        }
      }

      return false;
    }

    
   

  private:
    bool sphere(Mesh ObjA, Mesh ObjB) {

    glm::vec3 centerA(0.0f), centerB(0.0f);
    float radiusA = 0.0f, radiusB = 0.0f;

    //Step 1: Compute the center of the sphere (average of all vertices)
    for (const auto& vertex : ObjA.vertices) {
        centerA += vertex.Position;
    }
    centerA /= static_cast<float>(ObjA.vertices.size());

    for (const auto& vertex : ObjB.vertices) {
      //std::cout << vertex.transformedPos.x << ", " << vertex.transformedPos.y << ", " << vertex.transformedPos.z << std::endl;
        centerB += vertex.Position;
    }
    centerB /= static_cast<float>(ObjB.vertices.size());

    // Step 2: Compute the radius of the sphere (max distance from center)
    for (const auto& vertex : ObjA.vertices) {
        float distance = glm::length(vertex.Position - centerA);
        if (distance > radiusA) {
            radiusA = distance;
        }
    }

    for (const auto& vertex : ObjB.vertices) {
        float distance = glm::length(vertex.Position - centerB);
        if (distance > radiusB) {
            radiusB = distance;
        }
    }

    //centerA = glm::vec3(ObjA.modelTrans * glm::vec4(centerA, 1.0f));
    centerB = glm::vec3(ObjB.modelTrans * glm::vec4(centerB, 1.0f));

    float distance = glm::length(centerA - centerB);
    
    /*
    std::cout << "Center A: " << centerA.x << ", " << centerA.y << ", " << centerA.z << std::endl;
    std::cout << "Radius A: " << radiusA << std::endl;

    std::cout << "Center B: " << centerB.x << ", " << centerB.y << ", " << centerB.z << std::endl;
    std::cout << "Radius B: " << radiusB << std::endl;

    std::cout << "Distance: " << distance << std::endl;
    std::cout << "Sum of Radii: " << radiusA + radiusB << std::endl;
    */
    

    return distance <= (radiusA + radiusB);
    }

    bool AABB(Mesh model, Mesh model2) {
      return true;
    }

    bool OBB(Mesh model, Mesh model2) {
      return true;
    }

    bool convexHull(Mesh model, Mesh model2) {
      return true;
    }

};

#endif
