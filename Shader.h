#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
private:
    unsigned int m_RendererId;
    std::string m_FilePath;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    // string: filepath, pair: rendererId, reference count
    static std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> s_ShaderMap;

public:
    Shader(const std::string &filepath);
    ~Shader();

    void Bind() const;
    void UnBind() const;

    // Set uniforms
    void SetUniform1i(const std::string &name, int value);
    void SetUniform1f(const std::string &name, float value);
    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string &name, const glm::mat4 &matrix);

private:
    int GetUniformLocation(const std::string &name) const;

    ShaderProgramSource ParseShader(const std::string &filepath);
    unsigned int CompileShader(unsigned int type, const std::string &source);
    unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
};