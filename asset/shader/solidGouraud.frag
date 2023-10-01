#version 430

layout(location = 2) uniform vec4 color;

layout(location= 0) in vec3 vColor;

out vec4 fragColor;

void main()
{
    fragColor.rgb = (color .rgb * vColor) * color.a; // premultiply alpha
    fragColor.a = color.a;
}