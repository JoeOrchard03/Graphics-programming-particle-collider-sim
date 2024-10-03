#version 330 core

out vec4 color;
in vec2 textureCoords;
uniform sampler2D texture0;

const float offset = 1.0 / 300.0;


void main()
{
//TAKEN FROM: https://learnopengl.com/Advanced-OpenGL/Framebuffers
	vec2 offsets[9] = vec2[](
			vec2(-offset,offset),
			vec2(0.0f,offset),
			vec2(offset,offset),
			vec2(-offset,0.0f),
			vec2(0.0f,0.0f),
			vec2(offset,0.0f),
			vec2(-offset,-offset),
			vec2(0.0f, -offset),
			vec2(offset, -offset)
		);


		float kernal[9] = float[](
			-1,-1,-1,
			-1, 9,-1,
			-1,-1,-1
		);
		
		
		/*EDGE DETECTION -8 inverts the colour of the middle pixel of a fragment of 9 like seen below, setting the others to 1 gives a blur effect making the colours inverted and blurred.
		float kernal[9] = float[](
			1, 1, 1,
			1, -8, 1,
			1, 1, 1
		);*/
		

		vec3 sampleTex[9];
		for(int i = 0; i<9; i++){
			sampleTex[i] = vec3(texture(texture0, textureCoords.st + offsets[i]));
		}

		vec3 col = vec3(0.0);
		for(int i = 0; i< 9; i++){
			col += sampleTex[i] * kernal[i];
			}
		color = vec4(col,1.0);
}