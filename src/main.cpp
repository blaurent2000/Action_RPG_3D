#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

unsigned int createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0)
    return 0;

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    int success;
    char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Error linking program: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;

}

GLFWwindow* initGLFWandCreateWindow() {
    if (!glfwInit()) {
        std::cerr << "Error: impossible to init GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow( 800, 600, "OpenGL 3D",nullptr,nullptr);

    if (!window) {
        std::cerr << "Error: impossible to open window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    return window;
}

bool initGlad() {
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Error: Impossible to init GLAD" << std::endl;
        return false;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEPTH_TEST);

    return true;
}

void createCube(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO) {
    float vertices[] = {
        // Positions          // Couleurs
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.0f
    };

    unsigned int indices[] = {
        // Face arrière
        0, 1, 2,
        2, 3, 0,
        // Face avant
        4, 6, 5,
        6, 4, 7,
        // Face gauche
        0, 3, 7,
        7, 4, 0,
        // Face droite
        1, 5, 6,
        6, 2, 1,
        // Face inférieure
        0, 4, 5,
        5, 1, 0,
        // Face supérieure
        3, 2, 6,
        6, 7, 3
    };

     // ---VBO---
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // ---VAO---
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );
    // Position
    glVertexAttribPointer(
        0,                  // Location de la position
        3,                  // X, Y, Z
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),  // Taille totale d'un sommet
        (void*)0            // Début du sommet
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3*sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main() {

    GLFWwindow* window = initGLFWandCreateWindow();
    if (!window)
        return -1;

    if (!initGlad()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // ---Get shaders from files and compile---
    std::string vertexSource = readFile("shaders/vertex.vert");
    std::string fragmentSource = readFile("shaders/fragment.frag");
    unsigned int shaderProgram = createShaderProgram(vertexSource, fragmentSource);

    int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    unsigned int VAO, VBO, EBO;
    createCube(VAO, VBO, EBO);
   
    glm::mat4 model(1.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),  // Position de la caméra
        glm::vec3(0.0f, 0.0f, 0.0f),  // Point regardé
        glm::vec3(0.0f, 1.0f, 0.0f)   // Direction du haut
    );
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f
    );

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
        // Caméra et perspective
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Premier cube
        glm::mat4 model1(1.0f);
        model1 = glm::translate(model1, glm::vec3(-1.0f, 0.0f, 0.0f));
        model1 = glm::rotate(model1, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);
        // Deuxième cube
        glm::mat4 model2(1.0f);
        model2 = glm::translate(model2, glm::vec3(1.0f, 0.0f, 0.0f));
        model2 = glm::rotate(model2, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}