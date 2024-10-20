#version 330 core

//Declare output variables using "out"
out vec4 FragColor;

uniform vec4 ourColor; //Uniform global variable

void main()
{
    //Fragment shader only has one output which is the color output
    FragColor = ourColor;
} 