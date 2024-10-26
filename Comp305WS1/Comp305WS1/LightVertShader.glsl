//Sets the version of OpenGL
#version 330 core
//Declare all input vertex attirbutes, each vertex is made of a set of 3d coordinates which is why you use vector 3, also set the location of the variable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

//Output the frag position to the fragment shader
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;

void main()
{
    //Set the output of the vertex shader using the position of the vector and giving it a w value of 1
    //Gets the fragment position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;

    gl_Position = projection * view * vec4(aPos, 1.0);
}