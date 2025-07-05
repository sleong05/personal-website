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
    auto frontPoint = getOutlineEndPoint(*head, bodyParts[0]);
    auto backPoint = getOutlineEndPoint(bodyParts[bodyParts.size() - 1], bodyParts[bodyParts.size() - 2]);

    auto headOutline = getOutlinePoints(*head, bodyParts[0]);
    leftOutline.push_back(headOutline.first);
    rightOutline.push_back(headOutline.second);

    for (int i = 1; i < bodyParts.size(); i++)
    {
        auto bodyOutline = getOutlinePoints(bodyParts[i - 1], bodyParts[i]);
        leftOutline.push_back(bodyOutline.first);
        rightOutline.push_back(bodyOutline.second);
    }
    // fill in body
    std::vector<Sint16> vx;
    std::vector<Sint16> vy;

    // add head
    vx.push_back(static_cast<Sint16>(frontPoint.first));
    vy.push_back(static_cast<Sint16>(frontPoint.second));
    // Add left outline points
    for (const auto &p : leftOutline)
    {
        vx.push_back(p.x);
        vy.push_back(p.y);
    }

    // add tail
    vx.push_back(static_cast<Sint16>(backPoint.first));
    vy.push_back(static_cast<Sint16>(backPoint.second));

    // Add right outline points in reverse (tail → head)
    for (int i = static_cast<int>(rightOutline.size()) - 1; i >= 0; --i)
    {
        vx.push_back(rightOutline[i].x);
        vy.push_back(rightOutline[i].y);
    }
    Uint8 r = 54, g = 124, b = 162, a = 255;
    filledPolygonRGBA(renderer, vx.data(), vy.data(), vx.size(), r, g, b, a);

    // draw from front point
    thickLineRGBA(renderer, leftOutline[0].x, leftOutline[0].y, frontPoint.first, frontPoint.second, 3, 245, 245, 235, 255);
    thickLineRGBA(renderer, rightOutline[0].x, rightOutline[0].y, frontPoint.first, frontPoint.second, 3, 245, 245, 235, 255);
    // draw body
    for (int i = 0; i < leftOutline.size() - 1; ++i)
    {
        thickLineRGBA(renderer, leftOutline[i].x, leftOutline[i].y, leftOutline[i + 1].x, leftOutline[i + 1].y, 3, 245, 245, 235, 255);
        thickLineRGBA(renderer, rightOutline[i].x, rightOutline[i].y, rightOutline[i + 1].x, rightOutline[i + 1].y, 3, 245, 245, 235, 255);
    }

    // draw from end body to the end
    auto [leftX, leftY] = leftOutline.back();
    auto [rightX, rightY] = rightOutline.back();
    thickLineRGBA(renderer, leftX, leftY, backPoint.first, backPoint.second, 3, 245, 245, 235, 255);
    thickLineRGBA(renderer, rightX, rightY, backPoint.first, backPoint.second, 3, 245, 245, 235, 255);

    // draw eyes
    // 🖌 Step 4: Draw eyes
    auto [eye1, eye2] = getEyeLocations(*head, bodyParts[0]);
    filledCircleRGBA(renderer, static_cast<Sint16>(eye1.first), static_cast<Sint16>(eye1.second), 6, 245, 245, 235, 255);
    filledCircleRGBA(renderer, static_cast<Sint16>(eye2.first), static_cast<Sint16>(eye2.second), 6, 245, 245, 235, 255);
}

void Fish::swim(int canvasWidth, int canvasHeight)
{
    // Occasionally change direction randomly
    if ((rand() % 100) < (TURN_CHANCE * 100))
    {
        float angleChange = ((rand() % 200) / 100.0f - 1.0f) * TURN_ANGLE;
        head->direction += angleChange;
    }

    const float TURN_SPEED = 0.05f; // smaller = slower turn

    // Calculate desired angle away from edge
    float desiredAdjustment = 0;

    if (head->x < EDGE_BUFFER)
    {
        desiredAdjustment += TURN_SPEED * (0 - head->direction); // right
    }
    else if (head->x > canvasWidth - EDGE_BUFFER)
    {
        desiredAdjustment += TURN_SPEED * (180 - head->direction); // left
    }

    if (head->y < EDGE_BUFFER)
    {
        desiredAdjustment += TURN_SPEED * (90 - head->direction); // down
    }
    else if (head->y > canvasHeight - EDGE_BUFFER)
    {
        desiredAdjustment += TURN_SPEED * (270 - head->direction); // up
    }
    head->direction += desiredAdjustment;
    float radians = head->direction * DEG_TO_RAD;

    float targetX = head->x + std::cos(radians) * STEP_SIZE;
    float targetY = head->y + std::sin(radians) * STEP_SIZE;

    updateHead(targetX, targetY);
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