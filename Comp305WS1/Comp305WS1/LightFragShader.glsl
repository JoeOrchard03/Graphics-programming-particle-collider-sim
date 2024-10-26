#version 330 core

//Declare output variables using "out"
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    //ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //add the ambient light to the objects color
    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0);
}  