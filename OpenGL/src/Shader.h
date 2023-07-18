#pragma once
#include<string>
#include<unordered_map>


struct ShaderProgramSource {
	std::string VertexShaderSource;
	std::string FragmentShaderSource;
};

class Shader {
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::unordered_map<std::string, unsigned int> m_UniformLocationCache;
public:
	Shader(const std::string& filename);
	~Shader();

	void Bind() const;
	void UnBind() const;

	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	ShaderProgramSource ParseShaderSource(const std::string& filepath);
	unsigned int CompileShader(const std::string& source, unsigned int type);
	int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int GetUniformLocation(const std::string& name);
};
