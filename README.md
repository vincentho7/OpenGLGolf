# ⛳ OpenGL Golf 3D 
A mini-golf game inspired by Golf Gang, developed in C++ using OpenGL. 
This project features physics-based gameplay and procedurally generated environments.Developed by Maxime Boy-Arnould & Vincent Thong.

## Key Features
- Procedural Generation: Infinite map generation using Perlin Noise to build environments chunk by chunk.
- Physics Engine: Realistic ball movement and world interactions (Gravity, Collisions) powered by the Bullet Physics library.
- Dynamic Controls: Custom camera system and intuitive ball-striking mechanics.

## Requirements & Dependencies

Before compiling, ensure you have the following libraries installed:

| Library | Purpose | Link |
| :--- | :--- | :--- |
| **GLFW** | Window management & Inputs | [glfw.org](https://www.glfw.org/) |
| **GLM** | Mathematics (Vectors/Matrices) | [GitHub](https://github.com/g-truc/glm) |
| **Bullet 3** | Collision & Physics Engine | [GitHub](https://github.com/bulletphysics/bullet3) |


🚀 Getting Started
1. Clone the repository
``
git clone https://github.com/vincentho7/OpenGLGolf.git
cd OpenGLGolf
``
2. Compilation
The project includes a Makefile for easy compilation.
Standard Build:
``make
./main``

If your Bullet installation path differs from /usr/local/include/bullet, please update the path in the Makefile.

g++ -I./include src/* -I/usr/local/include/bullet -lBulletDynamics -lBulletCollision -lLinearMath -lglfw -o main
#👥 Authors
Maxime Boy-Arnould & Vincent Thong
