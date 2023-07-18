#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include <stdio.h>
#include<cassert>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

struct ShaderProgramSource {
    std::string VertexShaderSource;
    std::string FragmentShaderSource;
};

static ShaderProgramSource ParseShaderSource(const std::string& filepath)
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

unsigned int CompileShader(const std::string& source, unsigned int type)
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
        std::cout << "Failed to compile "<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;

}

static int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
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

int main(void)
{
    {
        GLFWwindow* window;

        /* Initialize the library */
        if (!glfwInit())
            return -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK)
            return -1;

        float positions[12] = {
            -0.5f, -0.5f,
            0.5f,-0.5f,
            0.5f, 0.5f,
            -0.5f,0.5f,
        };

        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        unsigned int indices[6]{
            0, 1, 2,
            2, 3, 0
        };

        IndexBuffer ib(indices, 6);


        ShaderProgramSource shaderSrc = ParseShaderSource("resources/shaders/BasicShader.shader");

        unsigned int shaderProgram = CreateShaderProgram(shaderSrc.VertexShaderSource, shaderSrc.FragmentShaderSource);
        GLCALL(glUseProgram(shaderProgram));

        GLCALL(int location = glGetUniformLocation(shaderProgram, "u_Color"));
        ASSERT(location != -1);
        GLCALL(glUniform4f(location, 0.0f, 0.5f, 0.0f, 1.0f));

        GLCALL(glUseProgram(0));
        GLCALL(glBindVertexArray(0));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            if (r >= 1.0f)
                increment = -0.05f;
            else if (r <= 0.0f)
                increment = 0.05f;

            r += increment;
            /* Render here */
            GLCALL(glClear(GL_COLOR_BUFFER_BIT));

            GLCALL(glUseProgram(shaderProgram));
            GLCALL(glUniform4f(location, r, 0.5f, 0.0f, 1.0f));

            va.Bind();
            ib.Bind();

            GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        GLCALL(glDeleteProgram(shaderProgram));
    }
    glfwTerminate();
    return 0;
}