#include <GLFW/glfw3.h>
#include <iostream>
#include <glad/glad.h>


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

//Does all the drawing onto the window gets called in the main while loop every frame
void processDrawing()
{
    //clears the screen and sets it as the color in the glClearColor function
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    //VBO is a vertex buffer object, it can store a large number of vertices in GPU memory. Allows sending of large batches of data to GPU to avoid performance issues
    unsigned int VBO;
    //Generates a vertex buffer object with an ID and points to the VBO
    glGenBuffers(1, &VBO);

    //Binds the VBO object to a open gl array buffer, meaning any calls made to the array buffer will configure VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //This function copies user data into the bound buffer, takes the size of data, can use sizeof(vertices), third param is data being sent and fourth is how the GPU should manage the data out of stream draw, static draw and dynamic draw, stream is set once and used a few times, static is set once and used a lot, dynamic is set a lot and used a lot, since triangle is not moving but is needed to be drawn every frame that is the one being used
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    //Runs until the window is told to close
    while (!glfwWindowShouldClose(window))
    {
        //Gets inputs
        processInput(window);

        //Want to call rendering commands every frame so they go here:
        processDrawing();

        //Swaps the color buffer
        glfwSwapBuffers(window);
        //Checks if events are triggered like mouse movement / key presses and calls necessary functions when they are detected
        glfwPollEvents();
    }

    //deletes and clears all used resources to avoid memory leaks and performance issues
    glfwTerminate();
    return 0;
}

