#include <iostream>
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>	
#include <assimp/scene.h>	
#include <assimp/postprocess.h>	

#include "Shader.h"
#include "Vertex.h"
#include <random>


bool LoadModel(const char* filePath, std::vector<Vertex>& vertices, std::vector<unsigned>& indices, std::string& texturePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |
													   aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_FixInfacingNormals);
	//block based off learningspace tutorials
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || !scene->HasMeshes()) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Model import failed", importer.GetErrorString(), NULL);
		return false;
	}
	//JUST A DEMO, assuming just one texture! You can use other textures, see documentation!
	//http://assimp.sourceforge.net/lib_html/structai_material.html
	bool hasTexture = false;
	aiString texPath("");
	if (scene->HasMaterials()) {
		aiMaterial* material = scene->mMaterials[0];
		hasTexture = material && material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) >= 0;
	}
	texturePath = texPath.C_Str(); //converts to const char
	//JUST A DEMO, assuming one mesh!
	aiMesh* mesh = scene->mMeshes[0];
	if (!mesh) return false;

	vertices.clear();
	indices.clear();

	vertices.resize(mesh->mNumVertices);
	aiVector3D* texCoords = hasTexture ? mesh->mTextureCoords[0] : nullptr;
	for(unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		vertices[i].x = mesh->mVertices[i].x;
		vertices[i].y = mesh->mVertices[i].y;
		vertices[i].z = mesh->mVertices[i].z;

		if (mesh->HasNormals()) {
			vertices[i].nx = mesh->mNormals[i].x;
			vertices[i].ny = mesh->mNormals[i].y;
			vertices[i].nz = mesh->mNormals[i].z;
		}

		if (texCoords) {
			vertices[i].u = texCoords[i].x;
			vertices[i].v = texCoords[i].y;
		}
	}

	for (unsigned i = 0; i < mesh->mNumFaces; i++) 
	{
		aiFace& face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; j++) 
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return !(vertices.empty() || indices.empty());
}

int main(int argc, char ** argsv)
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_Init failed", SDL_GetError(), NULL);
		return 1;
	}
	
	//Create a window, note we have to free the pointer returned using the DestroyWindow Function
	//https://wiki.libsdl.org/SDL_CreateWindow
	SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 720, SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind
	if (window == nullptr)
	{
		//Show error
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateWindow failed", SDL_GetError(), NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return 1;
	}

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	std::string texturePath;

	//we can check for box here too!
	if (!LoadModel("Crate.fbx", vertices, indices, texturePath))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR LOADING MODEL", "", NULL);
		//TODO: clean up here
		return 1;
	}

	bool hasTexture = !texturePath.empty();

	//only load texture path if exists
	SDL_Surface* image = hasTexture ? IMG_Load(texturePath.c_str()) : nullptr;
	if (hasTexture && !image) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "IMG_Load failed", IMG_GetError(), NULL);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);


	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	//Set SDL_GL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to initialise GLEW", (char*)glewGetErrorString(glewError), NULL);
	}

	//create Vertex Array Object
	GLuint VertexArrayID;
	//generate vertex array objects
	glGenVertexArrays(1, &VertexArrayID); //first param is number of object, 2nd is object
	//bind gl calls to VertexArrayID
	glBindVertexArray(VertexArrayID);


	//storage area for our buffer data
	GLuint vertexBuffer;
	//generate 1 buffer, put the resulting identifier in vertexBuffer
	glGenBuffers(1, &vertexBuffer);
	//bind the buffer so the operations of GLBuffer apply to vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//set our current buffer target data to the g_buffer_data object
	//static draw means we are making a static object - we define the object once and do not change it (similar to static in Unity/Unreal)
	//see GL documentation for other draw types
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0); //attribute in location 0
	glVertexAttribPointer(
		0,			//attribute 0, must match the layout in the shader
		3,			//size (of attribute - 3D vector, so 3)
		GL_FLOAT,	//data type
		GL_FALSE,	//normalised? (between -1 and 1? if not, GL_TRUE will map automatically)
		sizeof(Vertex),			//stride (how far from start of one attribute to the next - beggining of one vertex to the next)
		(void*)0	//array buffer offset (how far from start of array buffer does first vertex start?)
	);

	glEnableVertexAttribArray(1); //attribute in location 0
	glVertexAttribPointer(
		1,			//attribute 0, must match the layout in the shader
		3,			//size (of attribute - 3D vector, so 3)
		GL_FLOAT,	//data type
		GL_FALSE,	//normalised? (between -1 and 1? if not, GL_TRUE will map automatically)
		sizeof(Vertex),			//stride (how far from start of one attribute to the next - beggining of one vertex to the next)
		(void*)(3 * sizeof(GL_FLOAT))	//array buffer offset (how far from start of array buffer does first vertex start?)
	);

	glEnableVertexAttribArray(2); //attribute in location 1
	glVertexAttribPointer(
		2,			//attribute 1, must match the layout in the shader
		2,			//size (of attribute - 2D vector, so 2)
		GL_FLOAT,	//data type
		GL_FALSE,	//normalised? (between -1 and 1? if not, GL_TRUE will map automatically)
		sizeof(Vertex),			//stride (how far from start of one attribute to the next - beggining of one vertex to the next)
		(void*)(6 * sizeof(GL_FLOAT))	//NOTE: Starts after first 3 float values!
	);
	


	//modified from: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
	// Generate a buffer for the indices
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), &indices[0], GL_STATIC_DRAW);


	// Create one OpenGL texture
	GLuint textureID;

	if (image) {
		//NOTE: FOLLOWING CODE BLOCK DERIVED FROM: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#using-the-texture-in-opengl


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
	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("BasicVert.glsl", 
		"BasicFrag.glsl");
	GLuint postShaderID = LoadShaders("vertShader_post.glsl",
		"fragShader_post.glsl");

	glm::mat4 model = glm::mat4(1.0f);		//identity matrix
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	model = glm::scale(model,glm::vec3(0.01f,0.01f,0.01f));

	glm::mat4 mvp, view, projection; // set up model, view, projection
	
	glm::vec3 position(0, 0, 2), forward(0,0,-1), rotation(0);
	const glm::vec4 cameraFace(0,0,-1,0);
	const float walkspeed = 0.2f, rotSpeed = 0.1f;
	unsigned int transformLoc = glGetUniformLocation(programID, "transform");
	unsigned int objColourLoc = glGetUniformLocation(programID, "objColour");
	unsigned int modelLoc = glGetUniformLocation(programID, "model");
	unsigned int viewPosLoc = glGetUniformLocation(programID, "viewPos");

	//COPY THIS BLOCK FROM LECTURE SLIDES!!!
	//note: lightcolour is changed!
	float lightValues[] = {
		-1.0f, 1.f, 0.4f, // lightDir
		0.0f, // padding for alignment
		1.f, 1.f, 1.f // lightColour
	};
	GLuint bindingPoint = 1, uniformBuffer, blockIndex;
	blockIndex = glGetUniformBlockIndex(programID, "LightBlock");
	glUniformBlockBinding(programID, blockIndex, bindingPoint);
	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lightValues),
				 lightValues, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint,
		uniformBuffer);

	// NOTE: THIS SECTION IS FROM WORKSHOP SLIDES
	//The texture we are going to render to
	GLuint renderTextureID;
	glGenTextures(1, &renderTextureID);
	//Bind Texture
	glBindTexture(GL_TEXTURE_2D, renderTextureID);
	//fill with empty data
	//NOTE: REPLACE IMAGE width, height WITH SCREEN RESOLUTION
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 960, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//without clamping, our fragShader kernal may go over the edge and wrap over the screen, causing unwanted artefacts!
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


	//Screen Quad//
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

	GLuint screenQuadVBOID;
	glGenBuffers(1, &screenQuadVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);
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
	bool running = true;
	//SDL Event structure, this will be checked in the while loop
	SDL_Event ev;
	while (running)
	{
		//Poll for the events which have happened in this frame
		//https://wiki.libsdl.org/SDL_PollEvent
		while (SDL_PollEvent(&ev))
		{
			//Switch case for every message we are intereted in
			switch (ev.type)
			{
				//QUIT Message, usually called when the window has been closed
			case SDL_QUIT:
				running = false;
				break;
				//KEYDOWN Message, called when a key has been pressed down
			case SDL_MOUSEMOTION: {
				rotation.y -= ev.motion.xrel * rotSpeed;
				rotation.x -= ev.motion.yrel * rotSpeed;
				glm::mat4 viewRotate(1.f);
				viewRotate = glm::rotate(viewRotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				viewRotate = glm::rotate(viewRotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				forward = glm::normalize(glm::vec3(viewRotate * cameraFace)); //normalise to avoid magnitude
				break;
			}
			case SDL_KEYDOWN:
				//Check the actual key code of the key that has been pressed
				switch (ev.key.keysym.sym)
				{
					//Escape key
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_w:
					position += walkspeed * forward;
					break;
				case SDLK_s:
					position -= walkspeed * forward;
					break;
				}
			}
		}

		model = glm::rotate(model, glm::radians(0.1f), glm::vec3(0.0, 0.1, 0.1));
		//model = glm::translate(model, glm::vec3(0, 0.01, -0.01f));

		//Bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		//Note later disable!
		glEnable(GL_DEPTH_TEST);

		//define the colour the 'clear' call uses when drawing over entire screen
		glClearColor(0.0f,0.0f,0.4f,0.0f);
		//clear the screen (prevents drawing over previous screen)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use imported shader program(s)
		glUseProgram(programID);

		view = glm::lookAt(
			position,
			position + forward, //glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		projection = glm::perspective(glm::radians(45.f), 4.0f / 3.0f, 0.1f, 100.0f);
		//glm::ortho for orthographic
		mvp = projection * view * model;

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		if (hasTexture) {
			glUniform3f(objColourLoc, -1.0f, -1.0f, -1.0f);
		}
		else {
			glUniform3f(objColourLoc, 1.0f, 1.0f, 1.0f);
		}

		glUniform3f(viewPosLoc, position.x, position.y, position.z);

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		// Draw object from file
		glBindVertexArray(VertexArrayID);
		if(image) glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
		
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
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteVertexArrays(1, &VertexArrayID);

	SDL_FreeSurface(image);
	SDL_GL_DeleteContext(glContext);

	glDeleteProgram(programID);
	SDL_GL_DeleteContext(glContext);
	//Destroy the window and quit SDL2, NB we should do this after all cleanup in this order!!!
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);
	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();

	return 0;
}