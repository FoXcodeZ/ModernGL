#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>



static int windowWidth = 1024;
static int windowHeight = 768;

#define UNUSED(x) (void)(x)

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

static void setWireframe(const bool wireframe)
{
    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

static bool wireframe = false;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    UNUSED(scancode);
    UNUSED(action);
    UNUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        wireframe = !wireframe;
        setWireframe(wireframe);
    }
}


int main()
{
    constexpr float vertices[] = {
        // positions                colors
        0.5f,   0.5f,   0.0f,       1.0f,   0.0f,   0.0f,   // top right
        -0.5f,  0.5f,   0.0f,       0.0f,   1.0f,   0.0f,   // top left
        -0.5f,  -0.5f,   0.0f,      0.0f,   0.0f,   1.0f,   // bottom left
        0.5f,   -0.5f,  0.0f,       1.0f,   0.0f,   1.0f,   // bottom right
    };

    constexpr unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "ModernGL [ESC: exit, F1: wireframe]", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }


    glfwMakeContextCurrent(window);

    if (gladLoadGL(glfwGetProcAddress) == 0)
    {
        std::cerr << "Failed to initialize OpenGL loader" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);


    const char* vertexShaderSource =
        "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 vertexColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
        "   vertexColor = aColor;\n"
        "}\0";

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
    }
    // 0.85, 0.19, 0.68

    const char* fragmentShaderSource =
        "#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec3 vertexColor;\n"
        "void main()\n"
        "{\n"
        "FragColor = vec4(vertexColor, 1.0);\n"
        "}\0";

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO - Vertex Array Object
    // VBO - Vertex Buffer Object
    // EBO - Element Buffer Object
    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    framebufferSizeCallback(window, windowWidth, windowHeight);

    glfwSetKeyCallback(window, keyCallback);

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
