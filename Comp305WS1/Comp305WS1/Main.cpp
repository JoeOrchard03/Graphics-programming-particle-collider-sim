#include <GLFW/glfw3.h>
#include <iostream>
#include <glad/glad.h>
#include "Shaders.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Camera variables
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Time variabes:
//Time between current frame and last frame
float deltaTime = 0.0f;
//Time of last frame
float lastFrame = 0.0f;

//Mouse variables:
bool firstMouse = true;
float yaw = -90.0f; // -90 points to the right direction
float pitch = 0.0f;
float lastX = 400.0f;
float lastY = 300.0f;

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

//Handles mouse movement
void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
    //Stops a big jump that happens when you first press into the screen
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    //calculates the mouse movement between the current and last frame
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    //sets the mouse position as the new last mouse positions
    lastX = xpos;
    lastY = ypos;

    //Sets and multiplies the mouse movement by the sensitivity
    const float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Sets the pitch and yaw of the camera using the offset of the mouse since the last frame
    yaw += xOffset;
    pitch += yOffset;

    //Constrains the y movement so you cannot flip around in circles by looking up or down too much
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    //creates a direction vector
    glm::vec3 direction;
    //Maths for converting the mouse movement into the proper camera direction
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    //normalizes and assigns the direction the mouse to where the camera should be facing
    cameraFront = glm::normalize(direction);
}

//Input handler function - camera controls and view matrix
void processInput(GLFWwindow* window, int shaderProgram)
{
    const float cameraSpeed = 2.5f * deltaTime;
    //Closes window on escape press
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
        std::cout << "Pressing W" << std::endl;
    } 
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
        std::cout << "Pressing S" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        std::cout << "Pressing A" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        std::cout << "Pressing D" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    //Set the view matrix using the camera variables
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(
        viewLoc, //Location of the global/uniform variable
        1, //How many matrices to send
        GL_FALSE, //If you want to transpose the matrix (swapping colums and rows) leave as false
        glm::value_ptr(view)); //Actual matrix data, converted to usable data using value_ptr
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

    //Handles mouse movement, x pos and y pos are the current mouse positions
    void mouse_callback(GLFWwindow * window, double xpos, double ypos);

    //When mouse is moved call mouse_callback function
    glfwSetCursorPosCallback(window, mouse_callback);

    //Mouse stays in centre of the window and hides it
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Vertices for the cube
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    //Creates a Vertex buffer object that can store vertices, a Vertex Array object that stores the states of buffer objects, and an element buffer object for storing indices
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //binds the VAO
    glBindVertexArray(VAO);
    
    //binds the VBO and copies vertices into the VBO buffer obj
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //Enables the vertex attribute at the location specified
    glEnableVertexAttribArray(0);

    //Configures the texture attribute
    glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);

    //Load texture
    int texture = LoadTexture();

    //Creates a projection matrix which gives things perspective (makes things further away appear smaller) by creating a frustrum (area that renders things inside and does not render things outside)
    glm::mat4 projection;
    projection = glm::perspective(
        glm::radians(45.0f), //FOV
        800.0f / 600.0f, //Aspect ratio (viewport size)
        0.1f, //Near plane of the frustum
        100.0f); //Far plane of the frustum
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glEnable(GL_DEPTH_TEST);

    //Runs until the window is told to close
    while (!glfwWindowShouldClose(window))
    {
        //Gets delta time to use for smoothing out camera movement
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Gets inputs
        processInput(window, shaderProgram);

        //Want to call rendering commands every frame so they go here:
        //clears the screen and sets it as the color in the glClearColor function
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        //Uses the shader program that has the vertex and fragment shaders linked
        glUseProgram(shaderProgram);

        //Binds the vertex array object
        glBindVertexArray(VAO);

        //iterates through 10 cubes and renders them in different positions and angles
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            if (i == 0)
            {
                angle = rand() % 1 + 90;
            }
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(
                modelLoc, //Location of the global/uniform variable
                1, //How many matrices to send
                GL_FALSE, //If you want to transpose the matrix (swapping colums and rows) leave as false
                glm::value_ptr(model)); //Actual matrix data, converted to usable data using value_ptr

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //Swaps the color buffer
        glfwSwapBuffers(window);
        //Checks if events are triggered like mouse movement / key presses and calls necessary functions when they are detected
        glfwPollEvents();
    }

    //deletes and clears all used resources to avoid memory leaks and performance issues
    glfwTerminate();
    return 0;
}

