#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

uniform	vec3 lightDir;
uniform	vec3 lightColor;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;


vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float shadow;

void computeLightComponents()
{
	vec3 cameraPosEye = vec3(0.0f);

	vec3 normalEye = normalize(fNormal);

	vec3 lightDirN = normalize(lightDir);

	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	ambient = ambientStrength * lightColor;

	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeShadow() {
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

float computeFog()
{
	float fogDensity = 0.015f;
	float fragmentDistance = length(fPosEye.xyz);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
	computeLightComponents();

	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f); // orange

	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	shadow = computeShadow();

	vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray fog

	fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
}