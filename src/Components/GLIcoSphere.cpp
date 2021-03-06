#include "GLIcoSphere.h"
#include "GL/glew.h"
#include <map>
#include <stdint.h>

GLIcoSphere::GLIcoSphere( const int entityID /*= 0*/ ) : IGLComponent(entityID) {
	this->drawMode = GL_TRIANGLES;
	this->ElemBufIndex = 2;
	this->ColorBufIndex = 1;
	this->VertBufIndex = 0;
}

void GLIcoSphere::Initialize() {
	srand(this->GetEntityID());
	// Create the verts to begin refining at.
	double t = (1.0 + glm::sqrt(5.0)) / 2.0;
	glm::vec2 coordPair = glm::normalize(glm::vec2(1,t));

	this->verts.push_back(vertex(-coordPair.r, coordPair.g, 0));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(coordPair.r, coordPair.g, 0));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(-coordPair.r, -coordPair.g, 0));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(coordPair.r, -coordPair.g, 0));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));

	this->verts.push_back(vertex(0, -coordPair.r, coordPair.g));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(0, coordPair.r, coordPair.g));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(0, -coordPair.r, -coordPair.g));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(0, coordPair.r, -coordPair.g));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));

	this->verts.push_back(vertex(coordPair.g, 0, -coordPair.r));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(coordPair.g, 0, coordPair.r));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(-coordPair.g, 0, -coordPair.r));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));
	this->verts.push_back(vertex(-coordPair.g, 0, coordPair.r));
	this->colors.push_back((rand() % 6) > 0 ? color(0,0,1) : color(0,1,0));

	// Create the faces to begin refining at.
	std::vector<face> faceLevelZero;

	faceLevelZero.push_back(face(0,11,5));
	faceLevelZero.push_back(face(0,5,1));
	faceLevelZero.push_back(face(0,1,7));
	faceLevelZero.push_back(face(0,7,10));
	faceLevelZero.push_back(face(0,10,11));

	faceLevelZero.push_back(face(1,5,9));
	faceLevelZero.push_back(face(5,11,4));
	faceLevelZero.push_back(face(11,10,2));
	faceLevelZero.push_back(face(10,7,6));
	faceLevelZero.push_back(face(7,1,8));

	faceLevelZero.push_back(face(3,9,4));
	faceLevelZero.push_back(face(3,4,2));
	faceLevelZero.push_back(face(3,2,6));
	faceLevelZero.push_back(face(3,6,8));
	faceLevelZero.push_back(face(3,8,9));

	faceLevelZero.push_back(face(4,9,5));
	faceLevelZero.push_back(face(2,4,11));
	faceLevelZero.push_back(face(6,2,10));
	faceLevelZero.push_back(face(8,6,7));
	faceLevelZero.push_back(face(9,8,1));

	// Refine the IcoSphere by 2 levels.
	// We are using a temp face buffer as it will be replaced by a new set.
	// The vertex buffer is the actual buffer as we will add to it.
	Refine(this->verts, faceLevelZero, this->GetEntityID());

	// Store the new faces.
	this->faces = faceLevelZero;

	// We must create a vao and then store it in our GLIcoSphere.
	glGenVertexArrays(1, &this->vao); // Generate the VAO
	glBindVertexArray(this->vao); // Bind the VAO

	glGenBuffers(1, &this->buffers[this->VertBufIndex]); 	// Generate the vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, this->buffers[this->VertBufIndex]); // Bind the vertex buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * this->verts.size(), &this->verts.front(), GL_STATIC_DRAW); // Stores the verts in the vertex buffer.
	GLint posLocation = glGetAttribLocation(GLIcoSphere::shader.GetProgram(), "in_Position"); // Find the location in the shader where the vertex buffer data will be placed.
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0); // Tell the VAO the vertex data will be stored at the location we just found.
	glEnableVertexAttribArray(posLocation); // Enable the VAO line for vertex data.


	glGenBuffers(1, &this->buffers[this->ColorBufIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, this->buffers[this->ColorBufIndex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color) * this->colors.size(), &this->colors.front(), GL_STATIC_DRAW);
	GLint colLocation = glGetAttribLocation(GLIcoSphere::shader.GetProgram(), "in_Color");
	glVertexAttribPointer(colLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colLocation);

	glGenBuffers(1, &this->buffers[this->ElemBufIndex]); // Generate the element buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers[this->ElemBufIndex]); // Bind the element buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face) * this->faces.size(), &this->faces.front(), GL_STATIC_DRAW); // Store the faces in the element buffer.

	glBindVertexArray(0); // Reset the buffer binding because we are good programmers.
}

void GLIcoSphere::LoadShader() {
	GLIcoSphere::shader.LoadFromFile(GL_VERTEX_SHADER, "..\\..\\shaders\\icosphere.vert");
	GLIcoSphere::shader.LoadFromFile(GL_FRAGMENT_SHADER, "..\\..\\shaders\\icosphere.frag");
	GLIcoSphere::shader.CreateAndLinkProgram();
}

glm::vec3 GetMidPoint(vertex v1, vertex v2) {
	return glm::normalize(glm::vec3((v1.x + v2.x) / 2.0, (v1.y + v2.y) / 2.0, (v1.z + v2.z) / 2.0));
}

void GLIcoSphere::Refine(std::vector<vertex> &verts, std::vector<face> &faces, int level) {
	std::map<int64_t, int> cache;

	// refine faces
	std::vector<face> newFaces = faces;
	for (int i = 0; i < level; ++i) {
		std::vector<face> tempFaces;
		for (auto faceitr = newFaces.begin(); faceitr != newFaces.end(); ++faceitr) {
			int a,b,c;

			{
				int v1 = (*faceitr).v1;
				int v2 = (*faceitr).v2;
				// first check if we have it already
				bool firstIsSmaller = v1 < v2;
				int64_t smallerIndex = firstIsSmaller ? v1 : v2;
				int64_t greaterIndex = firstIsSmaller ? v2 : v1;
				int64_t key = (smallerIndex << 32) + greaterIndex;

				if (cache.find(key) != cache.end()) {
					a = cache[key];
				} else {
					glm::vec3 middle = GetMidPoint(this->verts[v1], this->verts[v2]);

					verts.push_back(vertex(middle.x, middle.y, middle.z));
					int blue = this->colors[v1].b + this->colors[v2].b;
					int green = this->colors[v1].g + this->colors[v2].g;
					
					if (blue == green) {
						int val = rand() % 2;
						if (val > 0) {
							green = 0;
						} else {
							blue = 0;
						}
					} else {
						int val = rand() % 6;
						if (val > 1) {
							blue = 1;
							green = 0;
						} else {
							green = 1;
							blue = 0;
						}
					}
					this->colors.push_back(color(0,green,blue));

					a = cache[key] = this->verts.size() - 1;
				}
			}
			{
				int v1 = (*faceitr).v2;
				int v2 = (*faceitr).v3;
				// first check if we have it already
				bool firstIsSmaller = v1 < v2;
				int64_t smallerIndex = firstIsSmaller ? v1 : v2;
				int64_t greaterIndex = firstIsSmaller ? v2 : v1;
				int64_t key = (smallerIndex << 32) + greaterIndex;

				if (cache.find(key) != cache.end()) {
					b = cache[key];
				} else {
					glm::vec3 middle = GetMidPoint(this->verts[v1], this->verts[v2]);

					verts.push_back(vertex(middle.x, middle.y, middle.z));
					int blue = this->colors[v1].b + this->colors[v2].b;
					int green = this->colors[v1].g + this->colors[v2].g;

					if (blue == green) {
						int val = rand() % 2;
						if (val > 0) {
							green = 0;
						} else {
							blue = 0;
						}
					} else {
						int val = rand() % 6;
						if (val > 1) {
							blue = 1;
							green = 0;
						} else {
							green = 1;
							blue = 0;
						}
					}
					this->colors.push_back(color(0,green,blue));

					b = cache[key] = this->verts.size() - 1;
				}
			}
			{
				int v1 = (*faceitr).v3;
				int v2 = (*faceitr).v1;
				// first check if we have it already
				bool firstIsSmaller = v1 < v2;
				int64_t smallerIndex = firstIsSmaller ? v1 : v2;
				int64_t greaterIndex = firstIsSmaller ? v2 : v1;
				int64_t key = (smallerIndex << 32) + greaterIndex;

				if (cache.find(key) != cache.end()) {
					c = cache[key];
				} else {
					glm::vec3 middle = GetMidPoint(this->verts[v1], this->verts[v2]);

					verts.push_back(vertex(middle.x, middle.y, middle.z));
					int blue = this->colors[v1].b + this->colors[v2].b;
					int green = this->colors[v1].g + this->colors[v2].g;

					if (blue == green) {
						int val = rand() % 2;
						if (val > 0) {
							green = 0;
						} else {
							blue = 0;
						}
					} else {
						int val = rand() % 6;
						if (val > 1) {
							blue = 1;
							green = 0;
						} else {
							green = 1;
							blue = 0;
						}
					}
					this->colors.push_back(color(0,green,blue));

					c = cache[key] = this->verts.size() - 1;
				}
			}

			tempFaces.push_back(face((*faceitr).v1, a, c));
			tempFaces.push_back(face((*faceitr).v2, b, a));
			tempFaces.push_back(face((*faceitr).v3, c, b));
			tempFaces.push_back(face(a, b, c));
		}
		newFaces = tempFaces;
	}
	faces = newFaces;
}

GLSLShader GLIcoSphere::shader;
