#ifndef COLLISION_H
#define COLLISION_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "include/glm/gtx/string_cast.hpp" // For glm::to_string

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
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

    bool checkRaycast() {
      return false;

    }

    
   

  private:
    void initSphereBB(Mesh& Obj) {
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

    glm::vec3 centerA = glm::vec3(ObjA.modelTrans * glm::vec4(ObjA.center, 1.0f));
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

      glm::vec3 centerA = glm::vec3(objA.modelTrans * glm::vec4(objA.center, 1.0f));
      glm::vec3 centerB = glm::vec3(objB.modelTrans * glm::vec4(objB.center, 1.0f));


      bool overlapX = (centerA.x + objA.length/2 > centerB.x - objB.length/2) &&
                      (centerA.x - objA.length/2 < centerB.x + objB.length/2);

      bool overlapY = (centerA.y + objA.height/2 > centerB.y - objB.height/2) &&
                      (centerA.y - objA.height/2 < centerB.y + objB.height/2);

      bool overlapZ = (centerA.z + objA.width/2 > centerB.z - objB.width/2) &&
                      (centerA.z - objA.width/2 < centerB.z + objB.width/2);

    // Return true if there is overlap in all three axes
    return overlapX && overlapY && overlapZ;

    }



glm::vec3 findAreaOfAABB(Mesh& obj, glm::mat4& r) {
    glm::vec3 dimensions;

    // Initialize min and max values to a very large and very small number
    float lengthMin = FLT_MAX, lengthMax = -FLT_MAX;
    float heightMin = FLT_MAX, heightMax = -FLT_MAX;
    float widthMin = FLT_MAX, widthMax = -FLT_MAX;

    // Iterate through all vertices of the mesh
    for (const auto& vertex : obj.vertices) {
        // Transform the vertex using the rotation matrix
        glm::vec3 currentVert = glm::vec3(r * glm::vec4(vertex.Position, 1.0f));

        // Update min and max for each axis
        if (lengthMin > currentVert.x) lengthMin = currentVert.x;
        if (lengthMax < currentVert.x) lengthMax = currentVert.x;

        if (heightMin > currentVert.y) heightMin = currentVert.y;
        if (heightMax < currentVert.y) heightMax = currentVert.y;

        if (widthMin > currentVert.z) widthMin = currentVert.z;
        if (widthMax < currentVert.z) widthMax = currentVert.z;
    }

    // Compute dimensions of the AABB
    dimensions.x = lengthMax - lengthMin;
    dimensions.y = heightMax - heightMin;
    dimensions.z = widthMax - widthMin;

    return dimensions;
}



void initOBB(Mesh& obj) {
  float volume = obj.length * obj.width * obj.height;

  glm::vec3 optimalRotation = glm::vec3(0.0f, 0.0f, 0.0f);

  const float angleStep = glm::radians(1.0f); // 1-degree step
  

  for (float x = 0; x < 360; x+=10) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 AABBrotated = findAreaOfAABB(obj, model);
    if ((AABBrotated.x * AABBrotated.y * AABBrotated.z) < volume) {
      volume = AABBrotated.x * AABBrotated.y * AABBrotated.z;
      optimalRotation.x = x;
    }

  }
  for (float y = 0; y < 360; y+=10) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 AABBrotated = findAreaOfAABB(obj, model);
    if ((AABBrotated.x * AABBrotated.y * AABBrotated.z) < volume) {
      volume = AABBrotated.x * AABBrotated.y * AABBrotated.z;
      optimalRotation.y = y;
    }

  }
  for (float z = 0; z < 360; z+=10) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 AABBrotated = findAreaOfAABB(obj, model);
    if ((AABBrotated.x * AABBrotated.y * AABBrotated.z) < volume) {
      volume = AABBrotated.x * AABBrotated.y * AABBrotated.z;
      optimalRotation.z = z;
    }

    std::cout << "Rotation: " << optimalRotation.x << ", " << optimalRotation.y << ", " << optimalRotation.z << std::endl;

  }
      
  obj.rotationOBB = optimalRotation;
}

glm::vec3 rotatedDims(float length, float width, float height, glm::vec3 rotation) {
    // Convert rotation angles (yaw, pitch, roll) from degrees to radians
    float yaw = glm::radians(-rotation.x);
    float pitch = glm::radians(-rotation.y);
    float roll = glm::radians(-rotation.z);

    // Define the rotation matrices for yaw, pitch, and roll
    glm::mat3 R_yaw = {
        glm::vec3(cos(yaw), 0.0f, sin(yaw)),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(-sin(yaw), 0.0f, cos(yaw))
    };

    glm::mat3 R_pitch = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, cos(pitch), -sin(pitch)),
        glm::vec3(0.0f, sin(pitch), cos(pitch))
    };

    glm::mat3 R_roll = {
        glm::vec3(cos(roll), -sin(roll), 0.0f),
        glm::vec3(sin(roll), cos(roll), 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };

    // Combine the rotation matrices: R = R_yaw * R_pitch * R_roll
    glm::mat3 R = R_yaw * R_pitch * R_roll;

    // Calculate the AABB dimensions
    float newLength = std::abs(length * R[0][0]) + std::abs(width * R[0][1]) + std::abs(height * R[0][2]);
    float newWidth = std::abs(length * R[1][0]) + std::abs(width * R[1][1]) + std::abs(height * R[1][2]);
    float newHeight = std::abs(length * R[2][0]) + std::abs(width * R[2][1]) + std::abs(height * R[2][2]);

    // Return the new dimensions as a glm::vec3
    return glm::vec3(newLength, newWidth, newHeight);
}


bool OBB(Mesh& objA, Mesh& objB) {
    // Ensure OBB rotations are initialized
    if (objA.rotationOBB.x == -1.0f) objA.rotationOBB = glm::vec3(-10.0f, 0.0f, -140.0f);
    if (objA.rotationOBB.x == -1.0f) initOBB(objA); 
    if (objB.rotationOBB.x == -1.0f) initOBB(objB); 

    // Helper lambda for overlap check
    auto checkOverlap = [](glm::vec3 centerA, glm::vec3 dimA, glm::vec3 centerB, glm::vec3 dimB) -> bool {
        return (centerA.x + dimA.x / 2 > centerB.x - dimB.x / 2 &&
                centerA.x - dimA.x / 2 < centerB.x + dimB.x / 2) &&
               (centerA.y + dimA.y / 2 > centerB.y - dimB.y / 2 &&
                centerA.y - dimA.y / 2 < centerB.y + dimB.y / 2) &&
               (centerA.z + dimA.z / 2 > centerB.z - dimB.z / 2 &&
                centerA.z - dimA.z / 2 < centerB.z + dimB.z / 2);
    };

    // Transform and check overlap in objA's reference frame
    glm::mat4 rotationMatrixA = glm::mat4(1.0f);
    rotationMatrixA = glm::rotate(rotationMatrixA, glm::radians(-objA.rotationOBB.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrixA = glm::rotate(rotationMatrixA, glm::radians(-objA.rotationOBB.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrixA = glm::rotate(rotationMatrixA, glm::radians(-objA.rotationOBB.z), glm::vec3(0.0f, 0.0f, 1.0f));


    glm::vec3 centerA = glm::vec3(objA.modelTrans * glm::vec4(objA.center, 1.0f));
    centerA = glm::vec3(rotationMatrixA * glm::vec4(centerA, 1.0f));

    glm::vec3 centerB = glm::vec3(objB.modelTrans * glm::vec4(objB.center, 1.0f));
    centerB = glm::vec3(rotationMatrixA * glm::vec4(centerB, 1.0f));

    glm::vec3 dimA = findAreaOfAABB(objA, rotationMatrixA);
    glm::vec3 dimB = findAreaOfAABB(objB, rotationMatrixA);
    //glm::vec3 dimA = rotatedDims(objA.length, objA.width, objA.height, objA.rotationOBB);
    //glm::vec3 dimB = rotatedDims(objB.length, objB.width, objB.height, objA.rotationOBB);



    /*std::cout << "" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "-                   ObjA rotations check                       -" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "objA centerA: " << centerA.x << ", " << centerA.y << ", " << centerA.z << std::endl;
    std::cout << "objA Dims: " << dimA.x << ", " << dimA.y << ", " << dimA.z << std::endl;
    std::cout << "objB centerA: " << centerB.x << ", " << centerB.y << ", " << centerB.z << std::endl;
    std::cout << "objB Dims: " << dimB.x << ", " << dimB.y << ", " << dimB.z << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;*/


    if (!checkOverlap(centerA, dimA, centerB, dimB)) return false;

    // Transform and check overlap in objB's reference frame
    glm::mat4 rotationMatrixB = glm::mat4(1.0f);
    rotationMatrixB = glm::rotate(rotationMatrixB, glm::radians(-objB.rotationOBB.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrixB = glm::rotate(rotationMatrixB, glm::radians(-objB.rotationOBB.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrixB = glm::rotate(rotationMatrixB, glm::radians(-objB.rotationOBB.z), glm::vec3(0.0f, 0.0f, 1.0f));

    centerA = glm::vec3(objA.modelTrans * glm::vec4(objA.center, 1.0f));
    centerA = glm::vec3(rotationMatrixB * glm::vec4(centerA, 1.0f));

    centerB = glm::vec3(objB.modelTrans * glm::vec4(objB.center, 1.0f));
    centerB = glm::vec3(rotationMatrixB * glm::vec4(centerB, 1.0f));

    dimA = findAreaOfAABB(objA, rotationMatrixB);
    dimB = findAreaOfAABB(objB, rotationMatrixB);
    //dimA = rotatedDims(objA.length, objA.width, objA.height, objB.rotationOBB);
    //dimB = rotatedDims(objB.length, objB.width, objB.height, objB.rotationOBB);



    /*std::cout << "" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "-                   ObjB rotations check                       -" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "objA centerA: " << centerA.x << ", " << centerA.y << ", " << centerA.z << std::endl;
    std::cout << "objA Dims: " << dimA.x << ", " << dimA.y << ", " << dimA.z << std::endl;
    std::cout << "objB centerA: " << centerB.x << ", " << centerB.y << ", " << centerB.z << std::endl;
    std::cout << "objB Dims: " << dimB.x << ", " << dimB.y << ", " << dimB.z << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;*/


    if (!checkOverlap(centerA, dimA, centerB, dimB)) return false;

    // If both checks pass, collision occurred
    return true;
}

    bool convexHull(Mesh& objA, Mesh& objB) {
        return true;
    }



    // Helper function to transform a vertex by a model matrix
glm::vec3 transformVertex(const glm::vec3& vertex, const glm::mat4& modelMatrix) {
    return glm::vec3(modelMatrix * glm::vec4(vertex, 1.0f));
}

// Helper function to check if a point lies inside a triangle
bool pointInTriangle(const glm::vec3& point, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec3 u = v1 - v0;
    glm::vec3 v = v2 - v0;
    glm::vec3 w = point - v0;

    float uu = glm::dot(u, u);
    float uv = glm::dot(u, v);
    float vv = glm::dot(v, v);
    float wu = glm::dot(w, u);
    float wv = glm::dot(w, v);

    float denominator = uv * uv - uu * vv;
    float s = (uv * wv - vv * wu) / denominator;
    float t = (uv * wu - uu * wv) / denominator;

    return (s >= 0.0f) && (t >= 0.0f) && (s + t <= 1.0f);
}

// Function to check if two triangles intersect
bool triangleIntersection(const glm::vec3& A1, const glm::vec3& A2, const glm::vec3& A3,
                          const glm::vec3& B1, const glm::vec3& B2, const glm::vec3& B3) {
    // Check if any vertex of one triangle is inside the other
    if (pointInTriangle(A1, B1, B2, B3) || pointInTriangle(A2, B1, B2, B3) || pointInTriangle(A3, B1, B2, B3)) {
        return true;
    }
    if (pointInTriangle(B1, A1, A2, A3) || pointInTriangle(B2, A1, A2, A3) || pointInTriangle(B3, A1, A2, A3)) {
        return true;
    }

    // Edge-edge intersection tests can be added here (optional).

    return false;
}

// Main function to check if two meshes collide
bool triangle(Mesh& objA, Mesh& objB) {
    // Transform the bounding sphere centers
    glm::vec3 transformedCenterA = transformVertex(objA.center, objA.modelTrans);
    glm::vec3 transformedCenterB = transformVertex(objB.center, objB.modelTrans);


    // Iterate over all triangles in both meshes
    for (size_t i = 0; i < objA.indices.size(); i += 3) {
        glm::vec3 A1 = transformVertex(objA.vertices[objA.indices[i]].Position, objA.modelTrans);
        glm::vec3 A2 = transformVertex(objA.vertices[objA.indices[i + 1]].Position, objA.modelTrans);
        glm::vec3 A3 = transformVertex(objA.vertices[objA.indices[i + 2]].Position, objA.modelTrans);

        for (size_t j = 0; j < objB.indices.size(); j += 3) {
            glm::vec3 B1 = transformVertex(objB.vertices[objB.indices[j]].Position, objB.modelTrans);
            glm::vec3 B2 = transformVertex(objB.vertices[objB.indices[j + 1]].Position, objB.modelTrans);
            glm::vec3 B3 = transformVertex(objB.vertices[objB.indices[j + 2]].Position, objB.modelTrans);

            if (triangleIntersection(A1, A2, A3, B1, B2, B3)) {
                return true;
            }
        }
    }

    return false;
}

};

#endif
