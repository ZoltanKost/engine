#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "animation.c"
#include "ui.c"


static Vector2 up = {0,-1};
static Vector2 vector2_zero = {0,0};



int InitEditAnimationWindow(FrameAnimation editingAnimation, 
							int* editing_frame_count, int parentID, ui_element_datas* uiDatas);

int ReInitEditAnimationWindowWithNewAnimation(FrameAnimation editingAnimation, 
							int* editing_frame_count, int existingWindowParent, ui_element_datas* uiDatas);

Sprite* CreateSprites(Texture2D texture,
	Vector2 pivot, int width, int height, int* spriteCount);

Sprite CreateSprite(Texture2D texture,
	Vector2 pivot);

void DrawSpriteRotated(Sprite spriteData, 
		Vector2 position, 
		float rotationEuler,int scaleFactorX);

void DrawBackgroundParallax(Camera2D camera, Vector2 speed, int width, int height,
	int unitsInWidth, int unitsInHeight, 
	Texture2D texture1,Texture2D texture2,Texture2D texture3);
/*
Animation* CreateAnimations(Texture2D* textures,
			float* frameDurations, Vector2 spritePivot,
			int *spriteWidth, int *spriteHeight, int length);*/

Animation CreateAnimation(Texture2D texture,
			float frameDuration, Vector2 spritePivot,
			int spriteWidth, int spriteHeight);

FrameAnimation CreateFrameAnimationFromTexture(Texture2D texture,
			float anim_duration, Vector2 spritePivot,
			int spriteWidth, int spriteHeight);

void DetectRectangle(Image image);

Rectangle DetectCollisionRectangle(Image image,int xMax, int yMax);


bool CheckCollisionRectRotated(Rectangle r1, Rectangle r2, 
							float rot1, float rot2,float scaleFactor, Vector2 pixel_offset1, Vector2 pixel_offset2);

void AddFrameToEditingAnimation(int paramCount);

void AddEventToEditingAnimation(int frameNumber);

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

#include "ship.c"
//#include "bullet.c"

#define SCREEN_WIDTH 1368
#define SCREEN_HEIGHT 720 


int main(){

	SetConfigFlags(FLAG_WINDOW_RESIZABLE & FLAG_WINDOW_ALWAYS_RUN & FLAG_FULLSCREEN_MODE);
	/*int width = 640; 
	int height = 360;*/

	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"My first raylib window! TY Raysan5!");
	//ToggleFullscreen();
	int unitsInWidth = SCREEN_WIDTH / 16;
	int unitsInHeight = SCREEN_HEIGHT / 9;
	float scaleFactorX = 2.0f;//(float)SCREEN_WIDTH / width;
	float scaleFactorY = 2.0f;//(float)SCREEN_HEIGHT / height;
	SetTargetFPS(60);
	Color bgColor = {18,18,18,255};

	Color green = {18,255,18,255};
	/*Color red = {255,0,0,255};*/
	
	float spawnTime = 0.75f;
	float spawn = 0.0f;
	int Count = 0;
	int MaxCount = 32;
	
	Texture2D space_stars = LoadTexture("resources/Stars.png");
	Texture2D space_dust = LoadTexture("resources/Dust.png");
	Texture2D space_nebulae = LoadTexture("resources/Nebulae.png");
	
	Texture2D fighting_ship_Texture = LoadTexture("resources/FighterShip/Fighter_Base.png");
	Texture2D fighting_ship_ShootTexture = LoadTexture("resources/FighterShip/Fighter_Weapon.png");
	Texture2D fighting_ship_DestructionTexture = LoadTexture("resources/FighterShip/Fighter_Destruction.png");
	Texture2D fighting_ship_engineTexture = LoadTexture("resources/FighterShip/Fighter_Engine.png");
	

	Texture2D asteroid_texture = LoadTexture("resources/Asteroid.png");


	Vector2 offset = {0.5f, 0.5f};

	//DetectRectangle(image);
	
	Ship ship = CreateShipLoadAnimations("resources/BasicShip/BasicShip.png",
								"resources/BasicShip/BasicShip_Weapons.png",
								"resources/BasicShip/BasicShip_Destruction.png",
								"resources/BasicShip/BasicShip_Engine.png",
								"resources/BasicShip/Bullet.png",
								NULL,
								"ship_animation1.anim",
								NULL,
								4, 850,1.5f,
				 				shipSpeed, 1);
 
	/*Ship fighter = CreateShip(fighting_ship_Texture, fighting_ship_ShootTexture,
				 			fighting_ship_DestructionTexture, fighting_ship_engineTexture,bulletToSpawn,
				 			shipSpeed, 1, vector2_zero);
	

	Ship asteroid = CreateShip(asteroid_texture, nullTexture,
				 			nullTexture, nullTexture,bulletToSpawn,
				 			0, 2, vector2_zero);*/

	Camera2D camera = {{SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f},{0,0},0,1.0f};

	ShipDatas datas = InitShips(8);
	BulletDatas bulletDatas = InitBullets(8);
	ui_element_datas uiDatas = init_ui(8);


	// Create buttonf for resizing keyframes
	Vector2 size = {32,32};
	/*Rectangle buttonRect = {16,SCREEN_HEIGHT - SCREEN_HEIGHT / 4 + 16,32,32};
	ui_element buttonEl = create_ui_element(buttonRect,(Sprite){0},LIGHTGRAY,3,ui_flag_rawRect | ui_flag_button,0,0,AddFrameToEditingAnimation);
	int button = add_ui_element(&uiDatas,buttonEl,-1);*/

	int playerShip = AddShip(&datas, ship);
	Vector2 shipOffset = (Vector2){200,200};
	Vector2 shipPos = {0};
	ship.position = Vector2Add(ship.position, shipOffset);
	ship.team = 2;
	int enemyShip = AddShip(&datas, ship);


	for(int i = 0; i < 5; i++)
	{
		shipPos = Vector2Add(shipPos,shipOffset);
		ship.position = shipPos;
		enemyShip = AddShip(&datas, ship);
		printf("\n %d",enemyShip);
	}


	/*shipPos = vector2_zero;
	for(int i = 0; i < 5; i++)
	{
		shipPos = Vector2Subtract(shipPos,shipOffset);
		asteroid.position = shipPos;
		AddShip(&datas, asteroid);
	}*/


	float bulletTimer = 0;
	printf("\n %d %d %d \n", playerShip, enemyShip,  datas.count);
	Vector2 parallaxOffset = {0};

	//ReadFrameAnimation("ship_animation1.anim", &ship.animations[1]);
	char* editingAnimationPath = "ship_animation1.anim";
	editingAnimation = ship.animations[1];
	int editing_frame_count = -1;

	InitEditAnimationWindow(editingAnimation, 
							&editing_frame_count, 0, &uiDatas);
	

	while(!WindowShouldClose())
	{
		BeginMode2D(camera);
		ClearBackground(bgColor);
		
		float dt = GetFrameTime();
		//printf("\n %.5f", dt);
		Vector2 mousePos = GetMousePosition();
		ui_cast_result uiInput = {-1,-1};
		if(IsKeyDown(KEY_ONE))
		{
			do
			{
				if(current_event_animation_flag & ship_flag_remove)
				{
					current_event_animation_flag = 4;
					break;
				}
				current_event_animation_flag <<= 1;
			}while(!(current_event_animation_flag & ship_flag_reset 
				|| current_event_animation_flag & ship_flag_shoot
				|| current_event_animation_flag & ship_flag_remove));
			printf("flag: %d\n", current_event_animation_flag);
		}
		if(IsKeyDown(KEY_TWO))
		{
			//WriteFrameAnimationToFile(editingAnimation, editingAnimationPath);
		}
		if(IsKeyDown(KEY_THREE))
		{
			editingAnimation = ship.animations[2];
			ReInitEditAnimationWindowWithNewAnimation(editingAnimation, 
							&editing_frame_count, 0, &uiDatas);
		}
		if(IsKeyDown(KEY_FOUR))
		{
			datas.data[playerShip].state = ship_state_destroy;
		}
		bool mousePressed = IsMouseButtonPressed(0);
		if(mousePressed)
		{
			uiInput = cast_over_ui(&uiDatas, mousePos);
			if(uiInput.id > -1)
			{
				int number_in_children = uiDatas.data[uiInput.id].number_in_children;
				if(uiInput.flags & ui_flag_button && uiDatas.data[uiInput.id].callback != 0)
				{
					uiDatas.data[uiInput.id].callback(number_in_children - 1);
				}
				printf("edit: %d ship[0]: %d ship[1] %d\n", (int)editingAnimation.frames, (int)datas.data[0].animations[2].frames, (int)datas.data[1].animations[2].frames);
				printf("edit: %d ship[0]: %d ship[1] %d\n", (int)editingAnimation.frames[number_in_children - 1].event, (int)datas.data[0].animations[2].frames[number_in_children - 1].event, (int)datas.data[1].animations[2].frames[number_in_children - 1].event);
				//ship.animations[1] = editingAnimation;
				//datas.data[0].animations[1] = editingAnimation;
				//WriteFrameAnimationToFile(editingAnimation, "shoot_animation.anim");
			}
		}
		//Vector2 gamepad_input = {0};
		bool moving = false;
		/*if(IsGamepadAvailable(0))
		{
			gamepad_input.x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
			gamepad_input.y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
		}*/
		mousePos.x += camera.target.x;
		mousePos.y += camera.target.y;
		Vector2 mouseRelativePosition = 
		{
			mousePos.x - datas.data[playerShip].position.x - camera.offset.x,
			mousePos.y - datas.data[playerShip].position.y - camera.offset.y
		};
		spawn += dt;

		Vector2 targetVector = {0};
		if(datas.data[playerShip].flags & ship_flag_move){
			targetVector = datas.data[playerShip].moveDirection;
		}else
		{
			targetVector = vector2_zero;
		}
		parallaxOffset = Vector2Lerp(parallaxOffset, targetVector, dt);
		DrawBackgroundParallax(camera, parallaxOffset,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			unitsInWidth, unitsInHeight,
			space_dust, space_nebulae, space_stars);
		
		/*if(IsGamepadAvailable(0))
		{
			printf("%.10f %.10f \n", gamepad_input.x, gamepad_input.y);
			datas.data[playerShip].moveDirection = Vector2Normalize(gamepad_input);
			moving = gamepad_input.x > 0.1f || gamepad_input.y > 0.1f;
		}*/
		if(IsMouseButtonDown(0))
		{
			datas.data[playerShip].moveDirection = Vector2Normalize(mouseRelativePosition);
			moving = true;
		}else
		{
			bool a = IsKeyDown(KEY_A); 
			bool s = IsKeyDown(KEY_S);
			bool d = IsKeyDown(KEY_D); 
			bool w = IsKeyDown(KEY_W);
			if(a || s || w || d)
			{
				datas.data[playerShip].moveDirection = vector2_zero;
				if(a)
				{
					datas.data[playerShip].moveDirection = 
					Vector2Add(datas.data[playerShip].moveDirection,
					 (Vector2){-1,0});
					moving = 1;
				}
				if(s)
				{
					datas.data[playerShip].moveDirection = 
					Vector2Add(datas.data[playerShip].moveDirection,
					 (Vector2){0,1});
					moving = 1;
				}
				if(d)
				{
					datas.data[playerShip].moveDirection = 
					Vector2Add(datas.data[playerShip].moveDirection,
					 (Vector2){1,0});
					moving = 1;
				}
				if(w)
				{
					datas.data[playerShip].moveDirection = 
					Vector2Add(datas.data[playerShip].moveDirection,
					 (Vector2){0,-1});
					moving = 1;
				}
			}
			
		}
		if(moving) datas.data[playerShip].flags = datas.data[playerShip].flags | ship_flag_move;
		else datas.data[playerShip].flags = datas.data[playerShip].flags & ((~ship_flag_move) + 1);
		//bulletTimer += dt;
		if((IsMouseButtonDown(1) || IsKeyDown(KEY_SPACE)))
		{
			datas.data[playerShip].state |= ship_state_shooting;
		}

		ProcessShips(&datas, &bulletDatas,dt, scaleFactorX);
		ProcessBullets(&bulletDatas,&datas, dt, scaleFactorX);
		process_ui(&uiDatas,dt,camera);

		camera.target = datas.data[playerShip].position;


		EndDrawing();
	}
	CloseWindow();
	return 0;
}

void DrawSpriteRotated(Sprite sprite, 
		Vector2 position, 
		float rotationEuler, int scaleFactorX)
{
		Rectangle spriteRectangle = sprite.rect;
		Rectangle destination = {position.x, position.y, 
								spriteRectangle.width * scaleFactorX, spriteRectangle.height * scaleFactorX};
		Vector2 pivot = {sprite.pivot.x * scaleFactorX,sprite.pivot.y * scaleFactorX};
		DrawTexturePro(sprite.texture, sprite.rect, destination, 
			pivot, rotationEuler, WHITE);
		//printf("%f %f", sprite.pivot.x, sprite.pivot.y);
}

Sprite CreateSprite(Texture2D texture,
			Vector2 pivot)
{
	Sprite res = {0};
	res.texture = texture;
	Rectangle rect = {0,0,texture.width,texture.height};
	res.rect = rect;
	pivot.x *= texture.width;
	pivot.y *= texture.height;
	res.pivot = pivot;
	return res;
}

void DrawBackgroundParallax(Camera2D camera, Vector2 speed, int width, int height,
	int unitsInWidth, int unitsInHeight,
	Texture2D texture1, Texture2D texture2, Texture2D texture3)
{
	// Draw Parallax
	Rectangle dustRect = 
	{
		camera.target.x - camera.offset.x - unitsInWidth/2,
		camera.target.y - camera.offset.y  - unitsInHeight/2,
		1.1f * width,
		1.1f * height,
	};

	Rectangle nebulaeRect = dustRect;

	nebulaeRect.x -= speed.x * nebulaeSpeedMultiplier;
	nebulaeRect.y -= speed.y * nebulaeSpeedMultiplier;
	
	Rectangle starsRect = dustRect;

	starsRect.x -= speed.x * starsSpeedMultiplier;
	starsRect.y -= speed.y * starsSpeedMultiplier;

	dustRect.x -= speed.x * dustSpeedMultiplier;
	dustRect.y -= speed.y * dustSpeedMultiplier;

	Rectangle backgroundRect = {0,0,texture1.width,texture1.height};
	
	DrawTexturePro(texture3, backgroundRect, starsRect, vector2_zero, 0.0f, WHITE);
	DrawTexturePro(texture2, backgroundRect, nebulaeRect, vector2_zero, 0.0f, WHITE);
	DrawTexturePro(texture1, backgroundRect, dustRect, vector2_zero, 0.0f, WHITE);
}

void AddFrameToEditingAnimation(int paramsCount)
{
	// TODO: allocations
	return;
	printf("Adding frame...\n");
	editingAnimation.frames = MemRealloc(editingAnimation.frames, editingAnimation.frame_count++);
}

void AddEventToEditingAnimation(int frameNumber)
{
	editingAnimation.frames[frameNumber].event ^= current_event_animation_flag;
	printf("for frame %d added event %d \n",frameNumber,ship_flag_reset);
}

Sprite* CreateSprites(Texture2D texture,
	Vector2 pivot, int width, int height, int* sCount)
{
	int spriteCount = (texture.width / width) * (texture.height / height);
	*sCount = spriteCount;
	int spriteSize = sizeof(Sprite);
	pivot.x *= width;
	pivot.y *= height;
	int x = 0; int y = 0;
	Sprite* result = MemAlloc(spriteCount * spriteSize);
	for(int i = 0; i < spriteCount; i++)
	{
		Sprite res = {0};
		res.texture = texture;
		Rectangle rect = {x,y,width,height};
		res.rect = rect;
		res.pivot = pivot;
		//printf("%d %d %d \n", &result[i], x,y);
		result[i] = res;
		x += width;
		if(x > texture.width)
		{
			x = 0; 
			y += height;
		}
	}
	return result;
}

FrameAnimation CreateFrameAnimationFromTexture(Texture2D texture,
			float anim_duration, Vector2 spritePivot,
			int spriteWidth, int spriteHeight)
{
	FrameAnimation result;

	Sprite* sprites = CreateSprites(texture, spritePivot,
						spriteWidth, spriteHeight, &result.frame_count);
	result.sprites = sprites;
	result.duration = anim_duration;
	result.frames = MemAlloc(result.frame_count*sizeof(Frame)); // 8
	for(int i = 0; i < result.frame_count; i++)
	{
		result.frames[i] = (Frame){
			.time = result.duration * i / result.frame_count,
			.position = (Vector2){0},
			.sprite_id = i,
			.rotation = 0,
		};
	}
	return result;
}

Animation CreateAnimation(Texture2D texture,
			float frameDuration, Vector2 spritePivot,
			int spriteWidth, int spriteHeight)
{
	Animation result;
	result.frameDuration = frameDuration;
	Sprite* sprites = CreateSprites(texture, spritePivot,
						spriteWidth, spriteHeight, &result.length);
	result.sprites = sprites;
	return result;
}

Rectangle DetectCollisionRectangle(Image image, int xMax, int yMax)
{
	int minX = xMax; int minY = yMax; 
	int maxX = 0; int maxY = 0;
	for(int y = 0; y < yMax; y++)
	{
		int yOffset = y * image.height;
		for(int x = 0; x < xMax; x++)
		{
			Color color = GetImageColor(image,x,y);
			if(color.a) 
			{
				if(x < minX) minX = x;
				if(y < minY) minY = y; 
				if(x > maxX) maxX = x; 
				if(y > maxY) maxY = y; 
			}
		}
	}
	printf("\n %d, %d, %d, %d", minX, minY, maxX - minX,maxY - minY);
	return (Rectangle) {.x = minX, .y = minY, .width = maxX - minX, .height = maxY - minY};
}

void DetectRectangle(Image image)
{
	for(int y = 0; y < image.height; y++)
	{
		int yOffset = y * image.height;
		for(int x = 0; x < image.width; x++)
		{

			Color color = GetImageColor(image,x,y);
			if(color.a) 
			{
				printf("\n hit picture %d, %d ",x,y);
			}
		}
	}
}
// TODO: optimize. Probably check AABB first. 
bool CheckCollisionRectRotated(Rectangle r1, Rectangle r2,
							float rot1, float rot2, float scaleFactor, Vector2 pixel_offset1, Vector2 pixel_offset2)
{
	rot1 *= PI/180;
	rot2 *= PI/180;

	r1.width *= scaleFactor;
	r1.height *= scaleFactor;
	r2.width *= scaleFactor;
	r2.height *= scaleFactor;

	r1.x += r1.width / 2;
	r1.y += r1.height / 2;
	r2.x += r2.width / 2;
	r2.y += r2.height / 2;

	pixel_offset1 = Vector2Scale(pixel_offset1,scaleFactor);
	pixel_offset2 = Vector2Scale(pixel_offset2,scaleFactor);

	/*
		1. Align with 0:0
		2. Rotate
		3. Shift back
		4. Check collision
		5. Check if rect inside

	*/ 
	
	Vector2 c1 = (Vector2){r1.x + r1.width/2, r1.y + r1.height/2};
	Vector2 c2 = (Vector2){r2.x + r2.width/2, r2.y + r2.height/2};
	Vector2 points1[4] = {
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x, r1.y},                       c1), rot1),c1),pixel_offset1),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x + r1.width, r1.y},            c1), rot1),c1),pixel_offset1),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x + r1.width, r1.y + r1.height},c1), rot1),c1),pixel_offset1),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x, r1.y + r1.height},           c1), rot1),c1),pixel_offset1)
	};
	Vector2 points2[4] = {
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x, r2.y},                       c2), rot2),c2),pixel_offset2),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x + r2.width, r2.y},            c2), rot2),c2),pixel_offset2),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x + r2.width, r2.y + r2.height},c2), rot2),c2),pixel_offset2),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x, r2.y + r2.height},           c2), rot2),c2),pixel_offset2)
	};

	// Check if any point is inside another rect
	for (int i = 0; i < 4; i++) {
		Vector2 p = points1[i];
		bool inside = false;
		int j = 3;
		for (int k = 0; k < 4; k++) {
			if (((points2[k].y > p.y) != (points2[j].y > p.y)) &&
				(p.x < (points2[j].x - points2[k].x) * (p.y - points2[k].y) /
						(points2[j].y - points2[k].y) + points2[k].x))
			{
				inside = !inside;
			}
			j = k;
		}
		if (inside) return true;
	}

	// Check if any point of r2 is inside r1
	for (int i = 0; i < 4; i++) {
		Vector2 p = points2[i];
		bool inside = false;
		int j = 3;
		for (int k = 0; k < 4; k++) {
			if (((points1[k].y > p.y) != (points1[j].y > p.y)) &&
				(p.x < (points1[j].x - points1[k].x) * (p.y - points1[k].y) /
						(points1[j].y - points1[k].y) + points1[k].x))
			{
				inside = !inside;
			}
			j = k;
		}
		if (inside) return true;
	}


	for(int currentPoint1 = 0; currentPoint1 < 4; currentPoint1++){
		int nextPoint1 = currentPoint1 + 1;
		if(nextPoint1 >= 4) nextPoint1 = 0;
		Vector2 pos1 = points1[currentPoint1];
		Vector2 pos2 = points1[nextPoint1];
		for(int currentPoint2 = 0;currentPoint2 < 4; currentPoint2++){
			int nextPoint2 = currentPoint2 + 1;
			if(nextPoint2 >= 4) nextPoint2 = 0;
			Vector2 pos3 = points2[currentPoint2];
			Vector2 pos4 = points2[nextPoint2];
			float denom = (pos4.y-pos3.y)*(pos2.x-pos1.x) - (pos4.x-pos3.x)*(pos2.y-pos1.y); 
			if(fabs(denom) < 1e-6f) continue;
			float uA = ((pos4.x-pos3.x)*(pos1.y-pos3.y) - (pos4.y-pos3.y)*(pos1.x-pos3.x)) / denom;
			float uB = ((pos2.x-pos1.x)*(pos1.y-pos3.y) - (pos2.y-pos1.y)*(pos1.x-pos3.x)) / denom;
			
			//printf("\n%.0f,%.0f, %0.0f",uA,uB, denom);
			if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		    	return true;
			}
		}
	}

	return false;
}

int InitEditAnimationWindow(FrameAnimation editingAnimation, 
							int* editing_frame_count, int parentID, ui_element_datas* uiDatas)
{
	// TODO: implement parenting
	if(parentID != 0) return -1;
	// Create UI background
	Rectangle element = {0,SCREEN_HEIGHT - SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT/4};
	Rectangle element1 = {0,0,SCREEN_WIDTH * 4 / 6,SCREEN_HEIGHT/4};
	ui_element el = create_ui_element(element,(Sprite){0},DARKGRAY,0,ui_flag_rawRect, 2,0,0);
	ui_element el1 = create_ui_element(element1,(Sprite){0},GRAY,1,ui_flag_rawRect, layout_flexibleY_in_parent, layout_relative_pos,0);
	int ui = add_ui_element(uiDatas,el,-1);
	int ui1 = add_ui_element(uiDatas,el1,ui);

	// Construct parent rect1

	ui_element el2 = create_ui_element(element1,(Sprite){0},WHITE,2,ui_flag_rawRect, layout_flexibleX_in_parent, layout_relative_pos,0);
	el2.rect.height /= 4;
	el2.rect.width /= 2;
	int ui_top = add_ui_element(uiDatas,el2,ui1);
	//el1.rect.y += el1.rect.height * 2;
	int ui_mid = add_ui_element(uiDatas,el2,ui1);


	// Create button for sprites
	Rectangle buttonRect = {0,0,48,48};
	//buttonRect.y += 40;
	ui_element sprite_button = 
			create_ui_element(buttonRect, (Sprite){0},LIGHTGRAY,4,ui_flag_button,0,1,0);

	buttonRect = (Rectangle){0,0,8,32};
	ui_element buttonEl = create_ui_element(buttonRect,(Sprite){0},LIGHTGRAY,4,ui_flag_rawRect | ui_flag_button,0,1,AddEventToEditingAnimation);
	for(int i = 0; i < editingAnimation.frame_count; i++)
	{
		int buttonE1 = add_ui_element(uiDatas,buttonEl,ui_top);
		//printf("id: %d flags: %d n: %d \n",buttonE1,uiDatas.data[buttonE1].flags, uiDatas->data[buttonE1].number_in_children );
		sprite_button.sprite = editingAnimation.sprites[i];
		int button_sprite = add_ui_element(uiDatas,sprite_button,ui_mid);
	}

	*editing_frame_count = editingAnimation.frame_count;

	return ui;
}

int ReInitEditAnimationWindowWithNewAnimation(FrameAnimation editingAnimation, 
							int* editing_frame_count, int existingWindowParent, ui_element_datas* uiDatas)
{
	printf("Start reinitting frames: %d\n", editingAnimation.frame_count);
	if(existingWindowParent < 0) return -1;
	ui_element* data = uiDatas->data;
	int parentID =  existingWindowParent + 2; // ADD NUMBER OF UI ELEMENTS HERE
	if(editingAnimation.frame_count < 1 || data[parentID].childrenCount < 1)
	{
		printf("!(editingAnimation.frame_count < 1 || data[parentID].childrenCount). create window again\n");
		return -1;
	}

	for(int i = 0; i < editingAnimation.frame_count; i++)
	{
		int buttonID = -1;
		// TODO: remove out of loop; check lengths
		if(data[parentID].childrenCount <= i)
		{
			printf("Not enough ui elements (bt), resizing\n");
			int copy_id = data[parentID].childrenID[0];
			printf("copy id: %d\n", copy_id);
			ui_element copy = data[copy_id];
			buttonID = add_ui_element(uiDatas, copy, parentID);
			printf("resized\n");
		}else buttonID = data[parentID].childrenID[i];
		/*if(data[parentID + 1].childrenCount <= i)
		{
			printf("Not enough ui elements (sprites), resizing. childrenCount: %d capacity: %d \n", data[parentID + 1].childrenCount, data[parentID + 1].childrenCapacity);
			int copy_id = data[parentID + 1].childrenID[0];
			ui_element copy = data[copy_id];
			copy.sprite = editingAnimation.sprites[i];
			//int button_sprite = add_ui_element(uiDatas, copy, parentID + 1);
			printf("resized\n");
		}*/
		printf("reassigned: id: %d flags: %d n: %d \n",buttonID,data[buttonID].flags, data[buttonID].number_in_children );
	}

	*editing_frame_count = editingAnimation.frame_count;

	return existingWindowParent;
}



