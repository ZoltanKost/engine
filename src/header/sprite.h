#ifndef SPRITE
#define SPRITE

#include <stdio.h>
#include "raylib.h"

typedef struct Sprite // 42 byte;
{
	Texture2D texture; // 20 TODO: can be moved out to some texture_sprites struct
	Rectangle rect; // 16
	Vector2 pivot; // 8
} Sprite;

Sprite* CreateSprites(Texture2D texture,
	Vector2 pivot, int width, int height, int* spriteCount);

Sprite CreateSprite(Texture2D texture,
	Vector2 pivot);

#endif