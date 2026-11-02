#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

std::string readFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Impossible to open : " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);

    const char* sourceCode = source.c_str();
    glShaderSource(shader, 1, &sourceCode, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Impossible to compile shader : " << infoLog << std::endl;
    }

    return shader;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Error: impossible to init GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        800,
        600,
        "OpenGL 3D",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Error: impossible to open window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Error: Impossible to init GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;

    float vertices[] = {
        0.0f,   0.7f,   0.0f,
        -0.7f,  -0.7f,  0.0f,
        0.7f,   -0.7f,  0.0f

    };

    // ---VBO---
    unsigned int VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );
    // ---VAO---
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3* sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))  {
        glClearColor(0.1f,0.1f,0.15f,0.1f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}