#version 430
uniform mat4 viewProj;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightViewProj;

layout(location= 0) in vec3 pos;
layout(location= 1) in vec3 normal;
layout(location= 2) in vec2 texCoord;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vPositionInLight;

void main()
{
    vWorldPos = (model * vec4(pos, 1.0)).xyz;
    vNormal = normalize(normalMatrix * normal);
    vTexCoord = texCoord;
    vPositionInLight = lightViewProj*vec4(vWorldPos, 1.0);
    
    gl_Position = viewProj * vec4(vWorldPos, 1.0);
}