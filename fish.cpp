#include "fish.hpp"
#include "drawCircle.hpp"

Fish::Fish(int startingX, int startingY) {
    head = new Head(startingX, startingY);

    bodyParts.emplace_back(170.0f, 170.0f);
    bodyParts.emplace_back(170.0f, 170.0f);
    bodyParts.emplace_back(170.0f, 170.0f);
    bodyParts.emplace_back(170.0f, 170.0f);
    bodyParts.emplace_back(170.0f, 170.0f);
    bodyParts.emplace_back(170.0f, 170.0f);

}

Fish::~Fish() {
    delete head;
}

void Fish::draw(SDL_Renderer* renderer) {
    draw_circle(renderer, (int)head->x, (int)head->y, CIRCLE_RADIUS); // draw head

    for (auto & bodyPart: bodyParts) { // draw body parts
        draw_circle(renderer, (int)bodyPart.x, (int)bodyPart.y, CIRCLE_RADIUS); 
    }
};

void Fish::updateBody() {
    BodyPart& firstPart = bodyParts[0];
    auto [newX, newY] = getNewPosition(head->x,head->y, firstPart.x, firstPart.y, 50);
    firstPart.x = newX;
    firstPart.y = newY; 

    for (int i = 1; i < bodyParts.size(); i++) {
        BodyPart& infront = bodyParts[i - 1];
        BodyPart& current = bodyParts[i];
        auto [nextX, nextY] = getNewPosition(infront.x, infront.y, current.x, current.y, 50);
        current.x = nextX;
        current.y = nextY; 
    }
}

std::pair<float, float> Fish::getNewPosition(float x1, float y1, float x2, float y2, float desiredLength) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);

    float ux = dx / length;
    float uy = dy / length;

    float endX = x1 + ux * desiredLength;
    float endY = y1 + uy * desiredLength;

    return {endX, endY};
}

void Fish::setHeadCircleOffset(const SDL_Event& e) {
    float dx = e.button.x - head->x;
    float dy = e.button.y - head->y;
    if (dx * dx + dy * dy <= CIRCLE_RADIUS * CIRCLE_RADIUS) {
        head->dragging = true;
        head->offsetX = dx;
        head->offsetY = dy;
    }
}

void Fish::moveHeadCircle(const SDL_Event& e) {
    if (head->dragging) {
        float targetX = e.motion.x - head->offsetX;
        float targetY = e.motion.y - head->offsetY;

        // smooth
        head->x += (targetX - head->x) * 0.2f;
head->y += (targetY - head->y) * 0.2f;
        updateBody();
    }
}

void Fish::toggleDragging() {
    head->dragging = false;
}