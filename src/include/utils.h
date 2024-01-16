#pragma once

#include <iostream>
#include <cstring>
#include <termios.h>

class Colors
{
public:
    static constexpr unsigned int RED = 0xFF0000FF;
    static constexpr unsigned int GREEN = 0x00FF00FF;
    static constexpr unsigned int BLUE = 0x0000FFFF;
    static constexpr unsigned int WHITE = 0xFFFFFFFF;
    static constexpr unsigned int BLACK = 0x000000FF;
    static constexpr unsigned int CYAN = 0x00FFFFFF;
    static constexpr unsigned int MAGENTA = 0xFF00FFFF;
    static constexpr unsigned int YELLOW = 0xFFFF00FF;
};

char getChar()
{
    char c;
    struct termios oldTerm = {}, newTerm = {};

    if (tcgetattr(STDIN_FILENO, &oldTerm) == -1)
    {
        std::cerr << "Error getting terminal attributes: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    newTerm = oldTerm;
    newTerm.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newTerm) == -1)
    {
        std::cerr << "Error setting terminal attributes: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    c = static_cast<char>(std::cin.get());

    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm) == -1)
    {
        std::cerr << "Error restoring terminal attributes: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    return c;
}
