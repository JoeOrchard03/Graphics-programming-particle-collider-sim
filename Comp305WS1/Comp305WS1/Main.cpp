#include <GLFW/glfw3.h>
#include <iostream>
#include <glad/glad.h>

//Points to vertex shader code
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

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

//Compiles vertex shader and checks for sucess
int CompileVertexShader()
{
    //Creates a new vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //Links the shader and shader source
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    //Compliles the shader
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else
    {
        std::cout << "Vertex shader compilation successful" << std::endl;
    }

    return vertexShader;
}

//Compiles fragment shader and checks for sucess
int CompileFragmentShader()
{
    //Creates new fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    //Link the fragment shader and it's source code
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    //Compiles
    glCompileShader(fragmentShader);
    //Checks for sucess of fragment shader

    int  success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else
    {
        std::cout << "Fragment shader compilation successful" << std::endl;
    }

    return fragmentShader;
}

int CreateShaderProgramObj(int vertexShader, int fragmentShader)
{
    unsigned int shaderProgram;
    //Creates a shader program and assigns an ID to it
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM_OBJ::CREATION_FAILED\n" << infoLog << std::endl;
    }
    else
    {
        std::cout << "Creating shader program successful" << std::endl;
    }

    return shaderProgram;
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

    float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
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

    //Compiles vertex shader and checks if it was successful
    int vertexShader = CompileVertexShader();

    //Compiles fragment shader and checks if it was successful
    int fragmentShader = CompileFragmentShader();

    //Creates shader program object with vertex shader and fragment shader linked to it
    int shaderProgram = CreateShaderProgramObj(vertexShader, fragmentShader);

    //Clears shader objects from memory, now they are on the shader program dont need them
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Tells OpenGL how to interpret the vertex data, first param specifies which vertex attribute to configure
    //2nd specifies size of the vertex which is a v3 so has 3 values
    //3rd specifies type of data
    //4th says if you want to normalize the vertex
    //5th is the stride that says the space between vertex attributes
    //6th is the offset of where the position data begins
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //Enables the vertex attribute at the location specified
    glEnableVertexAttribArray(0);

    

    //Runs until the window is told to close
    while (!glfwWindowShouldClose(window))
    {
        //Gets inputs
        processInput(window);

        //Want to call rendering commands every frame so they go here:
        //clears the screen and sets it as the color in the glClearColor function
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Uses the shader program that has the vertex and fragment shaders linked
        glUseProgram(shaderProgram);
        //Binds the vertex array object
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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

