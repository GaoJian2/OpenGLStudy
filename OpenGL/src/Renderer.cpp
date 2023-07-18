#include<iostream>
#include "Renderer.h"

void GLClearError() {
    while (glGetError());
}

bool GLGetError(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error](" << error << "):" << function << " " << file << " " << line << " " << std::endl;
        return false;
    }
    return true;
}