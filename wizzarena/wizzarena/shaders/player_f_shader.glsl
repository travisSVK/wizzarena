#version 420 core

in vec3 outColor;
in vec2 outTexCoord;

layout(binding = 0) uniform sampler2D colortexture;
layout(location = 0) out vec4 fragmentColor;

void main()
{
     fragmentColor = texture2D(colortexture, outTexCoord.xy);
     //fragmentColor.rgb = outColor;
     //fragmentColor.w = 1;
} 