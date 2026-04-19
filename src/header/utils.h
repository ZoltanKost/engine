#ifndef UTILS
#define UTILS
#include "raylib.h"
#define vector2_zero (Vector2){0,0}
#define vector2_up (Vector2){1,0}
Rectangle DetectCollisionRectangle(Image image, int xMax, int yMax);

#endif