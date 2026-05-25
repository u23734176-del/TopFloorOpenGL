#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

uniform int mode;

void main()
{
    vec3 color = texture(screenTexture, TexCoord).rgb;

    
    
    

    if(mode == 0)
    {
        FragColor = vec4(color, 1.0);
    }

    
    
    

    else if(mode == 1)
    {
        float gray =
            (color.r + color.g + color.b) / 3.0;

        FragColor = vec4(gray, gray, gray, 1.0);
    }

    
    
    

    else if(mode == 2)
    {
        FragColor = vec4(
            1.0 - color.r,
            1.0 - color.g,
            1.0 - color.b,
            1.0
        );
    }

    
    
    

    else
    {
        FragColor = vec4(color, 1.0);
    }
}