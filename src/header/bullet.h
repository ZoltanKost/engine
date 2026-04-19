#ifndef BULLET_MODULE
#define BULLET_MODULE
#include "animation.h"
#include "utils.h"
#include "raymath.h"

static const char bullet_active = 1;
static const char bullet_moving = 1 << 1;

// TODO: Do not spawn whole animation data.  reference the assets loaded in memory
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

BulletDatas InitBullets(int initCount);
Bullet      CreateBullet(Texture2D bulletTexture, int count, float speed, float lifeTime);
int         AddConcreteBullet(BulletDatas* datas, Bullet bulletToAdd);
void        RemoveBullet(BulletDatas* bullets, int id);
void        RemoveInactiveBullets(BulletDatas* bulletData);
void        ProcessBulletLifetime(BulletDatas* bulletData, float dt);
void        ProcessBulletMovement(BulletDatas* bulletData, float dt);
void        ProcessBulletRotation(BulletDatas* bulletData);
void        ProcessBulletAnimation(BulletDatas* bulletData, float dt, int scaleFactor);
void        ProcessBullets(BulletDatas* bulletData, float dt, int scaleFactor);
#endif