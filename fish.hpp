#include <vector>
#include <SDL.h>
struct BodyPart {
    float x,y;

    BodyPart(float x_, float y_) : x(x_), y(y_) {}
};

struct Head {
    float x, y;
    bool dragging;
    float offsetX, offsetY;

    Head(float x, float y) : x(x), y(y), dragging(false), offsetX(0.0f), offsetY(0.0f) {}
};

class Fish
{
    public:
        Fish(int startingX, int startingY);
        ~Fish();

        void draw(SDL_Renderer* renderer);
        void setHeadCircleOffset(const SDL_Event& e);
        void moveHeadCircle(const SDL_Event& e);
        void toggleDragging();
    private:
        void updateBody();
        std::pair<float, float> getNewPosition(float x1, float y1, float x2, float y2, float desiredLength);

        std::vector<BodyPart> bodyParts;
        Head* head;

        int CIRCLE_RADIUS = 20;
        
};