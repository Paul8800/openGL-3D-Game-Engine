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

    bool checkCollision(Model& modelA, Model& modelB, int precisionLvl) {
      //Checks collision between 2 model.h objects
      //
  //precisionLvl - 0 means weakest, 3 means greatest, anything ouside 0-3 does greatest
      
      vector<Mesh>& meshesA = modelA.meshes;
      vector<Mesh>& meshesB = modelB.meshes;

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
    void initSphereBB(Mesh& Obj) {
      std::cout << "ran init of BB" << std::endl;
      glm::vec3 center(0.0f);
      float radius = 0.0f;

      //Step 1: Compute the center of the sphere (average of all vertices)
      for (const auto& vertex : Obj.vertices) {
          center += vertex.Position;
      }
      center /= static_cast<float>(Obj.vertices.size());

      // Step 2: Compute the radius of the sphere (max distance from center)
      for (const auto& vertex : Obj.vertices) {
          float distance = glm::length(vertex.Position - center);
          if (distance > radius) {
              radius = distance;
          }
      }

      Obj.center = center;
      Obj.radius = radius;
    }

    bool sphere(Mesh& ObjA, Mesh& ObjB) {

    if (ObjA.radius == -1.0f) initSphereBB(ObjA);
    if (ObjB.radius == -1.0f) initSphereBB(ObjB);

    //glm::vec3 centerA = glm::vec3(ObjA.modelTrans * glm::vec4(ObjA.center, 1.0f));
    glm::vec3 centerA = ObjA.center;
    glm::vec3 centerB = glm::vec3(ObjB.modelTrans * glm::vec4(ObjB.center, 1.0f));

    float distance = glm::length(centerA - centerB);
    

    return distance <= (ObjA.radius + ObjB.radius);
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
