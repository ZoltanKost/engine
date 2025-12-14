typedef struct Bullet
{
    Sprite sprite;
    Rectangle collider;
    FrameAnimation* animations;
    CurrentAnimationData animData;
    Vector2 moveDirection;
    Vector2 position;
    float rotation;
    float moveSpeed;
    float lifeTime;
    float time;
    int id;
    char team;
    char flags;
} Bullet;

typedef struct BulletDatas
{
    Bullet* data;
    int count;
    int firstInactive;
} BulletDatas;


typedef struct Engine //88 byte
{
    FrameAnimation animation; // 40
    Sprite sprite; // 44 
    int current_frame; // 4
    float time; // 4
} Engine;
typedef struct Ship
{
	Engine engine; // 88 byte
	Bullet bulletToSpawn;
	Rectangle collider; // 16 byte
    Sprite sprite; // 44 byte
    CurrentAnimationData animData; // 12 -- 3 x 4 byte
    int id; // 4
    int animationCount; // 4
    float moveSpeed; // 4
    float rotation; // 4
    FrameAnimation* animations; // 8 byte
    Vector2 position; // 8 
    Vector2 moveDirection; // 8
    char team; // 1
    char flags;// 1 
    char state;// 1
} Ship;

typedef struct ShipDatas
{
    Ship* data;
    int count;
    int firstInactiveShip;
} ShipDatas;

ShipDatas InitShips(int initCount);

void ProcessShips(ShipDatas* shipData, BulletDatas* bulletData, float dt,int scaleFactor);

void ProcessEngine(Vector2 position, float rotation, Engine* engin, float dt, int scaleFactor);

Ship CreateShipLoadAnimations(char* base_texture_path,
								char* shoot_texture_path,
								char* destruction_texture_path,
								char* engine_texture_path,
								char* bullet_texture_path, 
								char* base_animation_path,
								char* shooting_animation_path,
								char* destruction_animation_path,
								int bulletFrameCount, 
								float bulletSpeed, 
								float bulletLifeTime,
				 				float speed, int team);


void RemoveShip(ShipDatas* ships, int id);
void RemoveUnactiveShips(ShipDatas* shipData);
void ProcessRotation(ShipDatas* shipData, float dt);
void ProcessState(ShipDatas* shipData, BulletDatas* bulletDatas);
void ProcessMovement(ShipDatas* shipData, float dt, float scaleFactor);
void ProcessCollisions(ShipDatas* datas, float scaleFactor);
void ProcessAnimation(ShipDatas* data, float dt, int scaleFactor);

#define ship_flag_active 	 1
#define ship_flag_move 		 1 << 1
#define ship_flag_shoot 	 1 << 2
#define ship_flag_reset 	 1 << 6
#define ship_flag_remove 	 1 << 7

#define ship_state_idle      0
#define ship_state_shooting  1
#define ship_state_destroy 	 1 << 7

#include "bullet.c"

ShipDatas InitShips(int initCount)
{
	ShipDatas result;
	result.data = MemAlloc(initCount * sizeof(Ship));
	result.count = initCount;
	result.firstInactiveShip = 0;
	return result;
	//ships[0] = CreatePlayerShip();
}

/*void ReplaceShip(ShipDatas* shipData, Ship ship,int id)
{
	Ship* ships; 
}*/

Ship CreateShip(Texture2D shipTexture, Texture2D shipShootTexture,
				 Texture2D shipDestructionTexture, Texture2D engineTexture, 
				 Bullet bulletToSpawn,
				 float shipSpeed, int team, Vector2 position)
{
	Vector2 offset = {0.5f, 0.5f};
	// TODO: Create collider from prefab
	Image image = LoadImageFromTexture(shipTexture);
	Rectangle ship_collider = DetectCollisionRectangle(image, image.width, image.height);
	UnloadImage(image);
	// TODO: Create Engine struct from engine prefab?? prefab
	Engine engine = {0};
	if(engineTexture.id != -1){
		FrameAnimation engine_animation = 
			CreateFrameAnimationFromTexture(engineTexture, 
				1.0f, offset, engineTexture.height,
				engineTexture.height);
		engine = (Engine){engine_animation,engine_animation.sprites[0],0,0.0f};
	}

	// Create Ship struct
	Ship ship = {0};
	FrameAnimation ship_animation = CreateFrameAnimationFromTexture(
			shipTexture,
			0.5f, (Vector2){0.5f,0.5f},
			64, 64);
	FrameAnimation shootAnimation = CreateFrameAnimationFromTexture(
			shipShootTexture,
			0.5f, (Vector2){0.5f,0.5f},
			64, 64);
	FrameAnimation destructionAnimation = CreateFrameAnimationFromTexture(
			shipDestructionTexture,
			0.75f, (Vector2){0.5f,0.5f},
			64, 64);
	FrameAnimation* anims = MemAlloc(3 * sizeof(FrameAnimation));
	anims[0] = ship_animation;
	anims[1] = shootAnimation;
	anims[2] = destructionAnimation;
	ship.engine = engine;
	ship.collider = ship_collider;
	ship.animations = anims;
	ship.sprite = anims[1].sprites[0];
	ship.moveSpeed = shipSpeed;
	
	CurrentAnimationData animData = {0};
	animData.current_animation = 0;
	ship.flags = 1;
	ship.state = 0;
	ship.team = team;
	ship.animData = animData;
	ship.animationCount = 3;
	ship.position = position;

	//bullet.moveSpeed = moveSpeed;
	bulletToSpawn.team = ship.team;
	ship.bulletToSpawn = bulletToSpawn;

	return ship;
}

void ProcessShips(ShipDatas* shipData, BulletDatas* bulletData, float dt,int scaleFactor)
{
	Ship* ships = shipData->data;
	int firstInactive = shipData->firstInactiveShip;
	RemoveUnactiveShips(shipData);
	ProcessState(shipData, bulletData);
	ProcessMovement(shipData, dt,scaleFactor);
	ProcessRotation(shipData, dt);
	ProcessCollisions(shipData, scaleFactor);
	ProcessAnimation(shipData, dt,scaleFactor);
}

// TODO: refactor into state arrays
void ProcessState(ShipDatas* shipData, BulletDatas* bulletDatas)
{
	Ship* data = shipData->data;
	for(int i = 0; i < shipData->firstInactiveShip; i++)
	{
		Ship ship = data[i];
		if(!(ship.flags & ship_flag_active)) continue;
		if(ship.flags & ship_flag_reset) 
		{
			printf("resetted state %d state: %d \n", ship.id, ship.state);
			ship.state = 0;
			ship.flags ^= ship_flag_reset;
		}
		switch(ship.state)
		{
			case (ship_state_shooting):
				printf("%d state: %d \n", ship.id, ship.state);
				if(ship.animData.current_animation != 1)
				{
					printf("anim: %d \n", ship.animData.current_animation);
					ship.animData = (CurrentAnimationData){0};
					ship.animData.current_animation = 1;
					break;
				}else if(!(ship.flags & ship_flag_shoot)) break;
				ship.flags^=ship_flag_shoot;
				ship.bulletToSpawn.position = ship.position;
				Vector2 moveAddition = vector2_zero;
				ship.bulletToSpawn.moveDirection = Vector2Normalize(ship.moveDirection);

				SpawnBullet(bulletDatas,ship.bulletToSpawn);

				break;
			case(ship_state_destroy):
				if(ship.animData.current_animation != 2)
				{
					ship.animData = (CurrentAnimationData){0};
					ship.animData.current_animation = 2;
				}
				break;
			default: ship.animData.current_animation = 0;
		}
		data[i] = ship;
	}
}

void RemoveUnactiveShips(ShipDatas* shipData)
{
	Ship* ships = shipData->data;
	for(int i = 0; i < shipData->firstInactiveShip; i++)
	{
		if((ships[i].flags & ship_flag_remove))
		{
			RemoveShip(shipData,i);
			i--;
			continue;
		}
	}
}

int AddShip(ShipDatas* datas, Ship shipToAdd)
{
	ShipDatas data = *datas;
	int firstInactive = data.firstInactiveShip;
	int count = data.count;
	Ship* ships = data.data;
	printf("\n firstInactive: %d  count: %d", firstInactive, count);
	if(firstInactive >= count)
	{
		//printf("allocating size: %d ", count * 2 * sizeof(Ship));
		ships = MemRealloc(ships,count * 2 * sizeof(Ship));
		data.data = ships;
		//printf("\n array reallocated  ");
		data.count = count * 2;
		//printf("Ship Array resized!, count is %d ", data.count);
	}
	shipToAdd.id = firstInactive;
	ships[firstInactive++] = shipToAdd;
	data.firstInactiveShip = firstInactive;

	*datas = data;
	return shipToAdd.id;
}

// TODO: probably animation and sprite drawing stuff should be separate
void ProcessAnimation(ShipDatas* data, float dt, int scaleFactor)
{	
	Ship* ships = data->data;
	int firstInactive = data->firstInactiveShip;
	for(int i = 0; i < firstInactive; i++)
	{
		Ship ship = ships[i];
		CurrentAnimationData animData = ship.animData;
		int current_anim = animData.current_animation;
		int current_frame = animData.current_frame;
		animData.time += dt;
		FrameAnimation currentAnim = ship.animations[current_anim];
		if(animData.time > currentAnim.duration) 
		{
			animData.time = 0;
			animData.current_frame = 0;
		}
		if(animData.time > currentAnim.frames[current_frame].time 
			&& (animData.current_frame < currentAnim.frame_count))
		{
			ship.flags = ship.flags | currentAnim.frames[animData.current_frame].event;
			if(ship.id == 0) printf("anim: %d frame %d sets flags: %d\n", current_anim, current_frame, currentAnim.frames[animData.current_frame].event);
			int sprite_id = currentAnim.frames[animData.current_frame++].sprite_id;
			ship.sprite = currentAnim.sprites[sprite_id];
		}

		ship.animData = animData;
		DrawSpriteRotated(ship.sprite,
			ship.position,ship.rotation,scaleFactor);
		ships[i] = ship;
	}
}

void ProcessMovement(ShipDatas* shipData, float dt, float scaleFactor)
{
	Ship* ships = shipData->data;
	int firstInactive = shipData->firstInactiveShip;
	for(int i = 0; i < firstInactive; i++)
	{
		Ship ship = ships[i];
		if(ship.flags & ship_flag_move && ship.flags & ship_flag_active)
		{
			Vector2 moveVector = {ship.moveDirection.x * ship.moveSpeed * dt,
								ship.moveDirection.y * ship.moveSpeed * dt};
			ship.position = Vector2Add(ship.position, moveVector);
			if(ship.engine.animation.sprites != NULL)
			ProcessEngine(ship.position, ship.rotation, &ship.engine, dt,scaleFactor);
			ships[i] = ship;
		}
	}
}

void ProcessRotation(ShipDatas* shipData, float dt)
{
	Ship* ships = shipData->data;
	int firstInactive = shipData->firstInactiveShip;
	for(int i = 0; i < firstInactive; i++)
	{
		Ship ship = ships[i];
		ship.rotation = Vector2Angle(up,ship.moveDirection);
		ship.rotation = (int)(ship.rotation * RAD2DEG) % 360;
		if(ship.rotation < 0) ship.rotation += 360;
		ships[i] = ship;
	}
}

inline void RemoveShip(ShipDatas* ships, int id)
{
	Ship* data = ships->data;
	int firstInactive = ships->firstInactiveShip;
	data[id] = data[--firstInactive];
	data[firstInactive].flags = 0;
	data[id].id = id;
	ships->firstInactiveShip = firstInactive;
}

void ProcessEngine(Vector2 position, float rotation, Engine* _engine, float dt, int scaleFactor)
{
	Engine engine = *_engine;
	engine.time += dt;
	if(engine.time > engine.animation.duration)
	{
		engine.time = 0;
		engine.current_frame = 0;
	}
	if(engine.time > engine.animation.frames[engine.current_frame].time 
		&& (engine.current_frame < engine.animation.frame_count))
	{
		/*if(++engine.current_frame >= engine.animation.frame_count)
		{
			engine.current_frame = 0;
		}*/
		int sprite_id =  engine.animation.frames[engine.current_frame++].sprite_id;
		engine.sprite = engine.animation.sprites[sprite_id];
	}

	Vector2 enginePos = position;
	DrawSpriteRotated(engine.sprite,enginePos,rotation,scaleFactor);
	*_engine = engine;
}

void ProcessCollisions(ShipDatas* datas, float scaleFactor)
{
	int firstInactive = datas->firstInactiveShip;
	Ship* ships = datas->data;
	for(int i = 0; i < firstInactive; i++){
		Ship ship = ships[i];
		if(!(ship.flags & ship_flag_active)) continue;
		Rectangle rec1 = ship.sprite.rect;
		rec1.x = ship.collider.x + ship.position.x;
		rec1.y = ship.collider.y + ship.position.y;
		rec1.width = ship.collider.width;
		rec1.height = ship.collider.height;
		for(int l = i; l < firstInactive; l++)
		{
			if(ships[l].team != ship.team && (ships[l].flags & ship_flag_active) && (ship.flags & ship_flag_active))
			{
				Rectangle rec2 = ships[l].sprite.rect;
				rec2.x = ships[l].collider.x + ships[l].position.x;
				rec2.y = ships[l].collider.y + ships[l].position.y;
				rec2.width = ships[l].collider.width;
				rec2.height = ships[l].collider.height;
				if(CheckCollisionRectRotated(rec1, rec2, 
										ship.rotation, ships[l].rotation, 
										scaleFactor,//vector2_zero,vector2_zero))
										(Vector2){-0.5f * ship.sprite.rect.width, -0.5f * ship.sprite.rect.height},
										(Vector2){-0.5f * ships[l].sprite.rect.width, -0.5f * ships[l].sprite.rect.height}))
				{
					if(ships[l].team != 0) 
						{
							ships[l].flags = ships[l].flags & (~ship_flag_active);
							ships[l].animData = (CurrentAnimationData){0};
							ships[l].animData.current_animation = 2;
						}
					if(ship.team != 0) {
						//ship.flags = ship.flags & ~ship_active;
						//CloseWindow();
						printf("\ncollide");
					}
				} 
			}
		}
	}
}

// TODO: do not create animations from textures. 
// Only assign textures to existing sprites .
Ship CreateShipLoadAnimations(char* base_texture_path,
								char* shoot_texture_path,
								char* destruction_texture_path,
								char* engine_texture_path,
								char* bullet_texture_path, 
								char* base_animation_path,
								char* shooting_animation_path,
								char* destruction_animation_path,
								int bulletFrameCount, 
								float bulletSpeed, 
								float bulletLifeTime,
				 				float speed, int team)
{
	Texture2D base_ship_Texture = LoadTexture(base_texture_path);
	Texture2D shooting_ship_Texture = LoadTexture(shoot_texture_path);
	Texture2D destruction_ship_Texture = LoadTexture(destruction_texture_path);
	Texture2D engine_ship_Texture = LoadTexture(engine_texture_path);
	Texture2D bullet_Texture = LoadTexture(bullet_texture_path);

	Bullet bulletToSpawn = CreateBullet(bullet_Texture,bulletFrameCount,
										bulletSpeed,bulletLifeTime);


	Ship ship = CreateShip(base_ship_Texture, shooting_ship_Texture,
				 destruction_ship_Texture, engine_ship_Texture, bulletToSpawn,
				 speed, team, vector2_zero);

	if(base_animation_path != 0)  
	{
		ReadFrameAnimation(base_animation_path,&ship.animations[0]);
		int count = ship.animations[0].frame_count;
		for(int i = 0; i < count; i++)
		{
			ship.animations[0].sprites[i].texture = base_ship_Texture;
		}
	}

	if(shooting_animation_path != 0)  
	{
		ReadFrameAnimation(shooting_animation_path,&ship.animations[1]);
		int count = ship.animations[1].frame_count;
		for(int i = 0; i < count; i++)
		{
			ship.animations[1].sprites[i].texture = shooting_ship_Texture;
		}
	}

	if(destruction_animation_path != 0) 
	{
		ReadFrameAnimation(destruction_animation_path,&ship.animations[2]);
		int count = ship.animations[2].frame_count;
		for(int i = 0; i < count; i++)
		{
			ship.animations[2].sprites[i].texture = destruction_ship_Texture;
		}
	}

	return ship;
}