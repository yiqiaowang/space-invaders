#include "spdlog/spdlog.h"
#include "dbg.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "buffer.h"
#include "shaders.h"

void error_callback(int error, const char* description)
{
    spdlog::error("Error: {}", description);
}


void validate_shader(GLuint shader, const char* file = 0)
{
    static const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

    if(length > 0)
    {
        spdlog::error("Shader {}({}) compile error: {}", shader, (file ? file: ""), buffer);
    }
}

bool validate_program(GLuint program)
{
    static const GLsizei BUFFER_SIZE = 512;
    GLchar buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);

    if(length > 0)
    {
        spdlog::error("Program {} link error: {}", program, buffer);
        return false;
    }

    return true;
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
    std::size_t buffer_width = 640; 
    std::size_t buffer_height = 480; 
    auto window = glfwCreateWindow(buffer_width, buffer_height, "Space Invaders", NULL, NULL);
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

    // Initialize Buffer
    uint32_t clear_color = rgb_to_uint32(0, 128, 0);
    Buffer buffer;
    buffer.width  = buffer_width;
    buffer.height = buffer_height;
    buffer.data   = new uint32_t[buffer.width * buffer.height];
    buffer_clear(&buffer, clear_color);

    // Start Part 2 - CPU Shaders
    GLuint fullscreen_triangle_vao;
    glGenVertexArrays(1, &fullscreen_triangle_vao);
    glBindVertexArray(fullscreen_triangle_vao);

    GLuint shader_id = glCreateProgram();

    //Create vertex shader
    {
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader_vp, 1, &vertex_shader, 0);
        glCompileShader(shader_vp);
        validate_shader(shader_vp, vertex_shader);
        glAttachShader(shader_id, shader_vp);

        glDeleteShader(shader_vp);
    }

    //Create fragment shader
    {
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader_fp, 1, &fragment_shader, 0);
        glCompileShader(shader_fp);
        validate_shader(shader_fp, fragment_shader);
        glAttachShader(shader_id, shader_fp);

        glDeleteShader(shader_fp);
    }

    glLinkProgram(shader_id);

    if(!validate_program(shader_id))
    {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();
        glDeleteVertexArrays(1, &fullscreen_triangle_vao);
        delete[] buffer.data;
        return -1;
    }
    glUseProgram(shader_id);


    /* Create Textures */
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB8,
            buffer.width, buffer.height, 0,
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data
            );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Attach texture to fragment shader */
    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);


    /* Bind vertex array */
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(fullscreen_triangle_vao);

    // Start Game Loop
    glClearColor(1.0, 0.0, 0.0, 1.0);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
