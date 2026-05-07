#ifndef SHIP
#define SHIP
#include "bullet.h"

#define ship_flag_active 	 1
#define ship_flag_move 		 1 << 1
#define ship_flag_rotate     1 << 2
#define ship_flag_shoot 	 1 << 3
#define ship_flag_destroy 	 1 << 4
#define ship_flag_reset 	 1 << 6
#define ship_flag_remove 	 1 << 7

#define ship_state_idle      0
#define ship_state_shooting  1
#define ship_state_destroy 	 1 << 7


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
    float rotationVelocity;
    FrameAnimation* animations; // 8 byte
    Vector2 position; // 8 
    Vector2 lookDirection; // 8
    Vector2 targetDirection;
    char team; // 1
    unsigned char flags;// 1 
    unsigned char state;// 1
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


void        RemoveShip(ShipDatas* ships, int id);
void        RemoveUnactiveShips(ShipDatas* shipData);
void        ProcessRotation(ShipDatas* shipData, float dt);
void        ProcessState(ShipDatas* shipData, BulletDatas* bulletDatas);
void        ProcessMovement(ShipDatas* shipData, float dt, float scaleFactor);
void        ProcessCollisions(ShipDatas* datas, float scaleFactor);
void        ProcessAnimation(ShipDatas* data, float dt, int scaleFactor);
void        ProcessBulletCollisions(BulletDatas* bulletData, ShipDatas* shipData, int scaleFactor);
#endif