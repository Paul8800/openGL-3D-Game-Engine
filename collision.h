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


    void initAABB(Mesh& obj) {

      // Step 1: Compute for the lowest and highest x values to determine length
      float lengthMin = obj.vertices[0].Position.x;
      float lengthMax = obj.vertices[0].Position.x;
      for (const auto& vertex : obj.vertices) {
          if (lengthMin > vertex.Position.x) lengthMin = vertex.Position.x;
          if (lengthMax < vertex.Position.x) lengthMax = vertex.Position.x;
      }
      obj.length = (lengthMax - lengthMin);

      float heightMin = obj.vertices[0].Position.y;
      float heightMax = obj.vertices[0].Position.y;
      for (const auto& vertex : obj.vertices) {
          if (heightMin > vertex.Position.y) heightMin = vertex.Position.y;
          if (heightMax < vertex.Position.y) heightMax = vertex.Position.y;
      }
      obj.height = (heightMax - heightMin);


      float widthMin = obj.vertices[0].Position.z;
      float widthMax = obj.vertices[0].Position.z;
      for (const auto& vertex : obj.vertices) {
          if (widthMin > vertex.Position.z) widthMin = vertex.Position.z;
          if (widthMax < vertex.Position.z) widthMax = vertex.Position.z;
      }
      obj.width = (widthMax - widthMin);

    }

    bool AABB(Mesh& objA, Mesh& objB) {

      if (objA.length == -1.0f) initAABB(objA);
      if (objB.length == -1.0f) initAABB(objB);

      glm::vec3 centerA = objA.center;
      glm::vec3 centerB = glm::vec3(objB.modelTrans * glm::vec4(objB.center, 1.0f));
      //glm::vec3 centerB = glm::vec3(objB.modelTrans * glm::vec4(objB.center, 1.0f));

      std::cout << "ObjA: " << objA.length << ",  " << objA.height << ",  " << objA.width << ",  : " << objA.center.y << std::endl; 
      std::cout << "ObjB  " << objB.length << ",  " << objB.height << ",  " << objB.width << ",  : " << objB.center.y << std::endl; 

      bool overlapX = (centerA.x + objA.length/2 > centerB.x - objB.length/2) &&
                      (centerA.x - objA.length/2 < centerB.x + objB.length/2);

      bool overlapY = (centerA.y + objA.height/2 > centerB.y - objB.height/2) &&
                      (centerA.y - objA.height/2 < centerB.y + objB.height/2);

      bool overlapZ = (centerA.z + objA.width/2 > centerB.z - objB.width/2) &&
                      (centerA.z - objA.width/2 < centerB.z + objB.width/2);

    // Return true if there is overlap in all three axes
    return overlapX && overlapY && overlapZ;

    }

    bool OBB(Mesh& objA, Mesh& objB) {
      return true;
    }

    bool convexHull(Mesh& objA, Mesh& objB) {
      return true;
    }

};

#endif
