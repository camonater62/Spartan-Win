#shader vertex
#version 330 core

uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

in vec4 VertexColor;
in vec3 VertexNormal;
in vec4 VertexPosition;

out vec4 Color;
out vec3 Normal;

void main() {
    Color = VertexColor;

    Normal = normalize(NormalMatrix * VertexNormal);

    gl_Position = MVPMatrix * VertexPosition;
}

#shader fragment
#version 330 core

uniform vec4 Ambient; 
uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 HalfVector;
uniform float Shininess;
uniform float Strength;

in vec4 Color;
in vec3 Normal;

out vec4 FragColor;

void main() {
    float diffuse = max(dot(Normal, LightDirection), 0.0);
    float specular = pow(max(dot(Normal, HalfVector), 0.0), Shininess);

    vec3 scatteredLight = Ambient + LightColor * diffuse;
    vec3 reflectedLight = LightColor * specular * Strength;

    vec3 rgb = min(Color.rgb * scatteredLight + reflectedLight, vec3(1.0));
    FragColor = vec4(rgb, Color.a);
}