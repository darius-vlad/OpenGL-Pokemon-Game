#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform float transparency;
uniform bool isScene2;

//direct light 
struct DirLight {
    vec3 direction;
    vec3 color;

    float ambientStrength;
    float specularStrength;
};  
uniform DirLight dirLight;


//pont lights
struct PointLight {    
    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;  

    float ambientStrength;
    float specularStrength;
};  
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

//spotlight
struct SpotLight {
   vec3 position;            // Position of the spotlight in world space
   vec3 direction;          // Direction the spotlight is pointing
   vec3 color;              // Color of the light

    float ambientStrength;        // Ambient light strength
    float specularStrength;       // Specular shininess exponent
    float constant;               // Constant attenuation factor
    float linear;                 // Linear attenuation factor
    float quadratic;              // Quadratic attenuation factor

    float innerCutoff;            // Cosine of the inner cutoff angle (in radians)
    float outerCutoff;            // Cosine of the outer cutoff angle (in radians)
};
uniform SpotLight spotlight;


vec3 outDbg;

vec3 totalAmbient = vec3(0.0f , 0.0f ,0.0f);
vec3 totalDiffuse = vec3(0.0f , 0.0f , 0.0f);
vec3 totalSpecular = vec3(0.0f ,0.0f,0.0f);

void computeDirLight()
{

     vec3 diffuse;
     vec3 specular;
     vec3 ambient;

    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(dirLight.direction, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    vec3 halfwayDir = normalize(lightDirN + viewDir);

    //compute ambient light
    ambient = dirLight.ambientStrength * dirLight.color;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * dirLight.color;
    
    //compute specular light
    //vec3 reflectDir = reflect(-lightDirN, normalEye);
    //float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);

    float spec = pow(max(dot(normalEye, halfwayDir), 0.0), 32.0f);

    specular = dirLight.specularStrength* spec * dirLight.color;

    totalAmbient += ambient;
    totalDiffuse += diffuse;
    outDbg = totalDiffuse;
    totalSpecular += specular;

}

void computePointLight(PointLight light)
{
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    // Compute eye space coordinates for the fragment position
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(fNormal);
    vec3 fragPosViewSpace = (view * model * vec4(fPosition, 1.0f)).xyz;
    // Compute the light direction (from the fragment to the light)
    vec4 lightPosViewSpace = view * vec4(light.position, 1.0f); // Transform light position to view space
    vec3 lightDir = normalize(lightPosViewSpace.xyz - fragPosViewSpace); // Direction to the light in view space

    // Compute the view direction (in eye space)
    vec3 viewDir = normalize(-fPosEye.xyz);

    // Compute the halfway vector for specular (for Blinn-Phong lighting)
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Compute the distance from the fragment to the light for attenuation
    float distance = length(light.position - (model * vec4(fPosition, 1.0f)).xyz);

    // Compute attenuation using the inverse square law
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Compute ambient light
    ambient = light.ambientStrength * light.color;

    // Compute diffuse light (Lambertian reflection)
    float diff = max(dot(normalEye, lightDir), 0.0);
    diffuse = diff * light.color;

    // Compute specular light (Blinn-Phong)
    float spec = pow(max(dot(normalEye, halfwayDir), 0.0), 32.0f);
    specular = light.specularStrength *spec * light.color;

    // Apply attenuation to ambient, diffuse, and specular components
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Add the light components to the total light for the fragment
    totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;
}



void computeSpotLight(SpotLight light)
{
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    vec3 normalEye = normalize(fNormal);

    // Transform fragment and light positions to view space
    vec3 fragPosViewSpace = (view * model * vec4(fPosition, 1.0f)).xyz;
    vec3 lightPosViewSpace = (view * vec4(light.position, 1.0f)).xyz;

    // Transform light direction to view space
    vec3 lightDirViewSpace = normalize((view * vec4(light.direction, 0.0f)).xyz);

    // Compute light direction
    vec3 lightDir = normalize(lightPosViewSpace - fragPosViewSpace);

    // Compute view direction
    vec3 viewDir = normalize(-fragPosViewSpace);

    // Compute halfway vector for Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Attenuation
    float distance = length(light.position - (model * vec4(fPosition, 1.0f)).xyz);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight effect (using cutoff angles)
    float theta = dot(lightDir, -lightDirViewSpace); // Use transformed light direction in view space
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    // Ambient
    ambient = light.ambientStrength * light.color;

    // Diffuse
    float diff = max(dot(normalEye, lightDir), 0.0);
    diffuse = diff * light.color;

    // Specular
    float spec = pow(max(dot(normalEye, halfwayDir), 0.0), 32.0);
    specular = light.specularStrength * spec * light.color;

    // Apply attenuation and spotlight intensity
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    // Accumulate light components
    totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;
}


float computeShadow()
{
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    if (normalizedCoords.z > 1.0f)
        return 0.0f;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
   // Check whether current frag pos is in shadow
    float bias = 0.005f;
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    return shadow;
}


float computeFog()
{

vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
 float fogDensity = 0.018f;
 if(isScene2 == false)
 fogDensity = 0.0f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}


void main() 
{

    computeDirLight();
    for(int i = 0 ; i < NR_POINT_LIGHTS ; i++)
    {
        computePointLight(pointLights[i]);
    }

    computeSpotLight(spotlight);
    //compute final vertex color
    totalAmbient *= texture(diffuseTexture, fTexCoords).rgb;
	totalDiffuse *= texture(diffuseTexture, fTexCoords).rgb;
	totalSpecular *= texture(specularTexture, fTexCoords).rgb;

    float shadow = computeShadow();
    float fogFactor = computeFog();
    vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
	vec3 color = min((totalAmbient + (1.0f - shadow)*totalDiffuse) + (1.0f - shadow)*totalSpecular, 1.0f);


     fColor = vec4(mix(fogColor , color, fogFactor), transparency);
     //fColor = vec4(outDbg, 1.0f);

}
