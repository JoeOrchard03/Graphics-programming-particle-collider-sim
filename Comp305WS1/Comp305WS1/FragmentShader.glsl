#version 330 core

//Declare output variables using "out"
out vec4 FragColor;

in vec3 ourColor; //Uniform global variable

void main()
{
    //Fragment shader only has one output which is the color output
    FragColor = vec4(ourColor, 1.0);
} 