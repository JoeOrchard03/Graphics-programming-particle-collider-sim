#include "LoadModel.h";

void LoadBufferObjects(std::vector<Vertex> vertices, std::vector<unsigned> indices, GLuint VBO, GLuint VAO, GLuint EBO)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//Tells OpenGL how to interpret the vertex data
	glVertexAttribPointer(
		0,			//Specifies which attribute to configure
		3,			//size (of attribute - 3D vector, so 3)
		GL_FLOAT,	//data type
		GL_FALSE,	//normalised? (between -1 and 1? if not, GL_TRUE will map automatically)
		sizeof(Vertex),			//stride (how far from start of one attribute to the next - beginning of one vertex to the next)
		(void*)0	//offset (how far from start of array buffer does first vertex start?)
	);
	//Enables the vertex attribute at the specified location
	glEnableVertexAttribArray(0);

	//Configures 2nd attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1); //attribute in location 0

	//Configures 3rd attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
}