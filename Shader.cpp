#include "Shader.h"

#include "Renderer.h"

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>

std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> Shader::s_ShaderMap;

Shader::Shader(const std::string &filepath)
    : m_RendererId(0)
    , m_FilePath(filepath) {

    if (s_ShaderMap.find(filepath) != s_ShaderMap.end()) {
        m_RendererId = s_ShaderMap[filepath].first;
        s_ShaderMap[filepath].second++;
        return;
    }

    ShaderProgramSource source = ParseShader(filepath);
    std::cout << "Compiling " << filepath << std::endl;
    m_RendererId = CreateShader(source.VertexSource, source.FragmentSource);

    s_ShaderMap[filepath] = std::make_pair(m_RendererId, 1);
}

Shader::~Shader() {
    if (s_ShaderMap.find(m_FilePath) != s_ShaderMap.end()) {
        s_ShaderMap[m_FilePath].second--;
        if (s_ShaderMap[m_FilePath].second == 0) {
            s_ShaderMap.erase(m_FilePath);
            GLCall(glDeleteProgram(m_RendererId));
        }
    } else {
        std::cout << "Shader not found in map during destruction" << std::endl;
    }
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source) {
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << "shader!" << std::endl;
        std::cout << message << std::endl;
        return 0;
    }

    return id;
}

ShaderProgramSource Shader::ParseShader(const std::string &filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else if (type != ShaderType::NONE) {
            ss[(int) type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CreateShader(
    const std::string &vertexShader, const std::string &fragmentShader) {

    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void Shader::Bind() const {
    GLCall(glUseProgram(m_RendererId));
}
void Shader::UnBind() const {
    GLCall(glUseProgram(0));
}

// Set uniforms
void Shader::SetUniform1i(const std::string &name, int value) {
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string &name, float value) {
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &matrix) {
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string &name) const {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }

    GLCall(int location = glGetUniformLocation(m_RendererId, name.c_str()));
    if (location == -1) {
        std::cerr << "Warning: uniform \'" << name << "\' doesn't exist!" << std::endl;
    }

    m_UniformLocationCache[name] = location;
    return location;
}