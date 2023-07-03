#include <GL/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <bullet/btBulletDynamicsCommon.h>

#include "program.hh"
#include "camera.hh"
#include "mesh.hh"
#include "perlinnoise.hh"
#include "water.hh"
#include <iostream>
#include <stdlib.h> 
float deg = -2.14;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void processInput(GLFWwindow *window, btRigidBody* ballRigidBody);

// settings
const unsigned int SCR_WIDTH =800;
const unsigned int SCR_HEIGHT = 600;
// g++ -I./include src/* -I/usr/include/bullet -lBulletDynamics -lBulletCollision -lLinearMath -lassimp -lglfw -o main
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // configure global opengl state
    glEnable(GL_DEPTH_TEST);


    // build and compile our shader program
    Program ourShader("shaders/vertex.glsl", "shaders/fragment.glsl"); // you can name your shader files however you like
    Program terrainShader("shaders/terrain_vert.glsl", "shaders/terrain_frag.glsl");
    Program ballShader("shaders/ball_vert.glsl", "shaders/ball_frag.glsl");
    Program waterShader("shaders/water_vertex.glsl", "shaders/water_frag.glsl");





    // world collision setting
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10.0, 0)); // Set gravity


    
    std::map<ChunkCoords, Mesh> chunks;
    updateChunks(0, 0, chunks, dynamicsWorld);

    std::map<WaterChunkCoords, Mesh> waterchunks;
    updateWaterChunks(0, 0, waterchunks);
    
    /*
    // Créez le triangle mesh
    btTriangleMesh* triangleMesh = new btTriangleMesh();

    // Ajoutez les triangles du mesh à la forme de collision
    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
        glm::vec3 v0 = mesh.vertices[mesh.indices[i]].Position;
        glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].Position;
        glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].Position;

        btVector3 btv0(v0.x, v0.y, v0.z);
        btVector3 btv1(v1.x, v1.y, v1.z);
        btVector3 btv2(v2.x, v2.y, v2.z);

        triangleMesh->addTriangle(btv0, btv1, btv2);
    }
    

    // Utilisez le maillage de triangle pour créer une forme de collision
    btBvhTriangleMeshShape* groundShape = new btBvhTriangleMeshShape(triangleMesh, true);

    // Créez le corps rigide pour le terrain comme montré précédemment
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
    */


    

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("texture/golf.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    

    //Model ourModel("models/ball2.obj");
    Mesh sphere = generateSphere(50, 50);
    btCollisionShape* ballShape = new btSphereShape(0.10); // 0.1 est le rayon de la sphère
    btDefaultMotionState* ballMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(10, 10, 10))); // Position initiale
    btScalar mass = 1;
    btVector3 ballInertia(0, 0, 0);
    ballShape->calculateLocalInertia(mass, ballInertia);
    btRigidBody::btRigidBodyConstructionInfo ballRigidBodyCI(mass, ballMotionState, ballShape, ballInertia);
    btRigidBody* ballRigidBody = new btRigidBody(ballRigidBodyCI);
    // Enable CCD pour pas que la balle traverse tout =)
    ballRigidBody->setCcdMotionThreshold(0.1);
    ballRigidBody->setCcdSweptSphereRadius(0.1);
    dynamicsWorld->addRigidBody(ballRigidBody);






    // render loop
 
    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window,ballRigidBody);

        // render
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //bind texture
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, texture);




        // Simulate the physics world
        dynamicsWorld->stepSimulation(1 / 60.f, 10);

        // Get the transform of the ball rigid body
        btTransform trans;
        ballRigidBody->getMotionState()->getWorldTransform(trans);

        // Convert to glm::vec3 for easier manipulation in OpenGL
        glm::vec3 ballPos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());




        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        
        
        ourShader.setMat4("model", model);
        


        // render box
        //glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0);


        //terrain.draw(terrainShader,view,projection);

        ballShader.use();
        ballShader.setMat4("projection", projection);
        ballShader.setMat4("view", view);
        glm::mat4 model3         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        
        model3 = glm::translate(model3, ballPos); // Translate the ball model to the physics world position
        model3 = glm::scale(model3, glm::vec3(0.1f, 0.1f, 0.1f)); // And scale it
        ballShader.setMat4("model", model3);
        sphere.Draw(ballShader);
        

        // Utiliser le shader de terrain
    terrainShader.use();

    // Définir les matrices de projection et de vue
    terrainShader.setMat4("projection", projection);
    terrainShader.setMat4("view", view);

    // Mettre à jour les chunks autour du joueur
    updateChunks(ballPos.x, ballPos.z, chunks,dynamicsWorld);

    // Dessiner chaque chunk
    for(auto& chunk : chunks) {
        // Configurer la matrice de modèle pour le chunk actuel
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(chunk.first.first * 100, -0.9, chunk.first.second * 100));
        terrainShader.setMat4("model", model);
    
        // Dessiner le chunk
        chunk.second.Draw(terrainShader);
    }
    

    // Utiliser le shader de l'eau
    waterShader.use();

    // Définir la position de la caméra
    waterShader.setVec3("viewPos", camera.Position);
    waterShader.setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
    waterShader.setVec3("lightPos", camera.Position + glm::vec3(0,0,0));

    // Définir les matrices de projection et de vue
    waterShader.setMat4("projection", projection);
    waterShader.setMat4("view", view);

    // Mettre à jour les chunks d'eau autour du joueur
    updateWaterChunks(ballPos.x, ballPos.z, waterchunks);

    // Dessiner chaque chunk d'eau
    updateWater(deltaTime,waterchunks);
    for(auto& chunk : waterchunks) {
        // Configurer la matrice de modèle pour le chunk actuel
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(chunk.first.first * 100, 0, chunk.first.second * 100));
        waterShader.setMat4("model", model);
    
        // Dessiner le chunk d'eau
        chunk.second.Draw(waterShader);
    }
        glm::vec3 campos = glm::vec3(trans.getOrigin().getX() + cos(deg) * 5, trans.getOrigin().getY()+4, trans.getOrigin().getZ() + sin(deg) * 5);

        camera.Position= campos;        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }   

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window, btRigidBody* ballRigidBody)
{
    
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        deg = deg + 0.03;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        deg = deg - 0.03;

    
    // Project the ball in a random direction with random force when 'P' is pressed
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        // Check if the ball is sleeping
        if (!ballRigidBody->isActive()) {
            // Wake up the ball
            ballRigidBody->activate(true);
        }

        // Calculate the ball and camera positions
        btTransform trans;
        ballRigidBody->getMotionState()->getWorldTransform(trans);
        btVector3 ballPos = trans.getOrigin();
        btVector3 cameraPos =btVector3(camera.Position.x,camera.Position.y,camera.Position.z); // Replace with your code to get the camera position.

        // Calculate the direction vector from the camera to the ball
        btVector3 direction = ballPos - cameraPos;
        direction.setY((rand() % 200 ) / 200.0); // Set the Y component to a random value
        direction.normalize();

        // Generate a random force between 1 and 10
        float impulse = (rand() % 1000 + 1000) / 500.0;

        // Apply the impulse
        ballRigidBody->applyCentralImpulse(direction * impulse);
    }
    
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // car y de haut en bas

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}