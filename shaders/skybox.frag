#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    
    FragColor = vec4(texture(skybox, TexCoords).rgb, 1.0);
}