#include <GLFW/glfw3.h>
#include <iostream>
#include <glad/glad.h>
#include "Shaders.h"
#include "stb_image.h"

//Convertes loaded image into texture
int LoadTexture()
{
    int width, height, nrChannels;
    //Uses the file path to gets the texture, stbi_load will then fill the other variables with the image's width, height and number of color channels
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

    //Creates and binds a texture object
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //if image data is opened successfully
    if (data)
    {
        //Generate a texture using image data
        glTexImage2D(
            GL_TEXTURE_2D, //Sets texture target,
            0, //Specifies mip map level, 0 is the base level
            GL_RGB, //Tells OpenGL the format to store the texture in
            width, //width from the loaded image
            height, //height from the loaded image
            0, //Legacy settings
            GL_RGB, //Format of the image
            GL_UNSIGNED_BYTE, //Data type of the image
            data); //Actual image data

        //Generates mip map for the loaded texture
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Successfully converted image into texture and loaded it" << std::endl;
    }
    else
    {
        std::cout << "Failed to convert image into texture or load it" << std::endl;
    }
    //Can remove the image after conversion into texture to help performance
    stbi_image_free(data);
    return texture;
}

//Called on resizing of the window by user
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    std::cout << "Re-sizing window" << std::endl;
}

//Input handler function
void processInput(GLFWwindow* window)
{
    //Closes window on escape press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    //Initialises the glfw library
    glfwInit();
    //Sets glfw params like version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //Creates a new window variable using the create window function, first two vars are width and height third is window name
    GLFWwindow* window = glfwCreateWindow(800, 600, "Comp305 Worksheet 1 OpenGL Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //Makes the new window the main focus of the program
    glfwMakeContextCurrent(window);

    //Initalises GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Sets the size of the rendering window, first 2 params set the start point to the lower left corner of the window, last 2 set size of the viewport (equal to the window)
    glViewport(0, 0, 800, 600);

    //Settings up window resize function for when user changes window size while it is displaying
    void framebuffer_size_callback(GLFWwindow * window, int width, int height);

    //Calls window resize function on window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Vertices for the rectangle
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    //Creates an element buffer object that stores indices
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    //binds the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //Copy the indices into EBO buffer obj
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //VBO is a vertex buffer object, it can store a large number of vertices in GPU memory. Allows sending of large batches of data to GPU to avoid performance issues
    unsigned int VBO;
    //Generates a vertex buffer object with an ID and points to the VBO
    glGenBuffers(1, &VBO);

    //Creats a vertex arrray object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    //Binds the VBO object to a open gl array buffer, meaning any calls made to the array buffer will configure VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //This function copies user data into the bound buffer, takes the size of data, can use sizeof(vertices), third param is data being sent and fourth is how the GPU should manage the data out of stream draw, static draw and dynamic draw, stream is set once and used a few times, static is set once and used a lot, dynamic is set a lot and used a lot, since triangle is not moving but is needed to be drawn every frame that is the one being used
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Creates shader program object with vertex shader and fragment shader linked to it
    int shaderProgram = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");;

    //Tells OpenGL how to interpret the vertex data, first param specifies which vertex attribute to configure
    //2nd specifies size of the vertex which is a v3 so has 3 values
    //3rd specifies type of data
    //4th says if you want to normalize the vertex
    //5th is the stride that says the space between vertex attributes
    //6th is the offset of where the position data begins
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //Enables the vertex attribute at the location specified
    glEnableVertexAttribArray(0);

    //Configures the color attribute
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    //Configures the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //Load texture
    int texture = LoadTexture();

    //Runs until the window is told to close
    while (!glfwWindowShouldClose(window))
    {
        //Gets inputs
        processInput(window);

        //Want to call rendering commands every frame so they go here:
        //clears the screen and sets it as the color in the glClearColor function
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        //Uses the shader program that has the vertex and fragment shaders linked
        glUseProgram(shaderProgram);
        
        ////Gets the time in seconds
        //float timeValue = glfwGetTime();
        ////Vary the color
        //float greenValue = sin(timeValue) / 2.0f + 0.5f;
        ////Gets the location of the global variable for use
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        ////Set the global variable
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        
        //Binds the vertex array object
        glBindVertexArray(VAO);
        //Handles drawingFirst param specifies the draw mode, 2nd is the number of vertices to draw,
        //third is the type of indices, fourth is the offset
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Swaps the color buffer
        glfwSwapBuffers(window);
        //Checks if events are triggered like mouse movement / key presses and calls necessary functions when they are detected
        glfwPollEvents();
    }

    //deletes and clears all used resources to avoid memory leaks and performance issues
    glfwTerminate();
    return 0;
}

