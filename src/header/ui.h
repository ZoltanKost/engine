#ifndef UI
#define UI

#include "raylib.h"
#include "sprite.h"

#define layout_relative_pos 1
#define layout_flexibleX_in_parent 2 
#define layout_flexibleY_in_parent 4

#define  ui_flag_active 1
#define  ui_flag_rawRect 1<<1
#define  ui_flag_button 1<<7
#define  uiColor (Color){0x36,0x36,0x36,0xFF}

typedef struct ui_element
{
	int parentID;
	int* childrenID;
	int number_in_children;
	int childrenCount;
	int childrenCapacity;
	Rectangle rect;
	Rectangle draw_rect; 
	Color color;
	Sprite sprite;
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

ui_element create_ui_element(Rectangle rect, Sprite sprite, Color color, int layer, char flags, char children_flags, char layout_flags, void (*callback)(int));
void calculate_ui_positions(ui_element_datas* UIdata, Camera2D cam);
void draw_ui(ui_element_datas* UIdata, Camera2D cam);
ui_cast_result cast_over_ui(ui_element_datas datas, Vector2 screenInput);
void remove_inactive_elements(ui_element_datas* ui_elementData);
void process_ui(ui_element_datas* data, float dt, Camera2D cam);

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