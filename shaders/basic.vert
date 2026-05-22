#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 ViewDir;

void main()
{
    // Fragment position in world space
    FragPos = vec3(model * vec4(position, 1.0));
    
    // Transform normal to world space (account for non-uniform scaling)
    Normal = normalize(mat3(transpose(inverse(model))) * normal);
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}