#version 430

uniform sampler2D tex;

in vec2 v_texCoord;

out vec4 fragColor;

void main()
{
    vec4 color = texture(tex, v_texCoord);

    const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0 * 256.0), 1.0/(256.0 * 256.0 * 256.0));
    fragColor = vec4(vec3(dot(color, bitShift)), 1.0);
}