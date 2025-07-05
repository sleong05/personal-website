#include <vector>
#include <SDL.h>
#include "BodyPart.hpp"

class Fish
{
public:
    Fish(int startingX, int startingY);
    ~Fish();

    void swim(int canvasWidth, int canvasHeight);

    void draw(SDL_Renderer *renderer);
    void drawCircles(SDL_Renderer *renderer);

    void setHeadCircleOffset(const SDL_Event &e);
    void moveHeadCircle(const SDL_Event &e);
    void toggleDragging();

private:
    int changeDirection = 0;
    void updateHead(float targetX, float targetY);
    void updateBody();

    std::vector<BodyPart> bodyParts;
    Head *head;

    int CIRCLE_RADIUS = 20;
};