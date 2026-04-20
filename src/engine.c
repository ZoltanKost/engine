#include "engine.h"

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
	SetTargetFPS(1000);
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
	printf("\n playerID: %d %d %d \n", playerShip, enemyShip,  datas.count);
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
				//printf("edit: %d ship[0]: %d ship[1] %d\n", (int)editingAnimation.frames, (int)datas.data[0].animations[2].frames, (int)datas.data[1].animations[2].frames);
				//printf("edit: %d ship[0]: %d ship[1] %d\n", (int)editingAnimation.frames[number_in_children - 1].event, (int)datas.data[0].animations[2].frames[number_in_children - 1].event, (int)datas.data[1].animations[2].frames[number_in_children - 1].event);
				//ship.animations[1] = editingAnimation;
				//datas.data[0].animations[1] = editingAnimation;
				//WriteFrameAnimationToFile(editingAnimation, "shoot_animation.anim");
			}
		}
		Vector2 gamepad_input = {0};
		bool moving = false;
		if(IsGamepadAvailable(0))
		{
			gamepad_input.x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
			gamepad_input.y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
		}
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
		
		if(IsGamepadAvailable(0))
		{
			printf("%.10f %.10f \n", gamepad_input.x, gamepad_input.y);
			datas.data[playerShip].moveDirection = Vector2Normalize(gamepad_input);
			moving = gamepad_input.x > 0.1f || gamepad_input.x < -0.1f || gamepad_input.y > 0.1f || gamepad_input.y < -0.1f;
		}else
		if(IsMouseButtonDown(0))
		{
			//printf("%.5f\n", dt);
			datas.data[playerShip].moveDirection = //Vector2Normalize(mouseRelativePosition);
				Vector2NormalizedSlerp(datas.data[playerShip].moveDirection, Vector2Normalize(mouseRelativePosition),dt * 10);
			//printf("x: %.5f y: %.5f\n",datas.data[playerShip].moveDirection.x, datas.data[playerShip].moveDirection.y);
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
		ProcessBullets(&bulletDatas, dt, scaleFactorX);
		process_ui(&uiDatas,dt,camera);
		DrawFPS(camera.target.x - SCREEN_WIDTH/ 2+ 50, camera.target.y -  SCREEN_HEIGHT/ 2+ 50);

		camera.target = datas.data[playerShip].position;


		EndDrawing();
	}
	CloseWindow();
	return 0;
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


