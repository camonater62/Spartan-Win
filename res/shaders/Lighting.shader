#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_Pos;
out vec3 v_Normal;
out vec3 v_Color;

uniform mat4 u_MVP;

void main() {
    v_Pos = position;
    gl_Position = u_MVP * vec4(position, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(u_MVP)));
    v_Normal = normalize(normalMatrix * normal);

    // Base
    v_Color.rgb = abs(normal);
    vec3 ones = vec3(1, 1, 1);
    if (dot(normal, ones) < 0.0)
        v_Color.rgb = ones - v_Color.rgb;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Pos;
in vec3 v_Normal;
in vec3 v_Color;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor;

    // Diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_Pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - v_Pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_LightColor;

    vec3 result = (ambient + diffuse + specular) * v_Color.rgb;
    color = vec4(result, 1.0);
}