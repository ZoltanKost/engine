
static const char bullet_active = 1;
static const char bullet_moving = 1 << 1;

BulletDatas InitBullets(int initCount);
int AddConcreteBullet(BulletDatas* datas, Bullet bulletToAdd);
void RemoveBullet(BulletDatas* bullets, int id);
void RemoveInactiveBullets(BulletDatas* bulletData);
void ProcessBulletLifetime(BulletDatas* bulletData, float dt);
void ProcessBulletMovement(BulletDatas* bulletData, float dt);
void ProcessBulletRotation(BulletDatas* bulletData);
void ProcessBulletCollisions(BulletDatas* bulletData, ShipDatas* shipData, int scaleFactor);
void ProcessBulletAnimation(BulletDatas* bulletData, float dt, int scaleFactor);
void ProcessBullets(BulletDatas* bulletData, ShipDatas* shipData, float dt, int scaleFactor);
Bullet CreateBullet(Texture2D bulletTexture, int count, float speed, float lifeTime);

// ---------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------
BulletDatas InitBullets(int initCount)
{
    BulletDatas result;
    result.data = MemAlloc(initCount * sizeof(Bullet));
    result.count = initCount;
    result.firstInactive = 0;
    return result;
}

void ProcessBullets(BulletDatas* bulletData, ShipDatas* shipData, float dt, int scaleFactor)
{
    RemoveInactiveBullets(bulletData);
    ProcessBulletLifetime(bulletData, dt);
    ProcessBulletMovement(bulletData, dt);
    ProcessBulletRotation(bulletData);
    ProcessBulletCollisions(bulletData, shipData, scaleFactor);
    ProcessBulletAnimation(bulletData, dt, scaleFactor);
}

Bullet CreateBullet(Texture2D bulletTexture, int animationCount, float speed, float lifeTime)
{
	int width = bulletTexture.width / animationCount;
	int height = bulletTexture.height;
	Image image = LoadImageFromTexture(bulletTexture);
	Rectangle bullet_collider = DetectCollisionRectangle(image,width, height);
	UnloadImage(image);

	FrameAnimation bulletAnim = CreateFrameAnimationFromTexture(bulletTexture,
			0.5f, (Vector2){0.5f,0.5f},
			width, height);
	FrameAnimation* animations = MemAlloc(sizeof(FrameAnimation));
	animations[0] = bulletAnim;

	return 
	(Bullet){
		.collider = bullet_collider,
		.sprite = bulletAnim.sprites[0],
		.animations = animations,
		.animData = 0,
		.moveDirection = vector2_zero,
		.position = vector2_zero,
		.moveSpeed = speed,
		.lifeTime = lifeTime,
		.flags = 1
	};
}

// ---------------------------------------------------------------
// Add & Remove
// ---------------------------------------------------------------
int AddConcreteBullet(BulletDatas* datas, Bullet bulletToAdd)
{
    BulletDatas data = *datas;
    int firstInactive = data.firstInactive;
    int count = data.count;
    Bullet* bullets = data.data;

    if (firstInactive >= count)
    {
        bullets = MemRealloc(bullets, count * 2 * sizeof(Bullet));
        data.data = bullets;
        data.count = count * 2;
    }

    bulletToAdd.id = firstInactive;
    bullets[firstInactive++] = bulletToAdd;
    data.firstInactive = firstInactive;
    *datas = data;

    return bulletToAdd.id;
}

void SpawnBullet(BulletDatas* datas, Bullet bulletToSpawn)
{
    BulletDatas data = *datas;
    int firstInactive = data.firstInactive;
    int count = data.count;
    Bullet* bullets = data.data;

    if (firstInactive >= count)
    {
        bullets = MemRealloc(bullets, count * 2 * sizeof(Bullet));
        data.data = bullets;
        data.count = count * 2;
    }

    bulletToSpawn.id = firstInactive;
    bullets[firstInactive++] = bulletToSpawn;
    data.firstInactive = firstInactive;
    *datas = data;

}

void RemoveBullet(BulletDatas* bullets, int id)
{
    Bullet* data = bullets->data;
    int firstInactive = bullets->firstInactive;
    data[id] = data[--firstInactive];
    data[firstInactive].flags = 0;
    data[id].id = id;
    bullets->firstInactive = firstInactive;
}


// ---------------------------------------------------------------
// Component: RemoveInactiveBullets
// ---------------------------------------------------------------
void RemoveInactiveBullets(BulletDatas* bulletData)
{
    Bullet* bullets = bulletData->data;
    for (int i = 0; i < bulletData->firstInactive; i++)
    {
        if (!(bullets[i].flags & bullet_active))
        {
            RemoveBullet(bulletData, i);
            i--;
        }
    }
}


// TODO: default bullets lifetime?
void ProcessBulletLifetime(BulletDatas* bulletData, float dt)
{
    Bullet* bullets = bulletData->data;
    for (int i = 0; i < bulletData->firstInactive; i++)
    {
        bullets[i].time += dt;
        if (bullets[i].time >= bullets[i].lifeTime)
        {
            RemoveBullet(bulletData, i);
            i--;
        }
    }
}


// ---------------------------------------------------------------
// Component: ProcessBulletMovement
// ---------------------------------------------------------------
void ProcessBulletMovement(BulletDatas* bulletData, float dt)
{
    Bullet* bullets = bulletData->data;
    for (int i = 0; i < bulletData->firstInactive; i++)
    {
        Bullet bullet = bullets[i];
        Vector2 moveVector = {
            bullet.moveDirection.x * bullet.moveSpeed * dt,
            bullet.moveDirection.y * bullet.moveSpeed * dt
        };
        bullet.position = Vector2Add(bullet.position, moveVector);
        bullets[i] = bullet;
    }
}


// ---------------------------------------------------------------
// Component: ProcessBulletRotation
// ---------------------------------------------------------------
void ProcessBulletRotation(BulletDatas* bulletData)
{
    Bullet* bullets = bulletData->data;
    for (int i = 0; i < bulletData->firstInactive; i++)
    {
        Bullet bullet = bullets[i];
        bullet.rotation = Vector2Angle(up, bullet.moveDirection);
        bullet.rotation = (int)(bullet.rotation * RAD2DEG) % 360;
        if (bullet.rotation < 0) bullet.rotation += 360;
        bullets[i] = bullet;
    }
}


// ---------------------------------------------------------------
// Component: ProcessBulletCollisions
// ---------------------------------------------------------------
void ProcessBulletCollisions(BulletDatas* bulletData, ShipDatas* shipData, int scaleFactor)
{
    Bullet* bullets = bulletData->data;
    int bulletCount = bulletData->firstInactive;
    int shipCount = shipData->firstInactiveShip;
    Ship* ships = shipData->data;

    for (int i = 0; i < bulletCount; i++)
    {
        Bullet bullet = bullets[i];
        if (!(bullet.flags & bullet_active)) continue;

        for (int s = 0; s < shipCount; s++)
        {
            Ship ship = ships[s];
            if (bullet.team == ship.team || ~ship.flags & ship_flag_active) continue;

            Rectangle rec1 = bullet.sprite.rect;
            rec1.x = bullet.collider.x + bullet.position.x;
            rec1.y = bullet.collider.y + bullet.position.y;
            rec1.width = bullet.collider.width;
            rec1.height = bullet.collider.height;

            Rectangle rec2 = ship.sprite.rect;
            rec2.x = ship.collider.x + ship.position.x;
            rec2.y = ship.collider.y + ship.position.y;
            rec2.width = ship.collider.width;
            rec2.height = ship.collider.height;

            if (CheckCollisionRectRotated(
                rec1, rec2,
                bullet.rotation, ship.rotation,
                scaleFactor,
                (Vector2){-0.5f * bullet.sprite.rect.width, -0.5f * bullet.sprite.rect.height},
                (Vector2){-0.5f * ship.sprite.rect.width, -0.5f * ship.sprite.rect.height}))
            {
                ships[s].state = ship_state_destroy;
                // TODO: state control
                ships[s].animData = (CurrentAnimationData){0};
                ships[s].animData.current_animation = 2; 
                bullet.flags = bullet.flags & (~bullet_active);
                printf("\n hit! bullet %d -> ship %d", i, s);
            }
        }
        bullets[i] = bullet;
    }
}


// ---------------------------------------------------------------
// Component: ProcessBulletAnimation
// ---------------------------------------------------------------
void ProcessBulletAnimation(BulletDatas* bulletData, float dt, int scaleFactor)
{
    Bullet* bullets = bulletData->data;
    for (int i = 0; i < bulletData->firstInactive; i++)
    {
        Bullet bullet = bullets[i];
        CurrentAnimationData animData = bullet.animData;
        int current_anim = animData.current_animation;
        int current_frame = animData.current_frame;
        animData.time += dt;
        FrameAnimation currentAnim = bullet.animations[current_anim];
        if (animData.time > currentAnim.duration) 
        {
            animData.time = 0;
            animData.current_frame = 0;
        }
        if (animData.time > currentAnim.frames[current_frame].time
            && (animData.current_frame < currentAnim.frame_count))
        {
            /*if (++animData.current_frame >= bullet.animations[current_anim].frame_count)
            {
                animData.current_frame = 0;
            }*/
            int sprite_id = currentAnim.frames[animData.current_frame++].sprite_id;
            bullet.sprite = currentAnim.sprites[sprite_id];
        }

        bullet.animData = animData;

        DrawSpriteRotated(
            bullet.sprite,
            bullet.position,
            bullet.rotation,
            scaleFactor
        );

        bullets[i] = bullet;
    }
}