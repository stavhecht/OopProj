#include "Console.h"

bool g_colors_enabled = true;

std::string build_hearts(int lifes) {
    if (lifes <= 0) return std::string();
    const char* heart = u8"\u2665";
    std::string out;
    // Append the UTF-8 heart literal repeatedly.
    for (int i = 0; i < lifes; i++) out += heart;
    return out;
}