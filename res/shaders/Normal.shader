#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_Normal;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0);
    v_Normal = normal;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;

void main() {
    color.r = abs(v_Normal.r);
    color.g = abs(v_Normal.g);
    color.b = abs(v_Normal.b);
    if (dot(v_Normal, vec3(1, 1, 1)) < 0.0)
        color.rgb *= 0.8;
    color.a = 1.0;
}