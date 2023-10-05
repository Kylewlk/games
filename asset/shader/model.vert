#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(location= 0) out vec3 WorldPos;
layout(location= 1) out vec3 Normal;
layout(location= 2) out vec2 TexCoords;

uniform mat4 viewProj;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(normalMatrix*aNormal);

    gl_Position =  viewProj * vec4(WorldPos, 1.0);
}