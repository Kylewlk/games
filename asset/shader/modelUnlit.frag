#version 430 core

uniform sampler2D tex;
uniform vec3 color;
uniform bool useTexture;

layout(location= 0) in vec3 WorldPos;
layout(location= 1) in vec3 Normal;
layout(location= 2) in vec2 TexCoords;

out vec4 fragColor;

void main()
{
    if (useTexture)
    {
        fragColor = texture(tex, TexCoords);
    }
    else
    {
        fragColor = vec4(pow(color, vec3(1.0/2.2)), 1.0);
    }

    //    fragColor = vec4(1.0);
}