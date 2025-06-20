// draw.cpp
#include "drawCircle.hpp"
#include <cmath>

void draw_circle(SDL_Renderer* renderer, int cx, int cy, int r) {
    const int points = 200;
    for (int i = 0; i < points; ++i) {
        float angle = 2 * M_PI * i / points;
        int x = static_cast<int>(cx + r * cosf(angle));
        int y = static_cast<int>(cy + r * sinf(angle));
        SDL_RenderDrawPoint(renderer, x, y);
    }
}
