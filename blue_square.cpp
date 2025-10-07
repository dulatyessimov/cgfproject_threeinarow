#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Vertex data for a blue square (position + color)
float vertices[] = {
    // positions       // colors
   -0.25f,  0.25f, 0.0f,  0.0f, 0.0f, 1.0f,
    0.25f,  0.25f, 0.0f,  0.0f, 0.0f, 1.0f,
    0.25f, -0.25f, 0.0f,  0.0f, 0.0f, 1.0f,
   -0.25f, -0.25f, 0.0f,  0.0f, 0.0f, 1.0f
};

unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

out vec3 fColor;

void main() {
    gl_Position = vec4(vPosition, 1.0);
    fColor = vColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 fColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(fColor, 1.0);
}
)";

int main() {
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "Blue Square", nullptr, nullptr);
    if(!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.8f,0.8f,0.8f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}
