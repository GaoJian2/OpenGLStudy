#pragma once
#include<GL/glew.h>
#include<iostream>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if(!(x)) throw std::runtime_error("OpenGL Error!");
#define GLCALL(x) GLClearError();\
x;\
ASSERT(GLGetError(#x, __FILE__, __LINE__));

void GLClearError();

bool GLGetError(const char* function, const char* file, int line);

class Renderer {
private:
public:
	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
};
