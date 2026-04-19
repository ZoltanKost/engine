#ifndef ANIMATION_MODULE
#define ANIMATION_MODULE

#include "raylib.h"
#include "sprite.h"

typedef struct Animation
{
	Sprite* sprites; // 8
	int length; // 4
	float frameDuration; // 4
} Animation;

typedef struct Frame
{
	float time; // 4b
	Vector2 position; // 8b
	int sprite_id; // 4 byte
	float rotation; // 4 byte
	char event; // 1 byte
} Frame;

typedef struct FrameAnimation // 36 byte 40 byte
{
	Frame* frames; // 8
	Sprite* sprites; // 8
	int frame_count; // 4
	float duration; // 4
} FrameAnimation;

typedef struct CurrentAnimationData
{
	int current_frame; 
	int current_animation;
	float time;
} CurrentAnimationData;

Animation CreateAnimation(Texture2D texture,
			float frameDuration, Vector2 spritePivot,
			int spriteWidth, int spriteHeight);

FrameAnimation CreateFrameAnimationFromTexture(Texture2D texture,
			float anim_duration, Vector2 spritePivot,
			int spriteWidth, int spriteHeight);

#endif