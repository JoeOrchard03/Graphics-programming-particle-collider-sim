#version 330 core

//Declare output variables using "out"
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    //Fragment shader only has one output which is the color output
    //multiply the light source color with the objects reflected color to get an accurate color outpu
    FragColor = vec4(lightColor * objectColor, 1.0);
} 