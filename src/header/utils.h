#ifndef UTILS
#define UTILS
#include "raylib.h"
#include "raymath.h"
#include <math.h> 
#define vector2_zero (Vector2){0,0}
#define vector2_up (Vector2){0,1}
#define vector3_up (Vector3){0,1,0}
Rectangle DetectCollisionRectangle(Image image, int xMax, int yMax);
Vector2 Vector2NormalizedSlerp(Vector2 direction, Vector2 target, float a);
bool CheckCollisionRectRotated(Rectangle r1, Rectangle r2,
							float rot1, float rot2, float scaleFactor, Vector2 pixel_offset1, Vector2 pixel_offset2);
float NormalizedDifferenceLength(Vector2 v1, Vector2 v2);

float ArcLength(Vector2 v1, Vector2 v2);

#endif