#include "spdlog/spdlog.h"
#include "dbg.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

void error_callback(int error, const char* description)
{
    spdlog::error("Error: {}", description);
}

int main() 
{
    glfwSetErrorCallback(error_callback);
    
    // Initialize GLFW
    if(!glfwInit()) return -1;

    // Specify window parameters
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window
    auto window = glfwCreateWindow(640, 480, "Space Invaders", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        spdlog::error("Error initializing GLEW");
        glfwTerminate();
        return -1;
    }

    // Info about OpenGL version
    int glVersion[2] = {-1, 1};
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
    spdlog::info("Using OpenGL: {}.{}", glVersion[0], glVersion[1]);


    // Start Game Loop
    glClearColor(1.0, 0.0, 0.0, 1.0);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
