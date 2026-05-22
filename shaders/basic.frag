#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor;

void main()
{
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

    float diff = max(dot(normalize(Normal), lightDir), 0.2);

    vec3 result = objectColor * diff;

    FragColor = vec4(result, 1.0);
}