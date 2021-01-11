#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vFragPos;
out vec4 vColor;
out vec3 vNormal;

uniform mat4 u_V;
uniform mat4 u_P;

void main()
{
    gl_Position = u_P * u_V * vec4(position, 1.0f);
    vColor = vec4(position.y / 1.2, 0.2 + position.y / 1.2, 0.1 + position.y / 1.0, 1.0f);
    vFragPos = position;
    vNormal = normal;
}