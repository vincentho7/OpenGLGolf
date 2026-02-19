# OPENGL GOLF PROJECT

Golf Project using C++ and OPENGL by Maxime Boy-arnould & Vincent Thong.

Mini game like Golf gang and Golf it using procedurally generated environment by building our map chunk by chunk.

## Requirements & Installation: 
GLFW
GLM : https://github.com/g-truc/glm
bullet : https://github.com/bulletphysics/bullet3

## Clone the repository
``git clone https://github.com/vincentho7/OpenGLGolf.git``
``make``
# Command (change the path of bullet if it doesn't match):  
g++ -I./include src/* -I/usr/local/include/bullet -lBulletDynamics -lBulletCollision -lLinearMath -lglfw -o main
