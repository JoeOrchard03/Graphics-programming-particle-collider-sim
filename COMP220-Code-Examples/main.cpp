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

//Global variables for loading models
//Vertices are the points that make up the shape
std::vector<Vertex> vertices;
//Indices control which vertices link to form a shape
std::vector<unsigned> indices;
//Texture
std::string texturePath;

//Global variables for loading models
//Vertices are the points that make up the shape
std::vector<Vertex> vertices2;
//Indices control which vertices link to form a shape
std::vector<unsigned> indices2;
//Texture
std::string texturePath2;

//Camera variables
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec4 cameraFace(0, 0, -1, 0);
glm::vec3 rotation = glm::vec3(0);
const float walkspeed = 0.2f, rotSpeed = 0.1f;

//Number of boxes to spawn to represent particles
//unsigned int numOfBoxes = 10;
//unsigned int numOfBoxes = 100;
unsigned int numOfBoxes = 1000;

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

	//LoadModel
	LoadModel("Crate.fbx", vertices, indices, texturePath);
	
	////LoadModel
	LoadModel("Crate.fbx", vertices2, indices2, texturePath2);

	//LoadModel
	//LoadModel("utah-teapot.fbx", vertices2, indices2, texturePath2);

	//Check if model has texture
	bool hasTexture = !texturePath.empty();

	//hard coded texture path
	SDL_Surface* image = IMG_Load("tex/crate_color.png");

	//only load texture path if exists
	//SDL_Surface* image2 = hasTexture ? IMG_Load(texturePath.c_str()) : nullptr;
	//if (hasTexture && !image2) {
	//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "IMG_Load failed", IMG_GetError(), NULL);
	//	SDL_DestroyWindow(window);
	//	SDL_Quit();
	//	return 1;
	//}

	SDL_Surface* image2 = IMG_Load("tex/crate_color.png");

	// Create one OpenGL texture
	GLuint textureID;
	//Create buffer objects
	unsigned int VBO, VAO, EBO;
	//Initalise them
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//Load all their attributes and stuff in this function
	LoadBufferObjects(vertices, indices, VBO, VAO, EBO);

	//Create texture and buffer objects for other model
	GLuint textureID2;
	unsigned int VBO2, VAO2, EBO2;
	//Initalise them
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);
	//Load all their attributes and stuff in this function
	LoadBufferObjects(vertices2, indices2, VBO2, VAO2, EBO2);

	//Texture binding for first model
	if (image) {
		//NOTE: FOLLOWING CODE BLOCK DERIVED FROM: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#using-the-texture-in-opengl

		//IF IMAGE BREAKING CHECK THIS
		glGenTextures(1, &textureID);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);

		int Mode = GL_RGB;

		if (image->format->BytesPerPixel == 4) {
			Mode = GL_RGBA;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, Mode, image->w, image->h, 0, Mode, GL_UNSIGNED_BYTE, image->pixels);

		// Nice trilinear filtering.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //repeats if we go beyond TEXTURE UV maxima
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//Texture binding for second model
	if (image2) {
		//NOTE: FOLLOWING CODE BLOCK DERIVED FROM: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#using-the-texture-in-opengl

		//IF IMAGE BREAKING CHECK THIS
		glGenTextures(1, &textureID2);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID2);

		int Mode = GL_RGB;

		if (image2->format->BytesPerPixel == 4) {
			Mode = GL_RGBA;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, Mode, image2->w, image2->h, 0, Mode, GL_UNSIGNED_BYTE, image2->pixels);

		// Nice trilinear filtering.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //repeats if we go beyond TEXTURE UV maxima
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Create and compile our GLSL program from the shaders
	// call the shader glsl files by name must be inside the same root folder like this example program otherwise it will not work.
	GLuint shaderProgram = LoadShaders("BasicVert.glsl",
		"BasicFrag.glsl");
	GLuint postShaderID = LoadShaders("vertShader_post.glsl",
		"fragShader_post.glsl");
	GLuint transparentShader = LoadShaders("BasicVert.glsl", "TransparentFrag.glsl");

	//Particle identity matrix
	glm::mat4 particleModel = glm::mat4(1.0f);
	particleModel = glm::scale(particleModel,glm::vec3(0.001f,0.001f,0.001f));
	particleModel = glm::translate(particleModel, glm::vec3(0.0f, 0.0f, 0.0f));
	
	//Glass identiy matrix
	glm::mat4 glassModel = glm::mat4(1.0f);
	glassModel = glm::scale(glassModel, glm::vec3(0.01f, 0.01f, 0.001f));
	//Move glass to the right so the particle does not spawn inside of it
	glassModel = glm::translate(glassModel, glm::vec3(0.0f, 0.0f, 1000.0f));

	//Setup matricies
	glm::mat4 glassPlaneMVP, //Glass plane model, view, projection matrix
		particleMVP, //Particle model, view, projection matrix
		view, //View matrix - handles everything that the camera sees
		projection; //Projection matrix - gives the camera depth perspective

	//Get location of uniform variables in the shader program
	unsigned int 
		transformLoc = glGetUniformLocation(shaderProgram, "transform"), //Location of the object
		objColourLoc = glGetUniformLocation(shaderProgram, "objColour"), //Color of the object, overwritten by texture
		modelLoc = glGetUniformLocation(shaderProgram, "model"); //Model identity matrix

	//Lighting
	//COPY THIS BLOCK FROM LECTURE SLIDES!!!
	//note: lightcolour is changed!
	float lightValues[] = {
		-1.0f, 1.f, 0.4f, // lightDir - rotation the light is coming from
		0.0f, // padding for alignment - do you want to offset the light
		1.f, 1.f, 1.f // lightColour - The colour of the light in rgb values
	};

	GLuint bindingPoint = 1, uniformBuffer, blockIndex;
	blockIndex = glGetUniformBlockIndex(shaderProgram, "LightBlock");
	glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);
	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lightValues), lightValues, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uniformBuffer);

	// NOTE: THIS SECTION IS FROM WORKSHOP SLIDES
	//The texture we are going to render to
	GLuint renderTextureID;
	glGenTextures(1, &renderTextureID);
	//Bind Texture
	glBindTexture(GL_TEXTURE_2D, renderTextureID);
	//fill with empty data
	//NOTE: REPLACE IMAGE width, height WITH SCREEN RESOLUTION
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 960, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	//min and mag filter filters the colours between pixels so there are not sharp changes, gl_linear filtering applies this.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//without clamping, our fragShader kernal may go over the edge and wrap over the screen, causing unwanted artefacts! Stops texture spilling over the side of the model
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//NOTE ALSO TAKEN FROM WORKSHOP SLIDES
	//The depth buffer
	GLuint depthBufferID;
	glGenRenderbuffers(1, &depthBufferID);
	//Bind the depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
	//Set the format of the depth buffer
	//NOTE: UPDATED SCREEN RESOLUTION
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 960, 720);

	//NOTE ALSO COPIED FROM WORKSHOP SLIDES
	//The framebuffer
	GLuint frameBufferID;
	glGenFramebuffers(1, &frameBufferID);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

	//NOTE partially COPIED FROM WORKSHOP SLIDES
	//Bind the texture as a colour attachment 0 to the
	//active framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTextureID, 0);
	//Bind the depth buffer as a depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		//error message!
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to create framebuffer", "", NULL);
	}

	//Screen Quad// - The post effect rectangle that goes in front of the camera
	//EXPANDED from workshop slides
	float quadVertices[] =
	{
		-1, -1,
		1, -1,
		-1, 1,
		1, 1,
	};

	unsigned quadIndices[] = {
		0, 1, 3, 
		3, 2, 0
	};

	//Array to store their positions
	std::vector <glm::vec3> boxPositions;
	//glm::mat4 boxModels[10];
	//glm::mat4 boxModels[100];
	glm::mat4 boxModels[1000];

	//for loop for randomly setting the x and ys of the boxes
	for (int i = 0; i < numOfBoxes; i++)
	{
		boxPositions.push_back(glm::vec3(rand() % 1000 + 1, rand() % 1000 + 1, 200.0f));
		glm::mat4 newBoxModel = glm::mat4(1.0f);
		newBoxModel = glm::scale(newBoxModel, glm::vec3(0.001f, 0.001f, 0.001f));
		newBoxModel = glm::translate(newBoxModel, boxPositions[i]);
		boxModels[i] = newBoxModel;
		std::cout << "Box " << i << " position is: " << glm::to_string(boxPositions[i]) << std::endl;
	}

	//OID means object ID
	GLuint screenQuadVBOID;
	glGenBuffers(1, &screenQuadVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);
	//Creats array buffer it is 8 * size of float because we need an x and y for each point 4x2 is 8
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), quadVertices, GL_STATIC_DRAW);

	GLuint screenVAOID;
	glGenVertexArrays(1, &screenVAOID);
	glBindVertexArray(screenVAOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);
	glEnableVertexAttribArray(0);
	//note, see previous use of attribPointer for detail. 
	//We can use 0 as its locaiton as it is a different object
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint quadEBOID;
	glGenBuffers(1, &quadEBOID);

	//copied and MODIFIED from existing previous element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBOID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * 6, quadIndices, GL_STATIC_DRAW);

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

		//Bind the framebuffer - making a new frame and loading that frame into the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		//Note later disable!
		glEnable(GL_DEPTH_TEST); // depth test stops us rendering stuff that is behind other stuff massively helps on performance.

		//define the colour the 'clear' call uses when drawing over entire screen - (SETS BACKGROUND COLOUR)
		glClearColor(1.0f,1.0f,1.0f, 0.0f);
		//clear the screen (prevents drawing over previous screen)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(45.f), 4.0f / 3.0f, 0.1f, 100.0f);
		//glm::ortho for orthographic
		glassPlaneMVP = projection * view * glassModel;

		//if there is a texture it disables the colour and lets the texture handle it
		if (hasTexture) {
			glUniform3f(objColourLoc, -1.0f, -1.0f, -1.0f);
		}
		//If you do not have a texture it uses white as the default
		else {
			glUniform3f(objColourLoc, 1.0f, 1.0f, 1.0f);
		}

		glBindVertexArray(VAO);
		glUseProgram(shaderProgram);
		if (image) glBindTexture(GL_TEXTURE_2D, textureID);
		/*particleMVP = projection * view * particleModel;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(particleMVP));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(particleModel));
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
		particleModel = glm::translate(particleModel, glm::vec3(0.0f, 0.0f, 1.0f));*/

		//For each item in numOfBoxes
		for (int i = 0; i < numOfBoxes; i++)
		{
			//Update the 
			particleMVP = projection * view * boxModels[i];
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(particleMVP));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(boxModels[i]));
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
			boxModels[i] = glm::translate(boxModels[i], glm::vec3(0.0f, 0.0f, rand() % 1 + 1));
		}

		//Draw glass pane
		glUseProgram(transparentShader);
		glassPlaneMVP = projection * view * glassModel;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(glassPlaneMVP));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glassModel));
		glBindVertexArray(VAO2);
		if (image2) glBindTexture(GL_TEXTURE_2D, textureID2);
		glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, (void*)0);

		

		//render texture on quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(postShaderID);
		glDisable(GL_DEPTH_TEST); //note earlier enable!

		glBindVertexArray(screenVAOID);
		glBindTexture(GL_TEXTURE_2D, renderTextureID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		
		SDL_GL_SwapWindow(window);
	}

	//clear memory before exit
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	SDL_FreeSurface(image);
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

