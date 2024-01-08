#pragma once

#include <iostream>
#include <termios.h>

char getChar()
{
    char c;
    struct termios oldTerm = {}, newTerm = {};

    if (tcgetattr(STDIN_FILENO, &oldTerm) == -1)
    {
        std::cerr << "Error getting terminal attributes!" << std::endl;
        exit(EXIT_FAILURE);
    }

    newTerm = oldTerm;
    newTerm.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newTerm) == -1)
    {
        std::cerr << "Error setting terminal attributes!" << std::endl;
        exit(EXIT_FAILURE);
    }

    c = static_cast<char>(std::cin.get());

    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm) == -1)
    {
        std::cerr << "Error restoring terminal attributes!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return c;
}
