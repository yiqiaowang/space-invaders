#ifndef SPRITE_H
#define SPRITE_H

#include <cstdint>

#include "buffer.h"

struct Sprite
{
    std::size_t width, height;
    uint8_t* data;
};

struct SpriteAnimation
{
    bool loop;
    std::size_t num_frames;
    std::size_t frame_duration;
    std::size_t time;
    Sprite** frames;
};

void buffer_sprite_draw(
    Buffer* buffer, const Sprite& sprite,
    std::size_t x, std::size_t y, uint32_t color
){
    for(std::size_t xi = 0; xi < sprite.width; ++xi)
    {
        for(std::size_t yi = 0; yi < sprite.height; ++yi)
        {
            std::size_t sy = sprite.height - 1 + y - yi;
            std::size_t sx = x + xi;
            if(sprite.data[yi * sprite.width + xi] &&
               sy < buffer->height && sx < buffer->width) 
            {
                buffer->data[sy * buffer->width + sx] = color;
            }
        }
    }
}

#endif /* SPRITE_H */
