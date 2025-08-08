#pragma once
struct BodyPart
{
    float x, y, radius;

    BodyPart(float x_, float y_, float radius_) : x(x_), y(y_), radius(radius_) {}
};

struct Head : public BodyPart
{
    bool dragging;
    float offsetX, offsetY;
    float direction;

    Head(float x, float y, float radius_)
        : BodyPart(x, y, radius_), dragging(false), offsetX(0.0f), offsetY(0.0f), direction(0) {}
};
