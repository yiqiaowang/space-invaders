/* External libraries */
#include "spdlog/spdlog.h"
#include "dbg.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

/* Internal libraries */
#include "buffer.h"
#include "shaders.h"
#include "sprite.h"
#include "game.h"

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




    /* Define a sprite */
    Sprite alien_sprite;
    alien_sprite.width = 11;
    alien_sprite.height = 8;
    alien_sprite.data = new uint8_t[11 * 8]
    {
        0,0,1,0,0,0,0,0,1,0,0, // ..@.....@..
        0,0,0,1,0,0,0,1,0,0,0, // ...@...@...
        0,0,1,1,1,1,1,1,1,0,0, // ..@@@@@@@..
        0,1,1,0,1,1,1,0,1,1,0, // .@@.@@@.@@.
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
        1,0,1,1,1,1,1,1,1,0,1, // @.@@@@@@@.@
        1,0,1,0,0,0,0,0,1,0,1, // @.@.....@.@
        0,0,0,1,1,0,1,1,0,0,0  // ...@@.@@...
    };

    Sprite player_sprite;
    player_sprite.width = 11;
    player_sprite.height = 7;
    player_sprite.data = new uint8_t[77]
    {
        0,0,0,0,0,1,0,0,0,0,0, // .....@.....
        0,0,0,0,1,1,1,0,0,0,0, // ....@@@....
        0,0,0,0,1,1,1,0,0,0,0, // ....@@@....
        0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
    };

    Sprite alien_sprite1;
    alien_sprite1.width = 11;
    alien_sprite1.height = 8;
    alien_sprite1.data = new uint8_t[88]
    {
        0,0,1,0,0,0,0,0,1,0,0, // ..@.....@..
            1,0,0,1,0,0,0,1,0,0,1, // @..@...@..@
            1,0,1,1,1,1,1,1,1,0,1, // @.@@@@@@@.@
            1,1,1,0,1,1,1,0,1,1,1, // @@@.@@@.@@@
            1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
            0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
            0,0,1,0,0,0,0,0,1,0,0, // ..@.....@..
            0,1,0,0,0,0,0,0,0,1,0  // .@.......@.
    };


    Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.num_aliens = 55;
    game.aliens = new Alien[game.num_aliens];

    game.player.x = 112 - 5;
    game.player.y = 32;

    game.player.life = 3;

    for(size_t yi = 0; yi < 5; ++yi)
    {
        for(size_t xi = 0; xi < 11; ++xi)
        {
            game.aliens[yi * 11 + xi].x = 16 * xi + 20;
            game.aliens[yi * 11 + xi].y = 17 * yi + 128;
        }
    }

    // Setup initial game state
    for(size_t ai = 0; ai < game.num_aliens; ++ai)
    {
        const Alien& alien = game.aliens[ai];
        buffer_sprite_draw(&buffer, alien_sprite,
            alien.x, alien.y, rgb_to_uint32(128, 0, 0));
    }

    buffer_sprite_draw(&buffer, player_sprite, game.player.x, game.player.y, rgb_to_uint32(128, 0, 0));


    // Create an animated alien
    SpriteAnimation* alien_animation = new SpriteAnimation;

    alien_animation->loop = true;
    alien_animation->num_frames = 2;
    alien_animation->frame_duration = 10;
    alien_animation->time = 0;

    alien_animation->frames = new Sprite*[2];
    alien_animation->frames[0] = &alien_sprite;
    alien_animation->frames[1] = &alien_sprite1;

    // Create animated player
    int player_move_dir = 1; 

    /* Attach texture to fragment shader */
    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    /* Enable vsync */
    glfwSwapInterval(1);
    
    /* Bind vertex array */
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(fullscreen_triangle_vao);

    // Start Game Loop
    glClearColor(1.0, 0.0, 0.0, 1.0);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glTexSubImage2D(
                GL_TEXTURE_2D, 0, 0, 0,
                buffer.width, buffer.height,
                GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                buffer.data
                );
        glfwSwapBuffers(window);
        glfwPollEvents();


        // Animate Alien
        ++alien_animation->time;
        if(alien_animation->time == alien_animation->num_frames * alien_animation->frame_duration)
        {
            if(alien_animation->loop) alien_animation->time = 0;
            else
            {
                delete alien_animation;
                alien_animation = nullptr;
            }
        }
        for(size_t ai = 0; ai < game.num_aliens; ++ai)
        {
            const Alien& alien = game.aliens[ai];
            size_t current_frame = alien_animation->time / alien_animation->frame_duration;
            const Sprite& sprite = *alien_animation->frames[current_frame];
            buffer_sprite_draw(&buffer, sprite, alien.x, alien.y, rgb_to_uint32(128, 0, 0));
        }

        // Animate Player
        if(game.player.x + player_sprite.width + player_move_dir >= game.width - 1)
        {
            game.player.x = game.width - player_sprite.width - player_move_dir - 1;
            player_move_dir *= -1;
        }
        else if((int)game.player.x + player_move_dir <= 0)
        {
            game.player.x = 0;
            player_move_dir *= -1;
        }
        else game.player.x += player_move_dir;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
