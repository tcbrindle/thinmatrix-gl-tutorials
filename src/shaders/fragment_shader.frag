
#version 330 core

const int numLights = 4;

in vec2 pass_textureCoords;
in vec3 surfaceNormal;
in vec3 toLightVector[numLights];
in vec3 toCameraVector;
in float visibility;

out vec4 out_colour;

uniform sampler2D textureSampler;
uniform vec3 lightColour[numLights];
uniform vec3 attenuation[numLights];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

void main(void)
{
    vec4 textureColour = texture(textureSampler, pass_textureCoords);
    if (textureColour.a < 0.5) {
        discard;
    }

    vec3 unitNormal = normalize(surfaceNormal);
    vec3 unitVectorToCamera = normalize(toCameraVector);

    vec3 totalDiffuse = vec3(0.0f);
    vec3 totalSpecular = vec3(0.0f);

    for (int i = 0; i < numLights; i++) {
        float distance = length(toLightVector[i]);
        float attFactor = attenuation[i].x + attenuation[i].y * distance + attenuation[i].z * distance * distance;
        vec3 unitLightVector = normalize(toLightVector[i]);
        float nDotl = dot(unitNormal, unitLightVector);
        float brightness = max(nDotl, 0.0f);
        vec3 lightDirection = -unitLightVector;
        vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
        float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
        specularFactor = max(specularFactor, 0.0);
        float dampedFactor = pow(specularFactor, shineDamper);
        vec3 diffuse =  brightness * lightColour[i]/attFactor;
        totalDiffuse += diffuse;
        vec3 specular =  dampedFactor * reflectivity * lightColour[i]/attFactor;
        totalSpecular += specular;
    }
    totalDiffuse = max(totalDiffuse, 0.4);

    out_colour = vec4(totalDiffuse, 1.0) * textureColour + vec4(totalSpecular, 1.0);
    out_colour = mix(vec4(skyColour, 1.0), out_colour, visibility);
}
