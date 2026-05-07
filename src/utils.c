#include "utils.h"
#include <io.h>
static const float hPi = PI / 2;
static const float twoPi = PI * 2;

float ArcLength(Vector2 v1, Vector2 v2)
{
	return Vector2Angle(v1,v2);
}

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

Vector2 Vector2NormalizedSlerp(Vector2 v1, Vector2 v2, float a)
{
	float angle = Vector2Angle(v1,v2);
	float a1 = sinf((1-a)* angle) / sinf(angle);
	float a2 = sinf(a* angle) / sinf(angle);
	v1 = Vector2Scale(v1,a1);
	v2 = Vector2Scale(v2,a2);
	return Vector2Normalize(Vector2Add(v1,v2));
}

float NormalizedDifferenceLength(Vector2 v1, Vector2 v2)
{
	Vector2 diff = Vector2Subtract(v1,v2);
	float l = Vector2LengthSqr(diff);
	return l;
}


float EulerFromVector(Vector2 dir)
{
	float rotation = Vector2Angle(vector2_up,dir);
	rotation = (int)(rotation * RAD2DEG) % 360;
	if(rotation < 0) rotation += 360;
	return rotation;
}

// TODO: optimize. Probably check AABB first. 
bool CheckCollisionRectRotated(Rectangle r1, Rectangle r2,
							float rot1, float rot2, float scaleFactor, Vector2 pixel_offset1, Vector2 pixel_offset2)
{
	rot1 *= PI/180;
	rot2 *= PI/180;

	r1.width *= scaleFactor;
	r1.height *= scaleFactor;
	r2.width *= scaleFactor;
	r2.height *= scaleFactor;

	r1.x += r1.width / 2;
	r1.y += r1.height / 2;
	r2.x += r2.width / 2;
	r2.y += r2.height / 2;

	pixel_offset1 = Vector2Scale(pixel_offset1,scaleFactor);
	pixel_offset2 = Vector2Scale(pixel_offset2,scaleFactor);

	/*
		1. Align with 0:0
		2. Rotate
		3. Shift back
		4. Check collision
		5. Check if rect inside

	*/ 
	
	Vector2 c1 = (Vector2){r1.x + r1.width/2, r1.y + r1.height/2};
	Vector2 c2 = (Vector2){r2.x + r2.width/2, r2.y + r2.height/2};
	Vector2 points1[4] = {
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x, r1.y},                       c1), rot1),c1),pixel_offset1),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x + r1.width, r1.y},            c1), rot1),c1),pixel_offset1),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x + r1.width, r1.y + r1.height},c1), rot1),c1),pixel_offset1),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r1.x, r1.y + r1.height},           c1), rot1),c1),pixel_offset1)
	};
	Vector2 points2[4] = {
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x, r2.y},                       c2), rot2),c2),pixel_offset2),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x + r2.width, r2.y},            c2), rot2),c2),pixel_offset2),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x + r2.width, r2.y + r2.height},c2), rot2),c2),pixel_offset2),
		Vector2Add(Vector2Add(Vector2Rotate(Vector2Subtract((Vector2) {r2.x, r2.y + r2.height},           c2), rot2),c2),pixel_offset2)
	};

	// Check if any point is inside another rect
	for (int i = 0; i < 4; i++) {
		Vector2 p = points1[i];
		bool inside = false;
		int j = 3;
		for (int k = 0; k < 4; k++) {
			if (((points2[k].y > p.y) != (points2[j].y > p.y)) &&
				(p.x < (points2[j].x - points2[k].x) * (p.y - points2[k].y) /
						(points2[j].y - points2[k].y) + points2[k].x))
			{
				inside = !inside;
			}
			j = k;
		}
		if (inside) return true;
	}

	// Check if any point of r2 is inside r1
	for (int i = 0; i < 4; i++) {
		Vector2 p = points2[i];
		bool inside = false;
		int j = 3;
		for (int k = 0; k < 4; k++) {
			if (((points1[k].y > p.y) != (points1[j].y > p.y)) &&
				(p.x < (points1[j].x - points1[k].x) * (p.y - points1[k].y) /
						(points1[j].y - points1[k].y) + points1[k].x))
			{
				inside = !inside;
			}
			j = k;
		}
		if (inside) return true;
	}


	for(int currentPoint1 = 0; currentPoint1 < 4; currentPoint1++){
		int nextPoint1 = currentPoint1 + 1;
		if(nextPoint1 >= 4) nextPoint1 = 0;
		Vector2 pos1 = points1[currentPoint1];
		Vector2 pos2 = points1[nextPoint1];
		for(int currentPoint2 = 0;currentPoint2 < 4; currentPoint2++){
			int nextPoint2 = currentPoint2 + 1;
			if(nextPoint2 >= 4) nextPoint2 = 0;
			Vector2 pos3 = points2[currentPoint2];
			Vector2 pos4 = points2[nextPoint2];
			float denom = (pos4.y-pos3.y)*(pos2.x-pos1.x) - (pos4.x-pos3.x)*(pos2.y-pos1.y); 
			if(fabs(denom) < 1e-6f) continue;
			float uA = ((pos4.x-pos3.x)*(pos1.y-pos3.y) - (pos4.y-pos3.y)*(pos1.x-pos3.x)) / denom;
			float uB = ((pos2.x-pos1.x)*(pos1.y-pos3.y) - (pos2.y-pos1.y)*(pos1.x-pos3.x)) / denom;
			
			//printf("\n%.0f,%.0f, %0.0f",uA,uB, denom);
			if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		    	return true;
			}
		}
	}

	return false;
}