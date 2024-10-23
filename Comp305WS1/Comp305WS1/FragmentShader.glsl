#version 330 core

//Declare output variables using "out"
out vec4 FragColor;

in vec2 TexCoord; //Uniform global variable

uniform sampler2D ourTexture;

void main()
{
    //Fragment shader only has one output which is the color output
    FragColor = texture(ourTexture, TexCoord);
} 