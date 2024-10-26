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

//Light source position:
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//Convertes loaded image into texture
int LoadTexture(char const* path)
{
    //Creates texture object
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    //Uses the file path to gets the texture, stbi_load will then fill the other variables with the image's width, height and number of color channels
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    //if image data is opened successfully
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        //Binds texture object
        glBindTexture(GL_TEXTURE_2D, textureID);

        //Generate a texture using image data
        glTexImage2D(
            GL_TEXTURE_2D, //Sets texture target,
            0, //Specifies mip map level, 0 is the base level
            format, //Tells OpenGL the format to store the texture in
            width, //width from the loaded image
            height, //height from the loaded image
            0, //Legacy settings
            format, //Format of the image
            GL_UNSIGNED_BYTE, //Data type of the image
            data); //Actual image data

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Generates mip map for the loaded texture
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Successfully converted image into texture and loaded it" << std::endl;

        //Can remove the image after conversion into texture to help performance
        stbi_image_free(data);

    }
    else
    {
        std::cout << "Failed to convert image into texture or load it" << std::endl;
        stbi_image_free(data);
    }
    return textureID;
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
void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 2.5f * deltaTime;
    //Closes window on escape press
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    } 
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
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

    //Creates shader program objects with vertex shaders and fragment shaders linked to them
    int lightShaderProgram = LoadShaders("LightVertShader.glsl", "LightFragShader.glsl");
    int shaderProgram = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");

    glEnable(GL_DEPTH_TEST);

    //Vertices for the cube
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    //Creates a Vertex buffer object that can store vertices and a Vertex Array object that stores the states of buffer objects
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Copies vertices into the VBO buffer obj
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertices), //takes the size of data, can use sizeof(vertices)
        vertices, //third param is data being sent
        GL_STATIC_DRAW); //how the GPU should manage the data out of stream draw, static draw and dynamic draw

    //binds the VAO
    glBindVertexArray(VAO);

    //Tells OpenGL how to interpret the vertex data - vertex position attributes
    glVertexAttribPointer(
        0, // which vertex attribute to configure
        3, // specifies size of the vertex which is a v3 so has 3 values
        GL_FLOAT, // specifies type of data
        GL_FALSE, // if you want to normalize the vertex
        8 * sizeof(float), // is the stride that says the space between vertex attributes
        (void*)0); // is the offset of where the position data begins
    //Enables the vertex attribute at the location specified
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    //new VAO for the light source object
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    //Bind vbo
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);

    int diffuseMap = LoadTexture("container2.png");

    glUseProgram(lightShaderProgram);
    int materialDiffuseLoc = glGetUniformLocation(lightShaderProgram, "material.diffuse");
    glUniform1f(materialDiffuseLoc, 0.0f);

    //Runs until the window is told to close
    while (!glfwWindowShouldClose(window))
    {
        //Gets delta time to use for smoothing out camera movement
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Gets inputs
        processInput(window);

        //Want to call rendering commands every frame so they go here:
        //clears the screen and sets it as the color in the glClearColor function
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Use light source program
        glUseProgram(lightShaderProgram);
        int lightPositionLoc = glGetUniformLocation(lightShaderProgram, "light.position");
        glUniform3fv(lightPositionLoc, 1, glm::value_ptr(glm::vec3(lightPos)));
        int viewPosLoc = glGetUniformLocation(lightShaderProgram, "viewPos");
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
        
        int lightAmbientLoc = glGetUniformLocation(lightShaderProgram, "light.ambient");
        glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));

        int lightDiffuseLoc = glGetUniformLocation(lightShaderProgram, "light.diffuse");
        glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));

        int lightSpecularLoc = glGetUniformLocation(lightShaderProgram, "light.specular");
        glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

        int materialSpecularLoc = glGetUniformLocation(lightShaderProgram, "material.specular");
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));

        int materialShininessLoc = glGetUniformLocation(lightShaderProgram, "material.shininess");
        glUniform1f(materialShininessLoc, 64.0f);

        //Creates a projection matrix which gives things perspective (makes things further away appear smaller) by creating a frustrum (area that renders things inside and does not render things outside)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f,100.0f);
        //creates view martrix (space seen from camera pov)
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        //Creates model matrix (transformations to apply to objects)
        glm::mat4 model = glm::mat4(1.0f);

        //Set uniform variables in the lighting vertex shader
        int projectionLoc = glGetUniformLocation(lightShaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        int viewLoc = glGetUniformLocation(lightShaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); 

        int modelLoc = glGetUniformLocation(lightShaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glBindVertexArray(VAO);
        //Draws cube
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Uses the shader program that has the vertex and fragment shaders linked
        glUseProgram(shaderProgram);

        //Sets uniform variables in the regular vertex shader
        projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //Set the view matrix using the camera variables
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Swaps the color buffer
        glfwSwapBuffers(window);
        //Checks if events are triggered like mouse movement / key presses and calls necessary functions when they are detected
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

