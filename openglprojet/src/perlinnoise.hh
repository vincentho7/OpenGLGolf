#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <GL/glad.h>
#include <glm/glm.hpp>
#include "../src/mesh.hh"
#include <stb_perlin.h>
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <bullet/btBulletDynamicsCommon.h>


const int MARGIN = 1; // La marge à ajouter de chaque côté
const int CHUNK_SIZE = 100; // taille d'un chunk
const float frequency = 0.07; // fréquence du bruit Perlin
typedef std::pair<int, int> ChunkCoords; // coordonnées d'un chunk

Mesh generate_mesh_from_noise(std::vector<std::vector<float>> pn, int width, int height) {
    auto perlinNoise = pn;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Vertex vertex;
            vertex.Position = glm::vec3(i, perlinNoise[i][j], j);
            // Calculate the tangent vectors and their cross product to get the normal
            float heightRight = (i < width - 1) ? perlinNoise[i+1][j] : perlinNoise[i][j];
            float heightUp = (j < height - 1) ? perlinNoise[i][j+1] : perlinNoise[i][j];
            glm::vec3 tangent1 = glm::vec3(1.0, heightRight - vertex.Position.y, 0.0);
            glm::vec3 tangent2 = glm::vec3(0.0, heightUp - vertex.Position.y, 1.0);
            vertex.Normal = glm::normalize(glm::cross(tangent1, tangent2));

            vertex.TexCoords = glm::vec2(i, j);
            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (int i = 0; i < width - 1; ++i) {
        for (int j = 0; j < height - 1; ++j) {
            indices.push_back(i * width + j);
            indices.push_back(i * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);

            indices.push_back(i * width + j);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j);
        }
    }

    // Generate mesh
    return Mesh(vertices, indices, std::vector<Texture>());
}


// Une fonction pour générer un chunk et convertir le bruit Perlin en un maillage (mesh)
Mesh generateChunk(int startX, int startY) {
    const int MARGIN = 1; // La marge à ajouter de chaque côté du chunk
    std::vector<std::vector<float>> chunk(CHUNK_SIZE + 2 * MARGIN, std::vector<float>(CHUNK_SIZE + 2 * MARGIN));

    for(int i = 0; i < CHUNK_SIZE + 2 * MARGIN; i++) {
        for(int j = 0; j < CHUNK_SIZE + 2 * MARGIN; j++) {
            float x = (float) (startX + i - MARGIN);
            float y = (float) (startY + j - MARGIN);
            chunk[i][j] = stb_perlin_noise3(x * frequency, y * frequency, 0.0, 0, 0, 0)*8;
        }
    }

    return generate_mesh_from_noise(chunk, CHUNK_SIZE + 2 * MARGIN, CHUNK_SIZE + 2 * MARGIN);
}




// Une fonction pour décharger un chunk
void unloadChunk(ChunkCoords coords, std::map<ChunkCoords, Mesh>& chunks) {
    chunks.erase(coords);
}


void updateChunks(int playerX, int playerY, std::map<ChunkCoords, Mesh>& chunks, btDiscreteDynamicsWorld* dynamicsWorld) {
    // Décharge les chunks trop éloignés du joueur...
    for(auto it = chunks.begin(); it != chunks.end();) {
        int distanceX = it->first.first * CHUNK_SIZE - playerX;
        int distanceY = it->first.second * CHUNK_SIZE - playerY;
        if(distanceX * distanceX + distanceY * distanceY > CHUNK_SIZE * CHUNK_SIZE * 4) {
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
    
    // Charge les chunks et les maillages de triangles autour du joueur...
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            ChunkCoords coords = std::make_pair((playerX + i * CHUNK_SIZE) / CHUNK_SIZE, (playerY + j * CHUNK_SIZE) / CHUNK_SIZE);
            if(chunks.count(coords) == 0) {
                Mesh newChunk = generateChunk(coords.first * CHUNK_SIZE, coords.second * CHUNK_SIZE);
                chunks[coords] = newChunk;

                btTriangleMesh* triangleMesh = new btTriangleMesh();
                for (size_t k = 0; k < newChunk.indices.size(); k += 3) {
                    glm::vec3 v0 = newChunk.vertices[newChunk.indices[k]].Position;
                    glm::vec3 v1 = newChunk.vertices[newChunk.indices[k + 1]].Position;
                    glm::vec3 v2 = newChunk.vertices[newChunk.indices[k + 2]].Position;

                    btVector3 btv0(v0.x, v0.y, v0.z);
                    btVector3 btv1(v1.x, v1.y, v1.z);
                    btVector3 btv2(v2.x, v2.y, v2.z);

                    triangleMesh->addTriangle(btv0, btv1, btv2);
                }

                // Utilisez le maillage de triangle pour créer une forme de collision
                btBvhTriangleMeshShape* groundShape = new btBvhTriangleMeshShape(triangleMesh, true);

                // Créez le corps rigide pour le terrain
                btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(coords.first * CHUNK_SIZE, -1, coords.second * CHUNK_SIZE)));
                btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
                btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
                dynamicsWorld->addRigidBody(groundRigidBody);
            }
        }
    }
}




#endif // PERLINNOISE_H