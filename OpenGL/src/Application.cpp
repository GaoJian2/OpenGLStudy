#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

#define ASSERT(x) if(!(x)) std::cin.get();
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    float positions[12] = {
        -0.5f, -0.5f,
        0.5f,-0.5f,
        0.5f, 0.5f,
        -0.5f,0.5f,
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);


    unsigned int indices[6]{
        0, 1, 2,
        2, 3, 0
    };

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    //����˵��:
    /*(index 0):�������Ƕ���ĵڼ������ԡ�����ָֻ���˶����λ��������index��Ϊ��һ��
    * (size 2):�������Ǽ�ά�Ķ��㡣�����Ƕ�ά���㡣
    * (type float):��������
    * (normalized false):�Ѿ�����һ����������ɫ0~255����Ҫ���˲�������Ϊtrue����һ��Ϊ0~1�ĸ�����
    * (stride 16):����֮�䣨��n�����㵽��n+1�����㣩��ƫ������Ϊ����ת����һ��������ڴ��ַ��
    * (*pointer (void*)0):һ�������е�n(n���Ǻ�����Ӧ���������)�����Ե�ƫ������
    */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    //index �������溯���ĵ�һ������
    glEnableVertexAttribArray(0);

    ShaderProgramSource shaderSrc = ParseShaderSource("resources/shaders/BasicShader.shader");

    unsigned int shaderProgram = CreateShaderProgram(shaderSrc.VertexShaderSource, shaderSrc.FragmentShaderSource);
    glUseProgram(shaderProgram);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}