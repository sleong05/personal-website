#include "fish.hpp"
#include "drawCircle.hpp"
#include <cmath>
#include <cstdlib>
#include <SDL.h>
#include <vector>
#include "SDL2_gfxPrimitives.h"

#define DEG_TO_RAD (M_PI / 180.0f)
#define TURN_ANGLE 15.0f
#define TURN_CHANCE 0.25f
#define EDGE_BUFFER 250
#define STEP_SIZE 8.0f
#define SPINE_DISTANCE 35

Fish::Fish(int startingX, int startingY)
{
    head = new Head(startingX, startingY, 30);

    int partSizes[] = {45, 50, 55, 50, 45, 35, 25, 23, 20, 15};

    float xLocation = 800.0f;
    for (int &size : partSizes)
    {

        bodyParts.emplace_back(xLocation, 400.0f, size);
        xLocation -= 35;
    }
}

Fish::~Fish()
{
    delete head;
}

void Fish::drawCircles(SDL_Renderer *renderer)
{
    draw_circle(renderer, *head); // draw head

    for (auto &bodyPart : bodyParts)
    { // draw body parts
        draw_circle(renderer, bodyPart);
    }
}

void Fish::draw(SDL_Renderer *renderer)
{
    std::vector<SDL_Point> leftOutline;
    std::vector<SDL_Point> rightOutline;
    // for drawing the front and back parts
    auto frontPoints = getOutlineEndPoints(*head, bodyParts[0]);
    auto backPoints = getOutlineEndPoints(bodyParts[bodyParts.size() - 1], bodyParts[bodyParts.size() - 2]);

    // push outline coordinates
    auto headOutline = getOutlinePoints(*head, bodyParts[0]);
    leftOutline.push_back(headOutline.first);
    rightOutline.push_back(headOutline.second);

    for (int i = 1; i < bodyParts.size(); i++)
    {
        auto bodyOutline = getOutlinePoints(bodyParts[i - 1], bodyParts[i]);
        leftOutline.push_back(bodyOutline.first);
        rightOutline.push_back(bodyOutline.second);
    }
    // big fins
    int bigFinPart = 2;
    float midAngle = getAngle(*head, bodyParts[bigFinPart]);

    // dorsal fin, slightly above spine
    auto [dx, dy] = getPerpendicularOffset(midAngle, 50); // push upwards
    float dorsalX = bodyParts[bigFinPart].x - dx;
    float dorsalY = bodyParts[bigFinPart].y - dy;
    drawRotatedEllipse(renderer, dorsalX, dorsalY, 52, 20, midAngle + 40, {129, 196, 211, 255});

    dorsalX = bodyParts[bigFinPart].x + dx;
    dorsalY = bodyParts[bigFinPart].y + dy;
    drawRotatedEllipse(renderer, dorsalX, dorsalY, 52, 20, midAngle - 40, {129, 196, 211, 255});

    // small fins
    int smallFinPart = 8;
    float tailAngle = getAngle(bodyParts[smallFinPart - 1], bodyParts[smallFinPart]);

    // dorsal fin, slightly above spine
    auto [dx1, dy1] = getPerpendicularOffset(tailAngle, 30); // push upwards
    dorsalX = bodyParts[smallFinPart].x - dx1;
    dorsalY = bodyParts[smallFinPart].y - dy1;
    drawRotatedEllipse(renderer, dorsalX, dorsalY, 30, 11, tailAngle + 40, {129, 196, 211, 255});

    dorsalX = bodyParts[smallFinPart].x + dx1;
    dorsalY = bodyParts[smallFinPart].y + dy1;
    drawRotatedEllipse(renderer, dorsalX, dorsalY, 30, 11, tailAngle - 40, {129, 196, 211, 255});

    // fill in body
    fillBody(frontPoints, leftOutline, backPoints, rightOutline, renderer);

    // draw outline
    drawOutline(renderer, leftOutline, frontPoints, rightOutline, backPoints);

    // draw eyes
    drawEyes(renderer);
}

void Fish::drawEyes(SDL_Renderer *&renderer)
{
    auto [eye1, eye2] = getEyeLocations(*head, bodyParts[0]);
    filledCircleRGBA(renderer, static_cast<Sint16>(eye1.first), static_cast<Sint16>(eye1.second), 7, 245, 245, 235, 255);
    filledCircleRGBA(renderer, static_cast<Sint16>(eye2.first), static_cast<Sint16>(eye2.second), 7, 245, 245, 235, 255);
}

void Fish::drawOutline(SDL_Renderer *&renderer, std::vector<SDL_Point> &leftOutline, std::vector<SDL_Point> &frontPoints, std::vector<SDL_Point> &rightOutline, std::vector<SDL_Point> &backPoints)
{
    int thickness = 3;
    // draw head
    thickLineRGBA(renderer, leftOutline[0].x, leftOutline[0].y, frontPoints[2].x, frontPoints[2].y, thickness, 245, 245, 235, 255);   // left body to outer left head
    thickLineRGBA(renderer, frontPoints[2].x, frontPoints[2].y, frontPoints[1].x, frontPoints[1].y, thickness, 245, 245, 235, 255);   // left tail to center head
    thickLineRGBA(renderer, frontPoints[1].x, frontPoints[1].y, frontPoints[0].x, frontPoints[0].y, thickness, 245, 245, 235, 255);   // center head to right head
    thickLineRGBA(renderer, rightOutline[0].x, rightOutline[0].y, frontPoints[0].x, frontPoints[0].y, thickness, 245, 245, 235, 255); // right head to right body
    // draw body
    for (int i = 0; i < leftOutline.size() - 1; ++i)
    {
        thickLineRGBA(renderer, leftOutline[i].x, leftOutline[i].y, leftOutline[i + 1].x, leftOutline[i + 1].y, thickness, 245, 245, 235, 255);
        thickLineRGBA(renderer, rightOutline[i].x, rightOutline[i].y, rightOutline[i + 1].x, rightOutline[i + 1].y, thickness, 245, 245, 235, 255);
    }

    // tail
    auto [leftX, leftY] = leftOutline.back();
    auto [rightX, rightY] = rightOutline.back();
    thickLineRGBA(renderer, rightX, rightY, backPoints[2].x, backPoints[2].y, thickness, 245, 245, 235, 255);                   // left body to outer left tail
    thickLineRGBA(renderer, backPoints[2].x, backPoints[2].y, backPoints[1].x, backPoints[1].y, thickness, 245, 245, 235, 255); // left tail to center tail
    thickLineRGBA(renderer, backPoints[1].x, backPoints[1].y, backPoints[0].x, backPoints[0].y, thickness, 245, 245, 235, 255); // center tail to right tail
    thickLineRGBA(renderer, leftX, leftY, backPoints[0].x, backPoints[0].y, thickness, 245, 245, 235, 255);                     // right tail to right body
}

void Fish::fillBody(std::vector<SDL_Point> &frontPoints, std::vector<SDL_Point> &leftOutline, std::vector<SDL_Point> &backPoints, std::vector<SDL_Point> &rightOutline, SDL_Renderer *&renderer)
{
    std::vector<Sint16> vx;
    std::vector<Sint16> vy;

    // add head points go right to left
    for (auto &point : frontPoints)
    {
        vx.push_back(static_cast<Sint16>(point.x));
        vy.push_back(static_cast<Sint16>(point.y));
    }
    // Add left outline points
    for (const auto &p : leftOutline)
    {
        vx.push_back(p.x);
        vy.push_back(p.y);
    }

    // add tail points still go right to left so reverse the order
    for (auto &point : backPoints)
    {
        vx.push_back(static_cast<Sint16>(point.x));
        vy.push_back(static_cast<Sint16>(point.y));
    }

    // Add right outline points in reverse (tail → head)
    for (int i = rightOutline.size() - 1; i >= 0; --i)
    {
        vx.push_back(rightOutline[i].x);
        vy.push_back(rightOutline[i].y);
    }
    Uint8 r = 54, g = 124, b = 162, a = 255;
    filledPolygonRGBA(renderer, vx.data(), vy.data(), vx.size(), r, g, b, a);
}

// ----------------- helpers -----------------
static float normDeg(float a)
{
    while (a < 0)
        a += 360;
    while (a >= 360)
        a -= 360;
    return a;
}
float shortestDiffDeg(float from, float to)
{
    float diff = fmodf(to - from + 540.0f, 360.0f) - 180.0f;
    return diff;
}

// -------------------------------------------
// Call once per frame
void Fish::swim(int canvasW, int canvasH)
{
    constexpr float ARC_TURN_SPEED = 2.5f; // deg per frame
    constexpr float STEP = 8.0f;           // swim speed
    constexpr int ARC_MIN_DURATION = 80;   // min frames per arc
    constexpr int ARC_MAX_DURATION = 180;
    constexpr float WALL_ARC_BOOST = 1.8f;
    constexpr float EDGE_GRADIENT = 120.0f;
    constexpr float MAX_TOTAL_TURN = .9f;

    // 1. Occasionally flip turning direction (every few seconds)
    if (arcDurationFrames <= 0)
    {
        turnDirection = (rand() % 2 == 0) ? 1 : -1;
        arcDurationFrames = ARC_MIN_DURATION + rand() % (ARC_MAX_DURATION - ARC_MIN_DURATION);
    }
    arcDurationFrames--;

    // 2. Base turn: gentle continuous arc
    float desiredTurn = turnDirection * ARC_TURN_SPEED;

    // 3. Steer away softly from walls
    auto steerAway = [&](float dist, float steerTo)
    {
        if (dist < EDGE_GRADIENT)
        {
            float t = 1.0f - dist / EDGE_GRADIENT;
            float wallInfluence = shortestDiffDeg(head->direction, steerTo);
            desiredTurn += wallInfluence * t * 0.2f * WALL_ARC_BOOST;
        }
    };

    steerAway(head->x, 0.0f);             // left
    steerAway(canvasW - head->x, 180.0f); // right
    steerAway(head->y, 90.0f);            // top
    steerAway(canvasH - head->y, 270.0f); // bottom

    float clampedTurn = std::clamp(desiredTurn, -MAX_TOTAL_TURN, MAX_TOTAL_TURN);
    head->direction = normDeg(head->direction + clampedTurn);

    // 5. Move forward
    float rad = head->direction * (M_PI / 180.f);
    float nextX = head->x + std::cos(rad) * STEP;
    float nextY = head->y + std::sin(rad) * STEP;

    updateHead(nextX, nextY);
}

void Fish::updateBody()
{
    BodyPart &firstPart = bodyParts[0];
    auto [newX, newY] = getNewPosition(head->x, head->y, firstPart.x, firstPart.y, SPINE_DISTANCE);
    firstPart.x = newX;
    firstPart.y = newY;

    for (int i = 1; i < bodyParts.size(); i++)
    {
        BodyPart &infront = bodyParts[i - 1];
        BodyPart &current = bodyParts[i];
        auto [nextX, nextY] = getNewPosition(infront.x, infront.y, current.x, current.y, SPINE_DISTANCE);
        current.x = nextX;
        current.y = nextY;
    }
}

void Fish::setHeadCircleOffset(const SDL_Event &e)
{
    float dx = e.button.x - head->x;
    float dy = e.button.y - head->y;
    if (dx * dx + dy * dy <= CIRCLE_RADIUS * CIRCLE_RADIUS)
    {
        head->dragging = true;
        head->offsetX = dx;
        head->offsetY = dy;
    }
}

void Fish::moveHeadCircle(const SDL_Event &e)
{
    if (head->dragging)
    {
        float targetX = e.motion.x - head->offsetX;
        float targetY = e.motion.y - head->offsetY;
        // smooth
        updateHead(targetX, targetY);
    }
}

void Fish::updateHead(float targetX, float targetY)
{
    head->x += (targetX - head->x) * 0.2f;
    head->y += (targetY - head->y) * 0.2f;
    updateBody();
}

void Fish::toggleDragging()
{
    head->dragging = false;
}