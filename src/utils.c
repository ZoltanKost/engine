#include "utils.h"

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
