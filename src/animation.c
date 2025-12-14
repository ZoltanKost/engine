typedef struct Sprite // 42 byte;
{
	Texture2D texture; // 20 TODO: can be moved out to some texture_sprites struct
	Rectangle rect; // 16
	Vector2 pivot; // 8
} Sprite;

typedef struct Animation
{
	Sprite* sprites; // 8
	int length; // 4
	float frameDuration; // 4
} Animation;

typedef struct Frame
{
	float time; // 4b
	Vector2 position; // 8b
	int sprite_id; // 4 byte
	float rotation; // 4 byte
	char event; // 1 byte
} Frame;

typedef struct FrameAnimation // 36 byte 40 byte
{
	Frame* frames; // 8
	Sprite* sprites; // 8
	int frame_count; // 4
	float duration; // 4
} FrameAnimation;

typedef struct CurrentAnimationData
{
	int current_frame; 
	int current_animation;
	float time;
} CurrentAnimationData;

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