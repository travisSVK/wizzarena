#version 420 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inTexCoord;

out vec3 outColor; // specify a color output to the fragment shader
out vec2 outTexCoord;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//uniform vec2 pos;

void main()
{
    gl_Position = vec4(inPos, 1.0);
    //gl_Position.x += pos.x;
    //gl_Position.y += pos.y;
    
	outColor = inColor.rgb;
    outTexCoord = inTexCoord;
}