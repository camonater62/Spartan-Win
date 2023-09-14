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
    color.rgb = abs(v_Normal);
    vec3 ones = vec3(1, 1, 1);
    if (dot(v_Normal, ones) < 0.0)
        color.rgb = ones - color.rgb;
    color.a = 1.0;
}