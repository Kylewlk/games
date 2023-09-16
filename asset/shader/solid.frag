#version 430

layout(location = 2) uniform vec4 color;


out vec4 fragColor;

void main()
{
    fragColor = color;
    fragColor.rgb *= color.a; // premultiply alpha
}