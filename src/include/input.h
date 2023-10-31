#pragma once

#include <iostream>
#include <termios.h>

char getChar()
{
    char c;
    struct termios oldTerm = {}, newTerm = {};

    tcgetattr(0, &oldTerm);
    newTerm = oldTerm;
    newTerm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newTerm);

    c = static_cast<char>(std::cin.get());
    tcsetattr(0, TCSANOW, &oldTerm);

    return c;
}
