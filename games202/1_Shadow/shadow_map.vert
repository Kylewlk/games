#version 430
layout(location= 1) uniform mat4 viewProj;
layout(location= 2) uniform mat4 model;

layout(location= 0) in vec3 pos;
layout(location= 1) in vec3 normal;
layout(location= 2) in vec2 texCoord;

out vec3 vWorldPos;

void main()
{
    vWorldPos = (model * vec4(pos, 1.0)).xyz;
    gl_Position = viewProj * vec4(vWorldPos, 1.0);
}
