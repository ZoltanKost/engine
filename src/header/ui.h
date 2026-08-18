#ifndef UI
#define UI

#include "raylib.h"
#include "sprite.h"

#define layout_null 				0
//#define layout_relative_pos 		1    all the positions of all the children are relative to the parent by default
#define layout_rowX				  	(1 << 1)
#define layout_rowY				  	(1 << 2) 
#define layout_flexibleX_in_parent  (1 << 3)
#define layout_flexibleY_in_parent 	(1 << 4)

#define children_layout_rowX				  	(1 << 1) 
#define children_layout_rowY				  	(1 << 2) 
#define children_layout_flexibleX_in_parent  	(1 << 3) 
#define children_layout_flexibleY_in_parent 	(1 << 4)


#define  ui_flag_null	 	(0)
#define  ui_flag_active 	(1)
#define  ui_flag_rawRect 	(1 << 1)	// 2
#define  ui_flag_text 		(1 << 2)	// 4
#define  ui_flag_button 	(1 << 3)	// 8
#define  ui_flag_dirty 		(1 << 7) 	// -128

#define  ui_empty_sprite			((Sprite){0}			)	
#define  ui_rect(w,h)				((Rectangle){0,0,w,h}	)
#define  ui_rect_offset(x,y,w,h)	((Rectangle){x,y,w,h}	)
#define  ui_null_text				((char*)0				)
#define	 ui_null_callback 			0

#define  uiColor (Color){0x36,0x36,0x36,0xFF}
// todo: rename types into PascalCase
typedef struct ui_element
{
	int parentID;
	int* childrenID;
	int number_in_children;	// id of this element in the parent chlidlrenID array; 0-indexed
	int childrenCount;
	int childrenCapacity;
	Rectangle rect; 			// logical rectangle. 0, 0 is top left
	Rectangle draw_rect;		// relative to camera rectangle. top left is camera.x, camera.y 
	Color color;
	Sprite sprite;
	char* text;
	void (*callback)(int);
	int layer;
	char children_layout;
	char layout_flags;
	char flags;
	int id;
}ui_element;

typedef struct ui_element_datas
{
	ui_element* data;
	int count;
	int firstInactive;
}ui_element_datas;

typedef struct ui_cast_result
{
	int id;
	char flags;
}ui_cast_result;

ui_element_datas init_ui(int initCount);

int add_ui_element(ui_element_datas* data, ui_element element, int parentID);
/*
	param: rect - rectangle specifying position and size of ui element
	sprite - image which should be displayed whithin that element 
	color - rectangle color
	layer - ??
	flags - ui_flag_active always set to 1
	children_flags children flags
	layout_flags - e.g. layout_relative_pos or layout_flexibleX_in_parent
	callback - callback to be called when pressing a button
*/
ui_element create_ui_element(Rectangle rect, Sprite sprite, Color color, char flags, char children_layout_flags, char layout_flags, void (*callback)(int));
ui_element create_ui_element_raw_rect(Rectangle rect, Color color, char flags, char children_layout_flags, char layout_flags);
ui_element create_ui_element_button(Rectangle rect, Sprite sprite, Color color, char flags, char layout_flags, void (*callback)(int), char* text);
ui_element create_ui_element_label(Rectangle rect, Color color, char layout_flags, char* text);
ui_element create_ui_element_sprite(Rectangle rect, Sprite sprite, Color color, char flags, char layout_flags);

void calculate_ui_positions(ui_element_datas* UIdata, Camera2D cam);
void draw_ui(ui_element_datas* UIdata, Camera2D cam);
ui_cast_result cast_over_ui(ui_element_datas datas, Vector2 screenInput);
void remove_inactive_elements(ui_element_datas* ui_elementData);
void process_ui(ui_element_datas* data, float dt, Camera2D cam);

void deactivate_ui_element(ui_element_datas* ui_elements, int id);

void activate_ui_element(ui_element_datas* ui_elements, int id);
// TODO: childrenID are not changing
void remove_ui_element(ui_element_datas* ui_elements, int id);

// TODO: write the "gaps" in array into the sorted binary tree.
// then, when creaeting new element pick the lowest id for the element.
// when removing elements, just write their ids to the tree and make inactive
int remove_children(ui_element_datas* ui_elements, int id);
int remove_children_rm_self(ui_element_datas* ui_elements, int id);
void remove_ui_element_no_replace(ui_element_datas* ui_elements, int id);
bool point_over_rect(Vector2 p, Rectangle rect);

#endif