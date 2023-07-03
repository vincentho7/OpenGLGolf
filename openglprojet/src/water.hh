#include <GL/glad.h>
#include <glm/glm.hpp>
#include "mesh.hh"
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <bullet/btBulletDynamicsCommon.h>

const int WMARGIN = 1; // La marge à ajouter de chaque côté
const int WCHUNK_SIZE = 100; // taille d'un chunk
typedef std::pair<int, int> WaterChunkCoords; // coordonnées d'un chunk

void updateWater(float deltaTime, std::map<WaterChunkCoords, Mesh>& chunks) {
    static float time = 0;
    time += deltaTime;

    for (auto& [coords, mesh] : chunks) {
        for (auto& vertex : mesh.vertices) {
            float waveHeight = sin(vertex.Position.x + time) * 0.1f;
            vertex.Position.y = -4.0f + waveHeight;
        }

        mesh.updateVertices();  // You'll need to implement this yourself
    }
}

Mesh generateWaterMesh(int width, int height) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Vertex vertex;
            vertex.Position = glm::vec3(i, -4.0f, j);
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);  // Upwards-facing normal
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

Mesh generateWaterChunk(int startX, int startY) {
    const int MARGIN = 1; // La marge à ajouter de chaque côté du chunk
    std::vector<std::vector<float>> chunk(WCHUNK_SIZE + 2 * MARGIN, std::vector<float>(WCHUNK_SIZE + 2 * MARGIN));

    for(int i = 0; i < WCHUNK_SIZE + 2 * MARGIN; i++) {
        for(int j = 0; j < WCHUNK_SIZE + 2 * MARGIN; j++) {
            float x = (float) (startX + i - MARGIN);
            float y = (float) (startY + j - MARGIN);
        }
    }

    return generateWaterMesh(WCHUNK_SIZE + 2 * MARGIN, WCHUNK_SIZE + 2 * MARGIN);
}

// Une fonction pour décharger un chunk
void unloadWaterChunk(WaterChunkCoords coords, std::map<WaterChunkCoords, Mesh>& chunks) {
    chunks.erase(coords);
}

void updateWaterChunks(int playerX, int playerY, std::map<WaterChunkCoords, Mesh>& chunks) {
    // Décharge les chunks trop éloignés du joueur...
    for(auto it = chunks.begin(); it != chunks.end();) {
        int distanceX = it->first.first * WCHUNK_SIZE - playerX;
        int distanceY = it->first.second * WCHUNK_SIZE - playerY;
        if(distanceX * distanceX + distanceY * distanceY > WCHUNK_SIZE * WCHUNK_SIZE * 4) {
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
    
    // Charge les chunks et les maillages de triangles autour du joueur...
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            WaterChunkCoords coords = std::make_pair((playerX + i * WCHUNK_SIZE) / WCHUNK_SIZE, (playerY + j * WCHUNK_SIZE) / WCHUNK_SIZE);
            if(chunks.count(coords) == 0) {
                Mesh newChunk = generateWaterChunk(coords.first * WCHUNK_SIZE, coords.second * WCHUNK_SIZE);
                
                chunks[coords] = newChunk;
            }
        }
    }
}
