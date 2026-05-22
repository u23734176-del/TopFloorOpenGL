#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 color;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

    float diff = max(dot(normalize(Normal), lightDir), 0.0);

    vec3 base = vec3(0.2, 0.8, 0.3);

    color = vec4(base * diff, 1.0);
}