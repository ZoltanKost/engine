#include "animation.h"


bool WriteFrameAnimationToFile(FrameAnimation animation, char* animationName)
{
	char* filename = animationName;
	FILE *file_ptr;
	file_ptr = fopen(animationName,"w");
	memset(file_ptr,0,sizeof(file_ptr));

	fwrite(&animation.frame_count,sizeof(int),1,file_ptr);
	fwrite(&animation.duration,sizeof(float),1,file_ptr);

	printf("writing animation %d %f\n ", animation.frame_count, animation.duration);

	int frame_count = animation.frame_count;

	for(int i = 0; i < frame_count; i++)
	{
		Frame frame = animation.frames[i];
		fwrite(&frame,sizeof(Frame),1,file_ptr);			
	}

	for(int i = 0; i < frame_count; i++)
	{
		Sprite sprite = animation.sprites[i];
		fwrite(&sprite,sizeof(Sprite),1,file_ptr);			
	}	
	fclose(file_ptr);
	return 1;
}

bool ReadFrameAnimation(char* filename, FrameAnimation* animation)
{
	FILE *file_ptr;
	file_ptr = fopen(filename,"r");
	if(!file_ptr)
	{
		printf("Can't open file!");
		return false;
	}

	int num = fread(&animation->frame_count,sizeof(int),1,file_ptr);

	fread(&animation->duration,sizeof(float),1,file_ptr);

	printf("reading animation %d %.5f\n", animation->frame_count, animation->duration);
	
	fread(animation->frames,sizeof(Frame) * animation->frame_count,1,file_ptr);			

	fread(animation->sprites,sizeof(Sprite) * animation->frame_count,1,file_ptr);			

	fclose(file_ptr);
	return 1;
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