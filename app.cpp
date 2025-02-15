//RC: g++ -g -o /home/paul/NeoVimProjects/temp/temp app.cpp glad/glad.c -I./glad -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp -lopenal
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include <AL/al.h>
#include <AL/alc.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
//#include "include/filesystem.h"

#include "shader.h"
#include "model.h"
#include "collision.h"
#include "camera.h"
#include "ui.h"
#include "audio/audio.h"


#include <iostream>
#include <unordered_map>
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>


Audio* audio = nullptr;
Speaker speaker;
Sound sound;

std::vector<ALuint> speakerList;
std::vector<ALuint> soundList;

std::vector<Model> modelsList;

Model* playerModel = nullptr;
Camera* camera = nullptr;

struct Cube {
    std::array<float, 3> location; // Center of the cube
    std::array<float, 3> scaleAmount; // Side length of the cube

     //Constructor to initialize location and scaleAmount
    Cube(const std::array<float, 3>& loc, const std::array<float, 3>& scale)
        : location(loc), scaleAmount(scale) {}
};

std::vector<struct Cube> cubeObjectList;
bool playerMove(glm::vec3 movement);

std::array<float, 120> makeRectangleEBO(const std::array<float, 3> location, const std::array<float, 3> dimensions);
void genRectangleEBO(const std::array<float, 120> vertices, const std::array<float, 3> location, const std::array<float, 3> scaleAmmount = {1, 1, 1});
bool checkCollision(const Cube& cubeA, const Cube& cubeB);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void init();
int loadTexture(std::string name, std::string pathToImg);
Shader* shader = nullptr;
unsigned int textures[6];

void stepPlayerPhysics(float step, float momentum, std::vector<std::vector<std::vector<std::vector<float>>>>& world);

std::mt19937& getRNG(int seed);

std::unordered_map<std::string, int> keySwitches;
std::unordered_map<std::string, bool> keyHeld;

std::atomic<bool> is_thread_running(false); // Atomic flag to track if a thread is running

const unsigned int SCR_WIDTH = 845;
const unsigned int SCR_HEIGHT = 480;

GLFWmonitor* monitor;
const GLFWvidmode* mode;

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
                        //
bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float roll =     0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  90.0f;

float playerVelocityY = 0.0f;

int main()
{
    init();
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window Title", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    camera = new Camera(window);

    // build and compile our shader program
    // ------------------------------------
    //Shader shader("shaders/shader.vs", "shaders/shader.fs");
    shader = new Shader("shaders/shader.vs", "shaders/shader.fs");

    //Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    //Model ourModel("/home/paul/NeoVimProjects/tacticalShooter/blender/randomLineOBB.obj");//FileSystem::getPath("blender/map1.blend"));
    //Model ourModel("/home/paul/NeoVimProjects/tacticalShooter/blender/backpack/backpack.obj");//FileSystem::getPath("blender/map1.blend"));
    //Model ourModel("FileSystem::getPath("blender/map1.blend"));
    
    playerModel = new Model("/home/paul/NeoVimProjects/tacticalShooter/blender/cube.obj");
    //modelsList.push_back(Model("/home/paul/NeoVimProjects/tacticalShooter/blender/randomLineOBB.obj"));
    modelsList.push_back(Model("/home/paul/NeoVimProjects/tacticalShooter/blender/dust2.obj"));
    modelsList.push_back(Model("/home/paul/NeoVimProjects/tacticalShooter/blender/backpack/backpack.obj"));
    //modelsList.push_back(Model("/home/paul/NeoVimProjects/tacticalShooter/blender/dust2.obj"));
    modelsList.push_back(Model("/home/paul/NeoVimProjects/tacticalShooter/blender/UItest.obj"));
    modelsList.push_back(Model("/home/paul/NeoVimProjects/tacticalShooter/blender/dust2Floor.obj"));


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    unsigned int VAO, VBO, EBO; //Vertex buffer object; Vertex array object, elemnt buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    //std::vector<std::vector<std::vector<std::vector<float>>>> world;
    //createWorld(world, {100, 100}, {-100, -100});
    //world world = new world({100, 100}, {-100, -100});

    //world* world = new world({100, 100}, {-100, -100});


    std::array<float, 120> block = makeRectangleEBO({-1.0f, 1.0f, 1.0f}, {2.0f, 2.0f, 2.0f});
    glBufferData(GL_ARRAY_BUFFER, block.size() * sizeof(float), block.data(), GL_STATIC_DRAW);

    unsigned int indices[] = {
      // Face 1
      0, 1, 2,  0, 2, 3,
      // Face 2
      4, 5, 6,  4, 6, 7,
      // Face 3
      8, 9, 10, 8, 10, 11,
      // Face 4
      12, 13, 14, 12, 14, 15,
      // Face 5
      16, 17, 18, 16, 18, 19,
      // Face 6
      20, 21, 22, 20, 22, 23
    };

//unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO); 

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, block.size() * sizeof(float), block.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);



       
  cubeObjectList.push_back({ Cube({-30, 4, 10}, {5, 7, 5}) });
  cubeObjectList.push_back({ Cube({-45, -2, 20}, {1, 1, 1}) });
  //cubeObjectList.push_back({ Cube({0, -8, 0}, {500, 5, 500}) });


  Ui ui(camera);
  ui.addElements(modelsList[2], {0, 0}, 10);


    audio = new Audio();

    speaker = audio->createSpeaker(10);
    sound = audio->loadFile2("audio/files/smallCrash.wav");

    for (int i = 0; i < 60; i++) speakerList.push_back(audio->createSpeaker());
    soundList.push_back(audio->genBuffer("audio/files/smallCrashMono.wav"));


    // render loop
    // ----------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        // render
        // ------
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClearColor(0.529f, 0.808f, 0.9254f, 1.0f); //rgba(122,168,254,255)
        glClearColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);
        //glClearColor(50.0f / 255.0f, 85.0f / 255.0f, 24.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);


        //Camera

        //transformations
        glm::mat4 model = glm::mat4(1.0f);

        camera->Rotation = glm::vec3(yaw, pitch, roll);
        camera->Fov = 120;
        camera->updateCameraView();

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        shader->setMat4("projection", projection);
        shader->setMat4("model", model);
        shader->setMat4("view", camera->getViewMatrix());

        // render the meshes
        shader->use();

        glBindVertexArray(VAO);

        
        // Set uniform values
        shader->setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f)); // Red color  
        shader->setVec3("outlineColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Black outline color  
        shader->setFloat("outlineThickness", 0.00f);
        shader->setFloat("outlineOften", 0.5f);



 
        for (int i = 0; i < cubeObjectList.size(); i++) {
          genRectangleEBO(block, cubeObjectList[i].location, cubeObjectList[i].scaleAmount);
        }
        
        modelsList[3].Draw(*shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 5.0f, 5.0f));
        modelsList[1].Draw(*shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        //modelsList[0].updateTransformations(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

        playerModel->Draw(*shader, glm::vec3(camera->Position.x, camera->Position.y-1.5, camera->Position.z+4), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(camera->Rotation.y, camera->Rotation.x, camera->Rotation.z));

        ui.draw(*shader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// Function to create the world
void init(){
    keySwitches["F11"] = 0;keySwitches["ESCAPE"] = 0;keySwitches["M"] = 2;
    keyHeld["F11"] = false;keyHeld["ESCAPE"] = false;keyHeld["M"] = false;
}

bool checkCollision(const Cube& cubeA, const Cube& cubeB) {
    // Check overlap in x, y, and z axes
    bool overlapX = (cubeA.location[0] + cubeA.scaleAmount[0] > cubeB.location[0] - cubeB.scaleAmount[0]) &&
                    (cubeA.location[0] - cubeA.scaleAmount[0] < cubeB.location[0] + cubeB.scaleAmount[0]);

    bool overlapY = (cubeA.location[1] + cubeA.scaleAmount[1] > cubeB.location[1] - cubeB.scaleAmount[1]) &&
                    (cubeA.location[1] - cubeA.scaleAmount[1] < cubeB.location[1] + cubeB.scaleAmount[1]);

    bool overlapZ = (cubeA.location[2] + cubeA.scaleAmount[2] > cubeB.location[2] - cubeB.scaleAmount[2]) &&
                    (cubeA.location[2] - cubeA.scaleAmount[2] < cubeB.location[2] + cubeB.scaleAmount[2]);

    // Return true if there is overlap in all three axes
    if (overlapX && overlapY && overlapZ && false) {
        //std::thread t1(playAudio);
        //std::thread t1(sound->play(speakerList[0], soundList[0]));
        std::thread t1([=]() {
            for (int i = 0; i < 60; i++) {
                if (audio->play(speakerList[i], soundList[0]))
                    break;
            }
        });
        t1.detach();
        std::cout << "test" << std::endl;
    }

    if (overlapX && overlapY && overlapZ) {
        //std::thread t1([=](){audio->play(speaker, sound);});
        std::thread t1([=]() {
                audio->play(speaker, sound);
        });
        t1.detach();
    }
    return overlapX && overlapY && overlapZ;
}

bool playerMove(glm::vec3 movement) {

  Cube playerBox({static_cast<float>(camera->Position.x), static_cast<float>(camera->Position.y), static_cast<float>(camera->Position.z)}, {1, 1, 1});

  const glm::vec3 ogMovement = movement;
  
  
  playerBox.location[0] += movement.x; //apply movement to player
  for (int i = 0; i < cubeObjectList.size(); i++) { // loop for each collision object
    if (checkCollision(cubeObjectList[i], playerBox)) { movement.x = 0; break; }} //test if movement leads to any colision, if so, block the movement
  playerBox.location[0] -= ogMovement.x; // remove the movement to test other axis

  playerBox.location[1] += movement.y;
  for (int i = 0; i < cubeObjectList.size(); i++) {
    if (checkCollision(cubeObjectList[i], playerBox)) { movement.y = 0; break; }}
  playerBox.location[1] -= ogMovement.y;

  playerBox.location[2] += movement.z;
  for (int i = 0; i < cubeObjectList.size(); i++) {
    if (checkCollision(cubeObjectList[i], playerBox)) { movement.z = 0; break; }}
  playerBox.location[2] -= ogMovement.z;

  
  //   Collision.h tests for models and no longer structs
  //
  Collision clsn;
  
  camera->Position.x += ogMovement.x;
  playerModel->updateTransformations(glm::vec3(camera->Position.x, camera->Position.y-1.5, camera->Position.z+4.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  if (clsn.checkCollision(modelsList[3], *playerModel, 1)) { movement.x = 0;}
  camera->Position.x -= ogMovement.x;

  camera->Position.y += ogMovement.y;
  playerModel->updateTransformations(glm::vec3(camera->Position.x, camera->Position.y-1.5, camera->Position.z+4.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  if (clsn.checkCollision(modelsList[3], *playerModel, 1)) { movement.y = 0;}
  camera->Position.y -= ogMovement.y;

  camera->Position.z += ogMovement.z;
  playerModel->updateTransformations(glm::vec3(camera->Position.x, camera->Position.y-1.5, camera->Position.z+4.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  if (clsn.checkCollision(modelsList[3], *playerModel, 1)) { movement.z = 0;}
  camera->Position.z -= ogMovement.z;


  //   ##################################################



  camera->Position += movement;
  if (movement.x != ogMovement.x || movement.y != ogMovement.y || movement.z != ogMovement.z) return true;
  
  return false;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, false);

    float globalSpeedMultiplier = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      globalSpeedMultiplier = 25;

    float cameraSpeed = 12.5f * deltaTime * globalSpeedMultiplier;
    float verticalSpeed = 15.0f * deltaTime * globalSpeedMultiplier;

    float gravity = -9.81f/20;
    float jumpHeight = 10.0f/40;


    bool isOnGround = false;
    if (keySwitches["M"] == 1) {
      Cube playerBox({static_cast<float>(camera->Position.x), static_cast<float>(camera->Position.y), static_cast<float>(camera->Position.z)}, {1, 1, 1});
      playerBox.location[1] += -verticalSpeed * deltaTime; //apply movement to player
      if (checkCollision(cubeObjectList[0], playerBox) || checkCollision(cubeObjectList[1], playerBox) || checkCollision(cubeObjectList[2], playerBox)) {
        playerVelocityY = 0;
      } else {
        isOnGround = false;
        if (playerMove(glm::vec3(0.0f, playerVelocityY, 0.0f))) isOnGround = true;
        playerVelocityY += gravity *deltaTime;
      }
      playerBox.location[1] += verticalSpeed * deltaTime*50; //apply movement to player
    }
    
    
    

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
      playerMove(glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
      playerMove(-glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
      playerMove(-glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
      playerMove(glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed);}
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      playerMove(-glm::vec3(0.0f, verticalSpeed, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      if (isOnGround && keySwitches["M"] == 1) playerVelocityY = jumpHeight;
      if (keySwitches["M"]!= 1) camera->Position += glm::vec3(0.0f, verticalSpeed, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
      std::cout << std::round(camera->Position.x) << ", " << std::round(camera->Position.y) << ", " << std::round(camera->Position.z) << std::endl;



    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keyHeld["M"]) {
      keyHeld["M"] = true;
      keySwitches["M"] += 1;
      if (keySwitches["M"] > 2) keySwitches["M"] = 0;
    } else if (glfwGetKey(window, GLFW_KEY_M) != GLFW_PRESS) {
      keyHeld["M"] = false;
    }


    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !keyHeld["F11"]) {
      if (keySwitches["F11"] != 1) {
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
      keySwitches["F11"]= 1;
      keyHeld["F11"] = true;
    } else {
      glfwSetWindowMonitor(window, NULL, 400, 300, 800, 600, 0);
      keySwitches["F11"] = 0;
    }keyHeld["F11"] = true;
    }else if(glfwGetKey(window, GLFW_KEY_F11) != GLFW_PRESS){keyHeld["F11"] = false;}

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !keyHeld["ESCAPE"]) {
      if (!keySwitches["ESCAPE"]) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        keySwitches["ESCAPE"] = 1;
      } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        keySwitches["ESCAPE"] = 0;
      }
      keyHeld["ESCAPE"] = true;
    } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {keyHeld["ESCAPE"] = false;}


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (keySwitches["ESCAPE"] == 1) {firstMouse=true; return;}
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;
  float sensitivity = 0.05f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;
  yaw += xoffset;
  pitch += yoffset;
  if(pitch > 89.0f)
    pitch = 89.0f;
  if(pitch < -89.0f)
    pitch = -89.0f;
  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
fov -= (float)yoffset*5;
if (fov < 1.0f)
  fov = 1.0f;
if (fov > 100.0f)
  fov = 100.0f;
}



void genRectangleEBO(const std::array<float, 120> vertices, const std::array<float, 3> location, const std::array<float, 3> scaleAmmount) {

  glm::mat4 model = glm::mat4(1.0f);
  glm::vec3 translation(location[0], location[1], location[2]+3);
  model = glm::translate(model, translation);
  model = glm::scale(model, glm::vec3(scaleAmmount[0], scaleAmmount[1], scaleAmmount[2]));

  shader->setMat4("model", model);
  //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  
    for (int i = 0; i < 6; ++i) {
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int))); // Draw the face
    }
    //glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //glDeleteBuffers(1, &EBO);
}

std::array<float, 120> makeRectangleEBO(const std::array<float, 3> location, const std::array<float, 3> dimensions) {
    float corners[] = { 
        location[0], location[1], location[2],                                                 // top-left front
        location[0], location[1], location[2] - dimensions[2],                                 // top-left back
        location[0] + dimensions[0], location[1], location[2] - dimensions[2],                 // top-right back
        location[0] + dimensions[0], location[1], location[2],                                 // top-right front
        location[0], location[1] - dimensions[1], location[2],                                 // bottom-left front
        location[0], location[1] - dimensions[1], location[2] - dimensions[2],                 // bottom-left back
        location[0] + dimensions[0], location[1] - dimensions[1], location[2] - dimensions[2], // bottom-right back
        location[0] + dimensions[0], location[1] - dimensions[1], location[2]                  // bottom-right front
    };

    float textureCorners[] = {
      // Top Face
      0.0f, 0.0f,  // Bottom left
      1.0f, 0.0f,  // Bottom right
      1.0f, 1.0f,  // Top right
      0.0f, 1.0f,  // Top left

      // Back Face (correct orientation)
      1.0f, 1.0f,  // Bottom left (top of texture)
      1.0f, 0.0f,  // Bottom right (top of texture)
      0.0f, 0.0f,  // Top right (bottom of texture)
      0.0f, 1.0f,  // Top left (bottom of texture)

      // Bottom Face
      0.0f, 1.0f,  // Bottom left
      1.0f, 1.0f,  // Bottom right
      1.0f, 0.0f,  // Top right
      0.0f, 0.0f,  // Top left

      // Front Face (90 degrees clockwise)
      0.0f, 0.0f,  // Bottom left
      0.0f, 1.0f,  // Bottom right
      1.0f, 1.0f,  // Top right
      1.0f, 0.0f,  // Top left

      // Left Face
      0.0f, 0.0f,  // Bottom left
      1.0f, 0.0f,  // Bottom right
      1.0f, 1.0f,  // Top right
      0.0f, 1.0f,  // Top left

      // Right Face (correct orientation)
      0.0f, 1.0f,  // Bottom left (top of texture)
      1.0f, 1.0f,  // Bottom right (top of texture)
      1.0f, 0.0f,  // Top right (bottom of texture)
      0.0f, 0.0f   // Top left (bottom of texture)
      };


    std::array<float, 120> vertices = {};
    int face_indices[] = {
        0, 1, 2, 3, //top
        1, 5, 6, 2, //back
        7, 6, 5, 4, //bottom
        4, 0, 3, 7, //front
        4, 5, 1, 0, //left
        3, 2, 6, 7  //right
    };

    int j = 0;
    int ti = 0;
    for (int i = 0; i < 24; i++) {
        int index = face_indices[i];
        vertices[j] = corners[index * 3];
        vertices[j + 1] = corners[index * 3 + 1];
        vertices[j + 2] = corners[index * 3 + 2];

          vertices[j + 3] = textureCorners[ti];//[(ti % 4) * 2]; // % 4 for texture coordinates
          vertices[j + 4] = textureCorners[ti+1];//[(ti % 4) * 2 + 1]; // % 4 for texture coordinates

        j += 5;
        ti += 2;
        
    }
    return vertices;
}
