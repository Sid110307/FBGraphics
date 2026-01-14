#pragma once

#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <cstdlib>
#endif

class Colors
{
public:
    static constexpr unsigned int RED = 0xFFFF0000;
    static constexpr unsigned int GREEN = 0xFF00FF00;
    static constexpr unsigned int BLUE = 0xFF0000FF;
    static constexpr unsigned int WHITE = 0xFFFFFFFF;
    static constexpr unsigned int BLACK = 0xFF000000;
    static constexpr unsigned int CYAN = 0xFF00FFFF;
    static constexpr unsigned int MAGENTA = 0xFFFF00FF;
    static constexpr unsigned int YELLOW = 0xFFFFFF00;

    static constexpr unsigned int RGBA(const unsigned int r, const unsigned int g, const unsigned int b,
                                       const unsigned int a) { return (a << 24) | (r << 16) | (g << 8) | b; }
};

inline char getChar()
{
#ifdef _WIN32
    if (!_kbhit()) return 0;

    int c = _getch();
    if (c == 0 || c == 224) c = _getch();

    return static_cast<char>(c);
#else
    timeval tv{0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) <= 0) return 0;

    static bool initialized = false;
    static termios oldTerm{};

    if (!initialized)
    {
        tcgetattr(STDIN_FILENO, &oldTerm);

        termios newTerm = oldTerm;
        newTerm.c_lflag &= ~(ICANON | ECHO);
        newTerm.c_cc[VMIN] = 0;
        newTerm.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

        atexit([] { tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm); });
        initialized = true;
    }

    char c = 0;
    if (read(STDIN_FILENO, &c, 1) == 1) return c;

    return 0;
#endif
}
