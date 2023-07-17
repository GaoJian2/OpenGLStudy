#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include <stdio.h>
#include<cassert>

#define ASSERT(x) if(!(x)) throw std::runtime_error("OpenGL Error!");
#define GLCALL(x) GLClearError();\
x;\
ASSERT(GLGetError(#x, __FILE__, __LINE__));

void GLClearError() {
    while (glGetError());
}

bool GLGetError(const char* function, const char* file, int line ) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error](" << error << "):" << function << " " << file << " " << line << " " << std::endl;
        return false;
    }
    return true;
}

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

    unsigned int vao;
    GLCALL(glGenVertexArrays(1, &vao));
    GLCALL(glBindVertexArray(vao));

    unsigned int buffer;
    GLCALL(glGenBuffers(1, &buffer));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));


    unsigned int indices[6]{
        0, 1, 2,
        2, 3, 0
    };

    unsigned int ibo;
    GLCALL(glGenBuffers(1, &ibo));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    //参数说明:
    /*(index 0):代表这是顶点的第几个属性。这里只指定了顶点的位置属性且index标为第一个
    * (size 2):代表这是几维的顶点。这里是二维顶点。
    * (type float):属性类型
    * (normalized false):已经被归一化。例如颜色0~255，需要将此参数设置为true，归一化为0~1的浮点数
    * (stride 16):顶点之间（第n个顶点到第n+1个顶点）的偏移量。为了跳转到下一个顶点的内存地址。
    * (*pointer (void*)0):一个顶点中第n(n就是函数对应的这个属性)个属性的偏移量。
    */
    GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
    //index 就是上面函数的第一个参数
    GLCALL(glEnableVertexAttribArray(0));

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

        GLCALL(glBindVertexArray(vao));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCALL(glDeleteProgram(shaderProgram));

    glfwTerminate();
    return 0;
}