#ifndef MESH_H
#define MESH_H

#include <GL/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "program.hh"
#include <string>
#include <vector>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex { 
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {

public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    Mesh() = default;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    void Draw(Program &shader);
    void updateVertices();
private:
    unsigned int VBO, EBO;

    void setupMesh();
};

inline Mesh generateSphere(unsigned int numSlices, unsigned int numStacks) {
    float PI = 3.1415;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // Empty textures for this example

    for (unsigned int j = 0; j <= numStacks; ++j) {
        for (unsigned int i = 0; i <= numSlices; ++i) {
            float x = cos((float)i / numSlices * 2.0f * PI) * sin((float)j / numStacks * PI);
            float y = cos((float)j / numStacks * PI) + 1.0f; // Translation by (0,1,0)
            float z = sin((float)i / numSlices * 2.0f * PI) * sin((float)j / numStacks * PI);

            Vertex vertex;
            vertex.Position = glm::vec3(x, y, z);
            vertex.Normal = glm::normalize(glm::vec3(x, y - 1.0f, z)); // Normal vector
            vertex.TexCoords = glm::vec2((float)i / numSlices, (float)j / numStacks); // Texture coordinates
            vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f); // Tangent vector (needs to be calculated)
            vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f); // Bitangent vector (needs to be calculated)

            for(int k=0; k < MAX_BONE_INFLUENCE; k++) {
                vertex.m_BoneIDs[k] = 0;
                vertex.m_Weights[k] = 0;
            }

            vertices.push_back(vertex);

            if (i != 0 && j != 0) {
                int a = (numSlices + 1) * j + i;
                int b = (numSlices + 1) * j + i - 1;
                int c = (numSlices + 1) * (j - 1) + i;
                int d = (numSlices + 1) * (j - 1) + i - 1;

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);

                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }
    }

    return Mesh(vertices, indices, textures);
}

#endif