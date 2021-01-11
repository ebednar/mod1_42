#version 410 core

layout(location = 0) in vec3 position;

out vec4 vColor;

uniform mat4 u_V;
uniform mat4 u_P;

void main()
{
    gl_Position = u_P * u_V * vec4(position, 1.0f);
    vColor = vec4(0.0f, 0.5f, 1.0f, 0.5f);
}