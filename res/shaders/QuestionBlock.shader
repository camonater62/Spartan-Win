#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_Normal;
out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0);
    v_Normal = normal;
    v_TexCoord = texCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main() {
    color = texture(u_Texture, v_TexCoord);
    if (color.a < 0.1)
        discard;
    if (dot(v_Normal, vec3(1, 1, 1)) < 0.0)
        color.rgb *= 0.8;
    color.a = 1.0;
}