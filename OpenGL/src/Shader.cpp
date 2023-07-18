#include "Shader.h"
#include <GL/glew.h>
#include "Renderer.h"
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>


Shader::Shader(const std::string& filepath) :
    m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShaderSource(filepath);
	m_RendererID = CreateShaderProgram(source.VertexShaderSource, source.FragmentShaderSource);
}

Shader::~Shader() {
    GLCALL(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    GLCALL(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
    GLCALL(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCALL(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    GLCALL(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "[OpenGL Warning]:" << name << " doesn't exist!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}

ShaderProgramSource Shader::ParseShaderSource(const std::string& filepath)
{
    enum class ShaderType {
        None = -1,
        Vertex = 0,
        Fragment = 1
    };

    std::stringstream ss[2];
    std::string line;
    ShaderType type = ShaderType::None;
    std::ifstream stream(filepath);
    while (std::getline(stream, line)) {
        if (line.find("#vertex") != std::string::npos)
            type = ShaderType::Vertex;
        else if (line.find("fragment") != std::string::npos)
            type = ShaderType::Fragment;
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    return{ ss[0].str(),ss[1].str() };
}

unsigned int Shader::CompileShader(const std::string & source, unsigned int type)
{
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(shader, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;

}

int Shader::CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}