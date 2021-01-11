#version 410 core

out vec4 color;

in vec3 vFragPos;
in vec4 vColor;
in vec3 vNormal;

struct Material {
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

uniform	int lightNumb;
uniform Light light;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;

void main()
{
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	vec3 diffuseColor = vec3(vColor);

    float distance = length(lightPos - vFragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = diffuseColor * light.ambient;
	ambient *= attenuation;
	result += ambient;

	vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(lightPos - vFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseColor * diff *  light.diffuse;
	diffuse *= attenuation;
	result += diffuse;

	vec3 viewDir = normalize(viewPos - vFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * spec * light.specular;
	specular *= attenuation;
	result += specular;

	color = vec4(result, 1.0f);
}