// Standard C++ includes
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

// OpenGL dependencies (assuming you are using GLEW/GLAD and freeGLUT)
// Note: You must replace 'GLAD/glad.h' with 'GLEW/glew.h' if using GLEW.
#include <GLAD/glad.h>
#include <GL/freeglut.h>

// --- Constants and Configuration ---

const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 500;

// Maximum vertices needed for the scene (Circle/Ellipse = 1 center + 360 points, Square/Triangle = few)
const int NUM_SEGMENTS = 360; // For circle/ellipse smoothness

// Simple structure to hold vertex data (position and color)
struct Vertex {
    GLfloat position[2]; // (x, y)
    GLfloat color[4];    // (r, g, b, a)
};

// Global variables for OpenGL objects
GLuint VBO, VAO;
GLuint program;

// Uniform location
GLint mvpLoc;

// Global container for all vertices
std::vector<Vertex> vertices;

// --- Utility Functions ---

/**
 * @brief Reads a shader file and returns its content as a string.
 */
std::string readShaderFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * @brief Compiles a shader from source.
 */
GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

/**
 * @brief Links compiled shaders into a program.
 */
GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = readShaderFile(vertexPath);
    std::string fragmentSource = readShaderFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty()) return 0;

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    // Clean up shaders as they're now linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// --- Shape Generation Functions ---

/**
 * @brief Adds vertices for a Circle using GL_TRIANGLE_FAN with red shading.
 * The center is (0.0, 0.5), radius 0.2.
 */
void generateCircle() {
    // Starting index for the circle in the VBO
    GLuint start_index = vertices.size();

    // 1. Center vertex (for GL_TRIANGLE_FAN)
    // Position: (0.0, 0.5)
    // Color: Solid red (R=1.0)
    vertices.push_back({
        {0.0f, 0.5f},
        {1.0f, 0.0f, 0.0f, 1.0f}
    });

    float radius = 0.2f;

    // 2. Perimeter vertices
    for (int i = 0; i <= NUM_SEGMENTS; ++i) {
        float angle = (float)i / NUM_SEGMENTS * 2.0f * M_PI;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        // Vary the red color intensity (R) along the perimeter
        // The angle varies from 0 to 2*pi.
        // We'll vary R from 0.5 (dim) to 1.0 (bright) and back.
        float red_intensity = 0.75f + 0.25f * cos(angle); 

        vertices.push_back({
            {x, y + 0.5f}, // y + 0.5 to offset to the center (0.0, 0.5)
            {red_intensity, 0.0f, 0.0f, 1.0f}
        });
    }
}

/**
 * @brief Adds vertices for an Ellipse using GL_TRIANGLE_FAN.
 * Center is (0.5, -0.5), radiusX 0.2, radiusY 0.12 (60%).
 */
void generateEllipse() {
    // Starting index for the ellipse
    GLuint start_index = vertices.size();

    // 1. Center vertex (for GL_TRIANGLE_FAN)
    // Position: (0.5, -0.5)
    // Color: Yellow
    vertices.push_back({
        {0.5f, -0.5f},
        {1.0f, 1.0f, 0.0f, 1.0f}
    });

    float radiusX = 0.2f;
    float radiusY = 0.12f; // Scaled down to 60% of X radius

    // 2. Perimeter vertices
    for (int i = 0; i <= NUM_SEGMENTS; ++i) {
        float angle = (float)i / NUM_SEGMENTS * 2.0f * M_PI;
        float x = radiusX * cos(angle);
        float y = radiusY * sin(angle);

        // Color: Orange
        vertices.push_back({
            {x + 0.5f, y - 0.5f}, // Offset to the center (0.5, -0.5)
            {1.0f, 0.6f, 0.0f, 1.0f}
        });
    }
}

/**
 * @brief Adds vertices for a rotating square made of multiple smaller squares.
 * Center is (-0.5, -0.5), size 0.4.
 * Note: Since we are not animating, we'll draw one fully colored square.
 * We'll use two GL_TRIANGLES for the square.
 */
void generateSquare() {
    // We'll draw a square centered at (-0.5, -0.5)

    // Square size
    float size = 0.4f;
    float halfSize = size / 2.0f;
    float centerX = -0.5f;
    float centerY = -0.5f;

    // Define vertices for the base square (two triangles)
    // Colors will vary from black (center) to white (edges)
    // Top-right, Top-left, Bottom-left, Bottom-right
    Vertex square_vertices[] = {
        // Triangle 1 (Top-Right, Top-Left, Bottom-Left)
        {{centerX + halfSize, centerY + halfSize}, {1.0f, 1.0f, 1.0f, 1.0f}}, // White (Corner)
        {{centerX - halfSize, centerY + halfSize}, {0.8f, 0.8f, 0.8f, 1.0f}}, // Light Gray (Corner)
        {{centerX - halfSize, centerY - halfSize}, {0.6f, 0.6f, 0.6f, 1.0f}}, // Gray (Corner)
        
        // Triangle 2 (Top-Right, Bottom-Left, Bottom-Right)
        {{centerX + halfSize, centerY + halfSize}, {1.0f, 1.0f, 1.0f, 1.0f}}, // White (Corner)
        {{centerX - halfSize, centerY - halfSize}, {0.6f, 0.6f, 0.6f, 1.0f}}, // Gray (Corner)
        {{centerX + halfSize, centerY - halfSize}, {0.4f, 0.4f, 0.4f, 1.0f}}  // Dark Gray (Corner)
    };

    vertices.insert(vertices.end(), std::begin(square_vertices), std::end(square_vertices));
}

/**
 * @brief Adds vertices for a Triangle.
 * Center is (-0.5, 0.5), size 0.3.
 */
void generateTriangle() {
    float size = 0.3f;
    float centerX = -0.5f;
    float centerY = 0.5f;

    // Vertices defined using angle increments of 2*pi/3 starting at pi/2 (top)
    // Color: Green (solid)
    Vertex triangle_vertices[] = {
        // Top point (angle pi/2)
        {{centerX + size * cos(M_PI / 2.0f), centerY + size * sin(M_PI / 2.0f)}, {0.0f, 1.0f, 0.0f, 1.0f}}, 
        // Bottom-left point (angle pi/2 + 2pi/3)
        {{centerX + size * cos(M_PI / 2.0f + 2.0f * M_PI / 3.0f), centerY + size * sin(M_PI / 2.0f + 2.0f * M_PI / 3.0f)}, {0.0f, 1.0f, 0.0f, 1.0f}}, 
        // Bottom-right point (angle pi/2 + 4pi/3)
        {{centerX + size * cos(M_PI / 2.0f + 4.0f * M_PI / 3.0f), centerY + size * sin(M_PI / 2.0f + 4.0f * M_PI / 3.0f)}, {0.0f, 1.0f, 0.0f, 1.0f}} 
    };

    vertices.insert(vertices.end(), std::begin(triangle_vertices), std::end(triangle_vertices));
}


/**
 * @brief Setup function called once.
 */
void init() {
    // Load and compile shaders
    program = createShaderProgram("shader.vert", "shader.frag");
    if (program == 0) {
        exit(EXIT_FAILURE);
    }
    glUseProgram(program);
    
    // Get uniform location for MVP matrix
    mvpLoc = glGetUniformLocation(program, "model_view_projection");

    // Clear previous data
    vertices.clear();

    // Generate all shape vertices
    generateCircle();
    generateEllipse();
    generateSquare();
    generateTriangle();

    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // Bind VAO, then bind VBO and configure vertex attributes
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes
    
    // 1. vPosition (layout 0)
    // 2 components (x, y), type GL_FLOAT, offset 0
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // 2. vColor (layout 1)
    // 4 components (r, g, b, a), type GL_FLOAT, offset 2*sizeof(GLfloat)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Set background color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * @brief Display callback function.
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(VAO);

    // Identity Matrix for MVP (since coordinates are already normalized between -1 and 1)
    GLfloat identityMatrix[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, identityMatrix);


    // --- Drawing Shapes (using vertex offsets) ---
    
    // The number of vertices per shape type
    // Circle: 1 center + 361 perimeter points (NUM_SEGMENTS + 1) -> 362 vertices
    // Ellipse: 1 center + 361 perimeter points -> 362 vertices
    // Square: 6 vertices (2 triangles)
    // Triangle: 3 vertices

    // Offset 0: Circle (GL_TRIANGLE_FAN)
    glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS + 2);

    // Offset 362: Ellipse (GL_TRIANGLE_FAN)
    glDrawArrays(GL_TRIANGLE_FAN, NUM_SEGMENTS + 2, NUM_SEGMENTS + 2);

    // Offset 724: Square (GL_TRIANGLES)
    glDrawArrays(GL_TRIANGLES, (NUM_SEGMENTS + 2) * 2, 6);

    // Offset 730: Triangle (GL_TRIANGLES)
    glDrawArrays(GL_TRIANGLES, (NUM_SEGMENTS + 2) * 2 + 6, 3);


    glBindVertexArray(0);
    glUseProgram(0);
    
    // Swap buffers for smooth drawing (single buffering only required for Assignment 1)
    glutSwapBuffers(); 
}

/**
 * @brief Reshape callback function.
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

/**
 * @brief Main function.
 */
int main(int argc, char** argv) {
    // 1. Initialize GLUT
    glutInit(&argc, argv);
    
    // Request a Core Profile context (Modern OpenGL)
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    // 2. Window size requirement
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); 
    glutCreateWindow("Assignment 1: 2D Polygons with Color");

    // 3. Initialize GLAD (or GLEW)
    if (!gladLoadGLLoader((GLADloadproc)glutGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Set callbacks and initialize OpenGL state
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    // 4. Start the GLUT event loop
    glutMainLoop();

    return 0;
}
