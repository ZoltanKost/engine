#include "sprite.h"

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
// TODO: allocation localization
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