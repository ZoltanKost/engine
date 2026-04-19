#ifndef ENGINE
#define ENGINE
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "animation.h"
#include "ui.h"
#include "utils.h"
#include "ship.h"


static float starsSpeedMultiplier = 12.0f;
static float dustSpeedMultiplier = 24.0f;
static float nebulaeSpeedMultiplier = 18.0f;
static float shipSpeed = 400.0f;
static int team_neutral = 0;
static Texture2D nullTexture = {
	.id = -1
};
static FrameAnimation editingAnimation = {0};
static char current_event_animation_flag = 1 << 6;

#define SCREEN_WIDTH 1368
#define SCREEN_HEIGHT 720 


int InitEditAnimationWindow(FrameAnimation editingAnimation, 
							int* editing_frame_count, int parentID, ui_element_datas* uiDatas);

int ReInitEditAnimationWindowWithNewAnimation(FrameAnimation editingAnimation, 
							int* editing_frame_count, int existingWindowParent, ui_element_datas* uiDatas);

void DrawSpriteRotated(Sprite spriteData, 
		Vector2 position, 
		float rotationEuler,int scaleFactorX);

void DrawBackgroundParallax(Camera2D camera, Vector2 speed, int width, int height,
	int unitsInWidth, int unitsInHeight, 
	Texture2D texture1,Texture2D texture2,Texture2D texture3);

void DetectRectangle(Image image);

bool CheckCollisionRectRotated(Rectangle r1, Rectangle r2, 
							float rot1, float rot2,float scaleFactor, Vector2 pixel_offset1, Vector2 pixel_offset2);

void AddFrameToEditingAnimation(int paramCount);

void AddEventToEditingAnimation(int frameNumber);

#endif