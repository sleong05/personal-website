// draw.hpp
#pragma once
#include <SDL.h>
#include "BodyPart.hpp"
#include <numbers>
#include <utility>
using OutlinePair = std::pair<SDL_Point, SDL_Point>;
using eyeLocationsPair = std::pair<std::pair<Sint16, Sint16>, std::pair<Sint16, Sint16>>;
constexpr double pi = 3.14159265358979323846;

// Draws a circle centered at (cx, cy) with radius r
void draw_circle(SDL_Renderer *renderer, BodyPart &BodyPart);
void draw_circle(SDL_Renderer *renderer, float x, float y, float radius);

void drawDots(SDL_Renderer *renderer, float cx, float cy, float r);

float getAngle(BodyPart &a, BodyPart &b);

OutlinePair getOutlinePoints(BodyPart &a, BodyPart &b);
std::pair<int, int> getOutlineEndPoint(BodyPart &endPart, BodyPart &closestPart);

std::pair<int, int> getNewPosition(float x1, float y1, float x2, float y2, float desiredLength);
eyeLocationsPair getEyeLocations(BodyPart &head, BodyPart &firstPart);