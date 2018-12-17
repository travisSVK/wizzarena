#version 420 core

in vec4 ourColor; // the input variable from the vertex shader (same name and same type)
in vec2 TexCoord; // the input variable from the vertex shader (same name and same type)
layout(binding = 0) uniform sampler2D groundTexture;
layout(binding = 1) uniform sampler2D normalLavaTexture;
layout(binding = 2) uniform sampler2D highlightLavaTexture;
layout(location = 0) out vec4 fragmentColor;

uniform float radius;
uniform float width;
uniform float height;
uniform float timeTillGrow;

void main()
{
     float centerX = width / 2.0f;
     float centerY = height / 2.0f;
     float normalizedX = abs(gl_FragCoord.x - centerX);
     float normalizedY = abs(gl_FragCoord.y - centerY);

     float weight = timeTillGrow; 
     if(weight > 1.0f)
     {
        weight = 1.0f;
     }
     float l = sqrt(pow(normalizedX, 2) + pow(normalizedY, 2));
     if (l > radius)
     {
        fragmentColor = mix(texture(normalLavaTexture, TexCoord), texture(highlightLavaTexture, TexCoord), weight);
         //fragmentColor = texture2D(normalLavaTexture, TexCoord.xy);
     }
     else
     {
        fragmentColor = texture2D(groundTexture, TexCoord.xy);
     }

} 