#include "Terrain.h"
#include <vector>
#include "Renderer.h"
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <functional>

Terrain::Terrain(int width, int height)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    generateTerrain(width, height, vertices, indices);

    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(3); // Position
    layout.Push<float>(3); // Normal
    layout.Push<float>(2); // Texture
    layout.Push<float>(3); // Color

    m_VAO = std::make_shared<VertexArray>();
    m_VBO = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(float));
    m_VAO->AddBuffer(*m_VBO, layout);
    m_IBO = std::make_shared<IndexBuffer>(&indices[0], indices.size());

    m_Shader = std::make_shared<Shader>("res/shaders/Plane.shader");
    m_Shader->Bind();

    m_VAO->UnBind();
    m_VBO->UnBind();
    m_IBO->UnBind();
    m_Shader->UnBind();
}

Terrain::~Terrain()
{
}

void Terrain::Render(glm::mat4 MVP) {
    Renderer renderer;
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP", MVP);
    renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
}

// Steps to generate terrain
// 1. Heightmap -> use layered perlin noise to create vertex positions
// 2. Road(s) 
//      -> use modified A* to generate a path through the map
//      -> generate splines by taking every Nth node 
//      -> flatten a widened road area following the spline
// 3. Normals -> calculate normals using updated vertex positions
// 4. Coloring -> color based off: road, height, gradient (normal)
void Terrain::generateTerrain(int widthSegments, int heightSegments, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    
    float seg_width = 1.0f / widthSegments;
    float seg_height = 1.0f / heightSegments;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> colors;

    vertices.reserve((3 + 3 + 2 + 3) * (widthSegments + 1) * (heightSegments + 1));
    positions.reserve((widthSegments + 1) * (heightSegments + 1));
    normals.reserve((widthSegments + 1) * (heightSegments + 1));
    textures.reserve((widthSegments + 1) * (heightSegments + 1));
    colors.reserve((widthSegments + 1) * (heightSegments + 1));

    std::function noise = [](float x, float y) {
        float z = glm::perlin(glm::vec2(x, y));
        return (z + 1.0f) / 2.0f;
    };

    // Positions
    for (int i = 0; i < heightSegments + 1; i++) {
        float y = i * seg_height - 0.5f;

        for (int j = 0; j < widthSegments + 1; j++) {
            float x = j * seg_width - 0.5f;

            float frequency = 4.0f;
            float wavelength = 1.0f / frequency;
            float e =    1 * noise(1 * x / wavelength, 1 * y / wavelength)
                    +  0.5 * noise(2 * x / wavelength, 2 * y / wavelength)
				    + 0.25 * noise(4 * x / wavelength, 4 * y / wavelength);
            e = e / (1 + 0.5 + 0.25);
            float z = glm::pow(e, 1.6f);

            positions.push_back({ x, y, z });


            // Texture
            textures.push_back({ j * seg_width, i * seg_height });
        }
    }

    // TODO: Generate roads
    // generateRoads();

    // Normals
    for (int i = 0; i < heightSegments + 1; i++) {
        for (int j = 0; j < widthSegments + 1; j++) {
            float y = i * seg_height - 0.5f;
            float x = j * seg_width - 0.5f;

            float right;
            if (j == widthSegments) {
				right = positions[i * (widthSegments + 1) + j].z;
			}
            else {
				right = positions[i * (widthSegments + 1) + j + 1].z;
			}
            float left;
            if (j == 0) {
                left = positions[i * (widthSegments + 1) + j].z;
            }
            else {
				left = positions[i * (widthSegments + 1) + j - 1].z;
			}
            float up;
            if (i == heightSegments) {
                up = positions[i * (widthSegments + 1) + j].z;
            }
            else {
                up = positions[(i + 1) * (widthSegments + 1) + j].z;
            }
            float down;
            if (i == 0) {
				down = positions[i * (widthSegments + 1) + j].z;
			}
            else {
				down = positions[(i - 1) * (widthSegments + 1) + j].z;
			}

            glm::vec3 normal = glm::normalize(glm::vec3(left - right, down - up, 2.0f));
            normals.push_back(normal);
        }
    }

    // Colors
    // TODO: Roads and normals
    for (int i = 0; i < heightSegments; i++) {
        for (int j = 0; j < widthSegments; j++) {
			int index = i * (widthSegments + 1) + j;
			glm::vec3 color = glm::vec3(1.0f, glm::clamp(positions[index].z, 0.0f, 1.0f), 1.0f);
			colors.push_back(color);

            //        // Color
    //        glm::vec3 color /*= glm::vec3(1.0f, glm::clamp(e, 0.0f, 1.0f), 1.0f)*/;
    //        switch (biome(z)) {
    //            case WATER:
                //	color = glm::vec3(0.0f, 0.0f, 1.0f);
                //    break;
    //            case SAND:
    //                color = glm::vec3(1.0f, 1.0f, 0.0f);
    //                break;
    //            case GRASS:
    //                color = glm::vec3(0.0f, 1.0f, 0.0f);
                //	break;
                //case ROCK:
                //	color = glm::vec3(0.5f, 0.5f, 0.5f);
                //	break;
                //case SNOW:
                //	color = glm::vec3(1.0f, 1.0f, 1.0f);
                //	break;
    //            default:
    //                color = glm::vec3(1.0f, 0.0f, 1.0f);
                //	break;
    //        }
    //        vertices.push_back(color.r);
    //        vertices.push_back(color.g);
    //        vertices.push_back(color.b);
		}
    }

    // Indices
    for (int i = 0; i < heightSegments; i++) {
        for (int j = 0; j < widthSegments; j++) {
            unsigned int a = i * (widthSegments + 1) + (j + 1);
            unsigned int b = i * (widthSegments + 1) + j;
            unsigned int c = (i + 1) * (widthSegments + 1) + j;
            unsigned int d = (i + 1) * (widthSegments + 1) + (j + 1);

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(c);
            indices.push_back(d);
            indices.push_back(a);
        }
    }


    // Vertices
    for (int i = 0; i < heightSegments; i++) {
        for (int j = 0; j < widthSegments; j++) {
            int index = i * (widthSegments + 1) + j;

            // Position
            vertices.push_back(positions[index].x);
            vertices.push_back(positions[index].y);
            vertices.push_back(positions[index].z);

            // Normal
            vertices.push_back(normals[index].x);
            vertices.push_back(normals[index].y);
            vertices.push_back(normals[index].z);

            // Texture
            vertices.push_back(textures[index].x);
            vertices.push_back(textures[index].y);

            // Color
            vertices.push_back(colors[index].r);
            vertices.push_back(colors[index].g);
            vertices.push_back(colors[index].b);
        }
    }
}