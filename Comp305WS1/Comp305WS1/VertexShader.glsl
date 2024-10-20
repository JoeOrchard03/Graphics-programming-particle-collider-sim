//Sets the version of OpenGL
#version 330 core
//Declare all input vertex attirbutes, each vertex is made of a set of 3d coordinates which is why you use vector 3, also set the location of the variable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor; //Specify color output to fragment shader

void main()
{
    //Set the output of the vertex shader using the position of the vector and giving it a w value of 1
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
}
