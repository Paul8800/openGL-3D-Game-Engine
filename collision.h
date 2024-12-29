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

  /*for (float x = 0; x < 360; x+=10) {
    for (float y = 0; y < 360; y+=10) {
      for (float z = 0; z < 360; z+=10) {
        model = glm::rotate(model, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));

        //gulm::mat4 rotation = glm::yawPitchRoll(x * angleStep, y * angleStep, z * angleStep);

        float newVol = findAreaOfAABB(obj, model);
        if (newVol < volume) {
          volume = newVol;
          optimalRotation = glm::vec3(x, y, z);
        }
      }
    }
  }*/
      
  obj.rotationOBB = optimalRotation;
}
    
bool OBB(Mesh& objA, Mesh& objB) {
  if (objA.rotationOBB.x == -1.0f) initOBB(objA);
  if (objB.rotationOBB.x == -1.0f) initOBB(objB);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(objA.rotationOBB.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(objA.rotationOBB.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(objA.rotationOBB.z), glm::vec3(0.0f, 0.0f, 1.0f));
  glm::vec3 dimB = findAreaOfAABB(objB, model);

  glm::mat4 combinedTransform = objA.modelTrans * model;
  glm::vec3 centerA = glm::vec3(combinedTransform * glm::vec4(objA.center, 1.0f));
  //centerA = glm::vec3(model * glm::vec4(objA.center, 1.0f));

  

  glm::vec3 dimA = findAreaOfAABB(objA, model);

  combinedTransform = objB.modelTrans * model;
  glm::vec3 centerB = glm::vec3(combinedTransform * glm::vec4(objB.center, 1.0f));
  //centerB = glm::vec3(model * glm::vec4(objB.center, 1.0f));





  bool overlapX = (centerA.x + dimA.x/2 > centerB.x - dimB.x/2) &&
                  (centerA.x - dimA.x/2 < centerB.x + dimB.x/2);

  bool overlapY = (centerA.y + dimA.y/2 > centerB.y - dimB.y/2) &&
                  (centerA.y - dimA.y/2 < centerB.y + dimB.y/2);

  bool overlapZ = (centerA.z + dimA.z/2 > centerB.z - dimB.z/2) &&
                  (centerA.z - dimA.z/2 < centerB.z + dimB.z/2);

  // Return true if there is overlap in all three axes
  if (!(overlapX && overlapY && overlapZ)) return false;




  return true;
}








void initOBB2(Mesh& obj) {
    glm::vec3 minPoint = obj.vertices[0].Position;
    glm::vec3 maxPoint = obj.vertices[0].Position;

    for (const auto& vertex : obj.vertices) {
        minPoint = glm::min(minPoint, vertex.Position);
        maxPoint = glm::max(maxPoint, vertex.Position);
    }

    // Set the local center and half-extents in local space
    obj.localCenter = (maxPoint + minPoint) / 2.0f;
    obj.halfExtents = (maxPoint - minPoint) / 2.0f;

    // Transform axes (identity by default, updated in OBB collision check)
    obj.axes[0] = glm::vec3(1.0f, 0.0f, 0.0f); // X-axis
    obj.axes[1] = glm::vec3(0.0f, 1.0f, 0.0f); // Y-axis
    obj.axes[2] = glm::vec3(0.0f, 0.0f, 1.0f); // Z-axis
}


    

bool OBB2(Mesh& objA, Mesh& objB) {
    if (objA.halfExtents.x == -1.0f) initOBB(objA);
    if (objB.halfExtents.x == -1.0f) initOBB(objB);

    // Transform centers into world space
    glm::vec3 centerA = glm::vec3(objA.modelTrans * glm::vec4(objA.localCenter, 1.0f));
    glm::vec3 centerB = glm::vec3(objB.modelTrans * glm::vec4(objB.localCenter, 1.0f));

    // Extract axes from transformations
    glm::vec3 axesA[3] = {
        glm::normalize(glm::vec3(objA.modelTrans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f))),
        glm::normalize(glm::vec3(objA.modelTrans * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f))),
        glm::normalize(glm::vec3(objA.modelTrans * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)))
    };

    glm::vec3 axesB[3] = {
        glm::normalize(glm::vec3(objB.modelTrans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f))),
        glm::normalize(glm::vec3(objB.modelTrans * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f))),
        glm::normalize(glm::vec3(objB.modelTrans * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)))
    };

    glm::vec3 T = centerB - centerA; // Vector between centers
    glm::mat3 R, AbsR;

    const float EPSILON = 1e-6f;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            R[i][j] = glm::dot(axesA[i], axesB[j]);
            AbsR[i][j] = std::abs(R[i][j]) + EPSILON;
        }
    }

    // Test axes A[i]
    for (int i = 0; i < 3; ++i) {
        float ra = objA.halfExtents[i];
        float rb = objB.halfExtents.x * AbsR[i][0] + objB.halfExtents.y * AbsR[i][1] + objB.halfExtents.z * AbsR[i][2];
        if (std::abs(glm::dot(T, axesA[i])) > ra + rb) return false;
    }

    // Test axes B[j]
    for (int i = 0; i < 3; ++i) {
        float ra = objA.halfExtents.x * AbsR[0][i] + objA.halfExtents.y * AbsR[1][i] + objA.halfExtents.z * AbsR[2][i];
        float rb = objB.halfExtents[i];
        if (std::abs(glm::dot(T, axesB[i])) > ra + rb) return false;
    }

    // Test cross products of A[i] and B[j]
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            float ra = objA.halfExtents[(i + 1) % 3] * AbsR[(i + 2) % 3][j] +
                       objA.halfExtents[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
            float rb = objB.halfExtents[(j + 1) % 3] * AbsR[i][(j + 2) % 3] +
                       objB.halfExtents[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
            if (std::abs(T[i] * R[i][j]) > ra + rb) return false;
        }
    }

    return true; // No separating axis found, OBBs are colliding
}







void projectVerticesOntoAxis(const std::vector<glm::vec3>& vertices, const glm::vec3& axis, float& min, float& max) {
        min = max = glm::dot(vertices[0], axis);
        for (const auto& vertex : vertices) {
            float projection = glm::dot(vertex, axis);
            if (projection < min) min = projection;
            if (projection > max) max = projection;
        }
    }

    bool SATConvexHull(const Mesh& objA, const Mesh& objB) {
        std::vector<glm::vec3> axes;

        axes.insert(axes.end(), objA.faceNormals.begin(), objA.faceNormals.end());
        axes.insert(axes.end(), objB.faceNormals.begin(), objB.faceNormals.end());

        for (size_t i = 0; i < objA.convexHullVertices.size(); ++i) {
            glm::vec3 edgeA = objA.convexHullVertices[(i + 1) % objA.convexHullVertices.size()] - objA.convexHullVertices[i];
            for (size_t j = 0; j < objB.convexHullVertices.size(); ++j) {
                glm::vec3 edgeB = objB.convexHullVertices[(j + 1) % objB.convexHullVertices.size()] - objB.convexHullVertices[j];
                glm::vec3 axis = glm::normalize(glm::cross(edgeA, edgeB));
                if (glm::length(axis) > 1e-6f) {
                    axes.push_back(axis);
                }
            }
        }

        for (const auto& axis : axes) {
            float minA, maxA, minB, maxB;
            projectVerticesOntoAxis(objA.convexHullVertices, axis, minA, maxA);
            projectVerticesOntoAxis(objB.convexHullVertices, axis, minB, maxB);
            if (maxA < minB || maxB < minA) {
                return false;
            }
        }

        return true;
    }

    void initConvexHull(Mesh& obj) {
        obj.convexHullVertices = computeConvexHull(obj);

        obj.faceNormals.clear();
        for (size_t i = 0; i < obj.convexHullVertices.size(); i += 3) {
            glm::vec3 v0 = obj.convexHullVertices[i];
            glm::vec3 v1 = obj.convexHullVertices[i + 1];
            glm::vec3 v2 = obj.convexHullVertices[i + 2];
            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            obj.faceNormals.push_back(normal);
        }
    }

std::vector<glm::vec3> computeConvexHull(Mesh& obj) {
    std::vector<glm::vec3> convexHullVertices;

    // A simple placeholder convex hull implementation: you'll need a more advanced algorithm here
    // For now, assume that you are using a library or implementing an algorithm like Quickhull
    // The convex hull computation algorithm would go here
    
    // Example (this is not a real convex hull algorithm):
    convexHullVertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));  // Add some example points
    convexHullVertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
    convexHullVertices.push_back(glm::vec3(1.0f, -1.0f, 1.0f));
    convexHullVertices.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));

    // Real convex hull code should calculate the convex hull vertices based on obj.vertices.
    return convexHullVertices;
}

    bool convexHull(Mesh& objA, Mesh& objB) {
        initConvexHull(objA);
        initConvexHull(objB);
        return SATConvexHull(objA, objB);
    }

};

#endif
