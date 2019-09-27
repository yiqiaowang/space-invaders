#ifndef GAME_H
#define GAME_H

#include <cstdint>

struct Alien
{
    std::size_t x, y;
    uint8_t type;
};

struct Player
{
    std::size_t x, y;
    std::size_t life;
};

struct Game
{
    std::size_t width, height;
    std::size_t num_aliens;
    Alien* aliens;
    Player player;
};

#endif /* GAME_H */
