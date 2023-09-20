#version 430 core

uniform sampler2D tex;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 cameraPosition;
uniform bool useTexture;
uniform bool halfLambert;

layout(location= 0) in vec3 WorldPos;
layout(location= 1) in vec3 Normal;
layout(location= 2) in vec2 TexCoords;

out vec4 fragColor;

void main()
{
    vec3 color;
    if (useTexture) {
        color = pow(texture(tex, TexCoords).rgb, vec3(2.2));
    } else {
        color = kd;
    }

    vec3 ambient = 0.05 * color;

    vec3 lightDir = normalize(lightPosition - WorldPos);
    vec3 normal = normalize(Normal);
    float d = 0.0;
    if (halfLambert)
    {
        d = dot(lightDir, normal) * 0.5 + 0.5;
    }
    else
    {
        d = max(dot(lightDir, normal), 0.0);
    }
    float light_atten_coff = lightIntensity / (length(lightPosition - WorldPos) + 0.1);
    vec3 diffuse =  color * lightColor * d * light_atten_coff;

    vec3 viewDir = normalize(cameraPosition - WorldPos);
    float spec = 0.0;
    // vec3 reflectDir = reflect(-uLightDir, normal);
    // spec = pow (max(dot(viewDir, reflectDir), 0.0), 35.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow (max(dot(viewDir, halfwayDir), 0.0), 35.0);
    vec3 specular = ks * lightColor * spec * light_atten_coff;

    fragColor = vec4(pow((ambient + diffuse + specular), vec3(1.0/2.2)), 1.0);
}