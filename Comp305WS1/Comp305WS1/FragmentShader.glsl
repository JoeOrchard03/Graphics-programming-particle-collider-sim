#version 330 core
//Declare output variables using "out"
out vec4 FragColor;

void main()
{
    //Fragment shader only has one output which is the color output
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 