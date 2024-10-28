#pragma once

#include <gl\glew.h>
#include <SDL_opengl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Vertex.h"
#include <assimp/Importer.hpp>	
#include <assimp/scene.h>	
#include <assimp/postprocess.h>	
#include <iostream>
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

void LoadBufferObjects(std::vector<Vertex> vertices, std::vector<unsigned> indices, GLuint VBO, GLuint VAO, GLuint EBO);