#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);