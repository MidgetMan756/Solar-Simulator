#include "window.h"

#include <iostream>
#include <stdexcept>

#include "globals.h"
#include "rendering.h"
#include "shaders.h"
#include "utils.h"

namespace SolarSim {

void initWindow() {
    // Init glfw
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialise GLFW");
    }

    // Let glfw know what we're using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create 1000x1000 window
    window = glfwCreateWindow(1000, 1000, "Solar Sim", nullptr, nullptr);

    // If window isn't made
    if (window == nullptr) {
        std::cerr << "Trouble making window";
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

    // Set current window context
    glfwMakeContextCurrent(window);

    // set up glad for colors and stuff
    gladLoadGL();

    // set up the current frame
    glViewport(0, 0, 1000, 1000);

    // Allows window to be resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Create dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Create Vertex Buffer Object (VBO) and save it's ID
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Create Vertex Array Object (VAO) and save it's ID
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);

    // Bind the Vertex Buffer Object to target
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // For masses
    // Create a shader object and save it's ID
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Attach the source code and compile
    const char* vertexSrc = Shaders::vertexShader;
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    // Create a shader object and save it's ID
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Attach the source code and compile
    const char* fragmentSrc = Shaders::fragmentShader;
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    // Create a program and save it's ID
    shaderProgram = glCreateProgram();

    // Attach shaders to the program and link them
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Use program
    glUseProgram(shaderProgram);

    // Delete shaders to free mem
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Tell OpenGL how to interpret the vertices and enable it
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int textVertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* textVertexSrc = Shaders::textVertexShader;
    glShaderSource(textVertexShader, 1, &textVertexSrc, nullptr);
    glCompileShader(textVertexShader);

    unsigned int textFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* textFragmentSrc = Shaders::textFragmentShader;
    glShaderSource(textFragmentShader, 1, &textFragmentSrc, nullptr);
    glCompileShader(textFragmentShader);

    textShaderProgram = glCreateProgram();
    glAttachShader(textShaderProgram, textVertexShader);
    glAttachShader(textShaderProgram, textFragmentShader);
    glLinkProgram(textShaderProgram);

    glDeleteShader(textVertexShader);
    glDeleteShader(textFragmentShader);

    glUseProgram(textShaderProgram);
    textScreenUniform = glGetUniformLocation(textShaderProgram, "uScreenSize");
    textColorUniform = glGetUniformLocation(textShaderProgram, "uTextColor");

    initTextRenderer();
    glUseProgram(shaderProgram);
}

void shutdownWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

} // namespace SolarSim
