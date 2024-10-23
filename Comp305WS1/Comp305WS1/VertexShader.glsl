//Sets the version of OpenGL
#version 330 core
//Declare all input vertex attirbutes, each vertex is made of a set of 3d coordinates which is why you use vector 3, also set the location of the variable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord; //The texture coordinates of the image

uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;

void main()
{
    //Set the output of the vertex shader using the position of the vector and giving it a w value of 1
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}