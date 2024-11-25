#define GLM_ENABLE_EXPERIMENTAL

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

//Window
SDL_Window* window;

bool running = true;

//Camera variables
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec4 cameraFace(0, 0, -1, 0);
glm::vec3 rotation = glm::vec3(0);
const float walkspeed = 0.2f, rotSpeed = 0.1f;

SDL_Window* CreateWindow()
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_Init failed", SDL_GetError(), NULL);
		return 0;
	}

	//Create a window, note we have to free the pointer returned using the DestroyWindow Function
	//https://wiki.libsdl.org/SDL_CreateWindow
	//Creates screen to view image with its dimensions uses a pointer so it does not duplicate, can change windows settings using different values
	window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 720, SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind, if not it did not work
	if (window == nullptr)
	{
		//Show error
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateWindow failed", SDL_GetError(), NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return 0;
	}

	return window;
}

void IntializeSDLVersion()
{
	//Set SDL_GL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

void IntializeGlew()
{
	//Initialize GLEW, glew lets sdl and open gl work together better
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to initialise GLEW", (char*)glewGetErrorString(glewError), NULL);
	}
}

void HandleInput(SDL_Event ev)
{
	//Switch case for every relevant input
	switch (ev.type)
	{
		//QUIT Message, usually called when the window has been closed
	case SDL_QUIT:
		running = false;
		break;
		//KEYDOWN Message, called when a key has been pressed down
	case SDL_MOUSEMOTION: {
		//Handles mouse movement
		rotation.y -= ev.motion.xrel * rotSpeed;
		rotation.x -= ev.motion.yrel * rotSpeed;
		glm::mat4 viewRotate(1.f);
		viewRotate = glm::rotate(viewRotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		viewRotate = glm::rotate(viewRotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		cameraFront = glm::normalize(glm::vec3(viewRotate * cameraFace)); //normalise to avoid magnitude
		break;
	}
	case SDL_KEYDOWN:
		//Check the actual key code of the key that has been pressed
		switch (ev.key.keysym.sym)
		{
		case SDLK_ESCAPE: // SDLK_(keybind) is how you do keybinds
			running = false;
			break;
		case SDLK_w:
			cameraPos += walkspeed * cameraFront;
			break;
		case SDLK_s:
			cameraPos -= walkspeed * cameraFront;
			break;
			//Added left and right camera movement
		case SDLK_a:
			cameraPos -= walkspeed * cameraRight;
			break;
		case SDLK_d:
			cameraPos += walkspeed * cameraRight;
			break;
		}
	}
}

int main(int argc, char ** argsv)
{
	//Initalize SDL version
	IntializeSDLVersion();

	//Create window
	window = CreateWindow();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	//Initalize Glew
	IntializeGlew();

	unsigned int VBO, VAO, EBO;
	//Initalise them
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Create and compile our GLSL program from the shaders
	// call the shader glsl files by name must be inside the same root folder like this example program otherwise it will not work.
	GLuint shaderProgram = LoadShaders("BasicVert.glsl",
		"BasicFrag.glsl");
	GLuint postShaderID = LoadShaders("vertShader_post.glsl",
		"fragShader_post.glsl");

	//Setup matricies
	glm::mat4 view, //View matrix - handles everything that the camera sees
		projection; //Projection matrix - gives the camera depth perspective

	float cubeVertices[] =
	{
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	//Tells OpenGL how to interpret the vertex data
	glVertexAttribPointer(
		0,			//Specifies which attribute to configure
		3,			//size (of attribute - 3D vector, so 3)
		GL_FLOAT,	//data type
		GL_FALSE,	//normalised? (between -1 and 1? if not, GL_TRUE will map automatically)
		0,			//stride (how far from start of one attribute to the next - beginning of one vertex to the next)
		(void*)0	//offset (how far from start of array buffer does first vertex start?)
	);

	//Enables the vertex attribute at the specified location
	glEnableVertexAttribArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//DEPTH TEST MOVED!!

	//Event loop, we will loop until running is set to false, usually if escape has been pressed or window is closed
	running = true;
	//SDL Event structure, this will be checked in the while loop
	SDL_Event ev;

	while (running) //functions as an update function
	{
		while (SDL_PollEvent(&ev))
		{
			HandleInput(ev);
		}

		//use imported shader program(s)
		glUseProgram(shaderProgram);
		//Represents where the camera is in 3D space
		int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));

		//view matrix represents what the camera sees
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//define the colour the 'clear' call uses when drawing over entire screen - (SETS BACKGROUND COLOUR)
		glClearColor(1.0f,1.0f,1.0f, 0.0f);
		//clear the screen (prevents drawing over previous screen)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//render texture on quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//clear memory before exit
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	//SDL_FreeSurface(image);
	SDL_GL_DeleteContext(glContext);

	glDeleteProgram(shaderProgram);
	SDL_GL_DeleteContext(glContext);
	//Destroy the window and quit SDL2, NB we should do this after all cleanup in this order!!!
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);
	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();

	return 0;
}

