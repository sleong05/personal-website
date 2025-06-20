#pragma once
inline std::pair<float, float> getNewPosition(float x1, float y1, float x2, float y2, float desiredLength) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);

    float ux = dx / length;
    float uy = dy / length;

    float endX = x1 + ux * desiredLength;
    float endY = y1 + uy * desiredLength;

    return {endX, endY};
}

