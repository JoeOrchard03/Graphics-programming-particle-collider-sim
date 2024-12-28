#pragma once
#include <iostream>
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"

#include <assimp/Importer.hpp>	
#include <assimp/scene.h>	
#include <assimp/postprocess.h>	

#include "Shader.h"
#include "Vertex.h"
#include "LoadModel.h"
#include "BufferObjectsLoad.h"
#include <random>
#include <vector>

struct ModelData {
	bool VerticesAndIndiciesPopulated;
	std::vector<Vertex> ModelVertices;
	std::vector<unsigned int> ModelIndices;
};