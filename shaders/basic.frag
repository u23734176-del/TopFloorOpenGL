#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

out vec4 FragColor;

// =====================================
// Light Structures (mirror LightSet.h)
// =====================================
struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float innerCutoff;
    float outerCutoff;
};

// =====================================
// Uniforms
// =====================================
uniform DirectionalLight directional;
uniform vec3 ambient;

const int MAX_POINT_LIGHTS = 4;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

uniform SpotLight spotlight;
uniform bool spotlightEnabled;

uniform bool isNight;
uniform vec3 cameraPos;
uniform sampler2D shadowMap;

// === Material properties (default) ===
uniform vec3 objectColor = vec3(0.5, 0.5, 0.5);
uniform float shininess = 32.0;


// PCF Shadow Calculation
// =====================================
float calcShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
    {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(norm, lightDir)), 0.0015);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(
                shadowMap,
                projCoords.xy + vec2(x, y) * texelSize
            ).r;
            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

// =====================================

// Blinn-Phong Calculation for Directional Light
// =====================================
vec3 calcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * objectColor;
    
    // Specular (Blinn-Phong: half-vector approach)
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
    vec3 specular = spec * light.color;
    
    return diffuse + specular;
}

// =====================================
// Blinn-Phong Calculation for Point Light (with attenuation)
// =====================================
vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Attenuation: 1.0 / (1.0 + linear*d + quadratic*d²)
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * objectColor * light.intensity;
    
    // Specular
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
    vec3 specular = spec * light.color * light.intensity;
    
    // Apply attenuation
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}

// =====================================
// Blinn-Phong Calculation for Spotlight
// =====================================
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Spotlight cone calculation
    float theta = dot(lightDir, normalize(-light.direction));
    
    // Fade: smooth transition from inner to outer cone
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    if (intensity <= 0.0) return vec3(0.0);
    
    // Distance attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (distance * distance + 1.0);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * objectColor * light.intensity;
    
    // Specular
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
    vec3 specular = spec * light.color * light.intensity;
    
    // Apply cone intensity and distance attenuation
    diffuse *= intensity * attenuation;
    specular *= intensity * attenuation;
    
    return diffuse + specular;
}

// =====================================
// Main
// =====================================
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 sunDir = normalize(-directional.direction);
    float shadow = calcShadow(FragPosLightSpace, norm, sunDir);
    
    // === Start with ambient ===
    vec3 result = ambient * objectColor;
    
    // === Add directional light (sun/moon), shadowed by the depth map ===
    result += (1.0 - shadow) * calcDirectionalLight(directional, norm, viewDir);
    
    // === Add point lights (only at night) ===
    if (isNight) {
        for (int i = 0; i < numPointLights; ++i) {
            result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
        }
    }
    
    // === Add spotlight (only if enabled) ===
    if (spotlightEnabled) {
        result += calcSpotLight(spotlight, norm, FragPos, viewDir);
    }
    
    FragColor = vec4(result, 1.0);
}