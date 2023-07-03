# poglfinal

POGL Golf Project - Maxime Boy-arnould , Vincent Thong

Requirements : 
    GLFW
    GLM : https://github.com/g-truc/glm
    bullet : https://github.com/bulletphysics/bullet3

Command (change the path of bullet if it doesn't match):  
g++ -I./include src/* -I/usr/local/include/bullet -lBulletDynamics -lBulletCollision -lLinearMath -lglfw -o main
Or
Make (same change path)