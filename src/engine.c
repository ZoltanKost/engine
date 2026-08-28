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
	printf("First texture: %d\n", space_stars.id);
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
		Vector2 gamepad_input = {0};
		bool moving = false;

		spawn += dt;

		bool mousePressed = IsMouseButtonPressed(0);

		if(mousePressed)
		{
			CastUI(mousePos, uiDatas);
		}
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
		
		// Parallax
		Vector2 targetVector = {0};
		if(datas.data[playerShip].flags & ship_flag_move){
			targetVector = datas.data[playerShip].lookDirection;
		}else
		{
			targetVector = vector2_zero;
		}
		parallaxOffset = Vector2Lerp(parallaxOffset, targetVector, dt);
		DrawBackgroundParallax(camera, parallaxOffset,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			unitsInWidth, unitsInHeight,
			space_dust, space_nebulae, space_stars);
		
			// Movement uiInput
		if(IsGamepadAvailable(0))
		{
			printf("%.10f %.10f \n", gamepad_input.x, gamepad_input.y);
			datas.data[playerShip].targetDirection = Vector2Normalize(gamepad_input);
			moving = gamepad_input.x > 0.5f || gamepad_input.x < -0.5f || gamepad_input.y > 0.5f || gamepad_input.y < -0.5f;
		}else
		if(IsMouseButtonDown(0))
		{
			//printf("%.5f\n", dt);
			datas.data[playerShip].targetDirection = Vector2Normalize(mouseRelativePosition);
				//Vector2NormalizedSlerp(datas.data[playerShip].lookDirection, Vector2Normalize(mouseRelativePosition),dt);
			//printf("x: %.5f y: %.5f\n",datas.data[playerShip].lookDirection.x, datas.data[playerShip].lookDirection.y);
			moving = true;
		}else
		{
			bool a = IsKeyDown(KEY_A); 
			bool s = IsKeyDown(KEY_S);
			bool d = IsKeyDown(KEY_D); 
			bool w = IsKeyDown(KEY_W);
			if(a || s || w || d)
			{
				datas.data[playerShip].lookDirection = vector2_zero;
				if(a)
				{
					datas.data[playerShip].lookDirection = 
					Vector2Add(datas.data[playerShip].lookDirection,
					 (Vector2){-1,0});
					moving = 1;
				}
				if(s)
				{
					datas.data[playerShip].lookDirection = 
					Vector2Add(datas.data[playerShip].lookDirection,
					 (Vector2){0,1});
					moving = 1;
				}
				if(d)
				{
					datas.data[playerShip].lookDirection = 
					Vector2Add(datas.data[playerShip].lookDirection,
					 (Vector2){1,0});
					moving = 1;
				}
				if(w)
				{
					datas.data[playerShip].lookDirection = 
					Vector2Add(datas.data[playerShip].lookDirection,
					 (Vector2){0,-1});
					moving = 1;
				}
			}
		}

		if((IsMouseButtonDown(1) || IsKeyDown(KEY_SPACE)))
		{
			datas.data[playerShip].state |= ship_state_shooting;
		}


		if(moving) 
		{
			datas.data[playerShip].flags = datas.data[playerShip].flags | ship_flag_move | ship_flag_rotate;
		}
		else datas.data[playerShip].flags = datas.data[playerShip].flags & (~(ship_flag_move | ship_flag_rotate));
		//bulletTimer += dt;

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
// todo: refactor; probably has to be moved to ui
void CastUI(Vector2 mousePos, ui_element_datas uiDatas)
{
		ui_cast_result uiInput = {-1,-1};
		uiInput = cast_over_ui(uiDatas, mousePos);
		if(uiInput.id > -1)
		{
			int number_in_children = uiDatas.data[uiInput.id].number_in_children;
			if(uiInput.flags & ui_flag_button && uiDatas.data[uiInput.id].callback != 0)
			{
				uiDatas.data[uiInput.id].callback(number_in_children); // todo: change what to return based on flags/parent flags
			}
			//printf("edit: %d ship[0]: %d ship[1] %d\n", (int)editingAnimation.frames, (int)datas.data[0].animations[2].frames, (int)datas.data[1].animations[2].frames);
			//printf("edit: %d ship[0]: %d ship[1] %d\n", (int)editingAnimation.frames[number_in_children - 1].event, (int)datas.data[0].animations[2].frames[number_in_children - 1].event, (int)datas.data[1].animations[2].frames[number_in_children - 1].event);
			//ship.animations[1] = editingAnimation;
			//datas.data[0].animations[1] = editingAnimation;
			//WriteFrameAnimationToFile(editingAnimation, "shoot_animation.anim");
		}
}