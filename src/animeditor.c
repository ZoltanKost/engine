#include "animeditor.h"

/*
	Animations are made from textures, from sprites, and they have Events attached to frames.
	AnimEditor opens a window with a look of sprites and attached events
	Click to the event button should attach a specific event to specific frame. Events are specified in window.
	Animation window has a scroll with all availible animations.
	Animation window also has save button.
*/

static FrameAnimation editing_animation = {0};
static char current_event_animation_flag = 1 << 6;

static int animeditor_window_parent = -1;
static ui_element_datas* ui_data_array = 0;
static char animeditor_active = 1;
static char* chosen_animation_flag_string = "test";
static int chosen_aim_flag_string_label = 0;
void AddFrameToEditingAnimation(int paramsCount)
{
	// TODO: allocations
	return;
	printf("Adding frame...\n");
	editing_animation.frames = MemRealloc(editing_animation.frames, editing_animation.frame_count++);
}

void AddEventToEditingAnimation(int frameNumber)
{
    //printf("frames:%d number:%d", (int)editing_animation.frames, frameNumber);
	editing_animation.frames[frameNumber].event ^= current_event_animation_flag;
	printf("for frame %d added event %d \n",frameNumber,current_event_animation_flag);
}

void SwitchEditingAnimationFlag(int id)
{
    do
    {
        if(current_event_animation_flag & ship_flag_remove)
        {
            current_event_animation_flag = ship_flag_shoot;
			break;
        }
        current_event_animation_flag <<= 1;
    }while(!((current_event_animation_flag & ship_flag_reset) // 64 
        || (current_event_animation_flag & ship_flag_shoot) // 8
        || (current_event_animation_flag & ship_flag_remove))); // -128
	switch(current_event_animation_flag)
	{
		case ship_flag_reset:
			chosen_animation_flag_string = "Reset";
			printf("event changed: %s\n",chosen_animation_flag_string); // TODO here set the image of animation?
			break;
		case ship_flag_shoot:
			chosen_animation_flag_string = "Shoot";
			printf("event changed: %s\n",chosen_animation_flag_string);
			break;
		case -(ship_flag_remove):								// char c = 128 wrapped to -127
			chosen_animation_flag_string = "Remove";
			printf("event changed: %s\n",chosen_animation_flag_string);
			break;
	}
    printf("flag: %d\n", current_event_animation_flag);
}

void CloseEditAnimationWindow(int param)
{
	if(animeditor_active)
	{
		deactivate_ui_element(ui_data_array,animeditor_window_parent);
	}else
	{
		activate_ui_element(ui_data_array,animeditor_window_parent);
	}
	animeditor_active = !animeditor_active;
}



int InitEditAnimationWindow(FrameAnimation _editing_animation, 
							int* editing_frame_count, int parentID, ui_element_datas* uiDatas)
{
	ui_data_array = uiDatas;
    editing_animation = _editing_animation;
	// TODO: implement parenting
	if(parentID != 0) 
	{
		printf("Error: parenting of animation window not implemented!");
		return -1;
	}

	// Create UI background
	Rectangle	animesior_window_position = {0,SCREEN_HEIGHT - SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT/4};
	Rectangle animeditor_body_position = {SCREEN_WIDTH / 6,0,SCREEN_WIDTH * 4 / 6,SCREEN_HEIGHT/4};

	ui_element el = create_ui_element_raw_rect( // background
		animesior_window_position, DARKGRAY,
		ui_flag_null,
		layout_row,
		layout_null); 

	ui_element el1 = create_ui_element_raw_rect(
		animeditor_body_position, GRAY, 
		ui_flag_null,
		layout_flexibleY_in_parent, 
		layout_null); // frames background
	
	animeditor_window_parent = add_ui_element(uiDatas,el,-1);
	int animeditor_main_background = add_ui_element(uiDatas,el1,animeditor_window_parent);

	// Button to close and open the window
	ui_element close_animeditor_button =  create_ui_element(
		ui_rect_offset(20,20,20,20), 
		ui_empty_sprite, 
		GRAY, ui_flag_rawRect | ui_flag_button, layout_null, layout_null, 
		&CloseEditAnimationWindow
	);
	add_ui_element(uiDatas,close_animeditor_button,-1);

	// TODO: create scroll window with all animations


	// Construct parent for events and buttons
	ui_element bkg_parent = create_ui_element_raw_rect(
		animeditor_body_position, WHITE, 
		ui_flag_null, layout_flexibleX_in_parent, layout_null);

	bkg_parent.rect.height /= 4;
	bkg_parent.rect.width /= 2;
	int frame_events_parent = add_ui_element(uiDatas,bkg_parent,animeditor_main_background);  // parent for events
	//el1.rect.y += el1.rect.height * 2;
	int frame_sprites_parent = add_ui_element(uiDatas,bkg_parent,animeditor_main_background); // parent for sprites


	// Create button for sprites
	Rectangle button_rect = {0,0,48,48};
	//button_rect.y += 40;
	ui_element sprite_button = 
			create_ui_element_sprite(
				button_rect, ui_empty_sprite, 
				LIGHTGRAY,
				ui_flag_null,
				layout_null); // sprite button

	button_rect = (Rectangle){0,0,8,32};
	ui_element event_button_el = 
		create_ui_element_button(button_rect, ui_empty_sprite, LIGHTGRAY,
							ui_flag_rawRect, layout_null,
							AddEventToEditingAnimation,ui_null_text); // ui Button
	
	ui_element choose_event_button_el = 
		create_ui_element_button(ui_rect(50,60), ui_empty_sprite, LIGHTGRAY,
							ui_flag_rawRect, layout_null,
							SwitchEditingAnimationFlag, ui_null_text);
	

	for(int i = 0; i < editing_animation.frame_count; i++)
	{
		int event_button = add_ui_element(uiDatas,event_button_el,frame_events_parent);
		//printf("id: %d flags: %d n: %d \n",event_button,uiDatas.data[event_button].flags, uiDatas->data[event_button].number_in_children );
		sprite_button.sprite = editing_animation.sprites[i];
		int sprite_button_id = add_ui_element(uiDatas,sprite_button,frame_sprites_parent);
	}
	add_ui_element(uiDatas,choose_event_button_el,animeditor_window_parent);

	ui_element label = create_ui_element_label((Rectangle){0,0,50,50}, LIGHTGRAY,  layout_null, &chosen_animation_flag_string);

	chosen_aim_flag_string_label = add_ui_element(uiDatas, label, animeditor_window_parent);

	*editing_frame_count = editing_animation.frame_count;

	return animeditor_window_parent;
}

void saveEditedAnimation()
{

}
int ReInitEditAnimationWindowWithNewAnimation(FrameAnimation editing_animation, 
							int* editing_frame_count, int existingWindowParent, ui_element_datas* uiDatas)
{
	printf("Start reinitting frames: %d\n", editing_animation.frame_count);
	if(existingWindowParent < 0) return -1;
	ui_element* data = uiDatas->data;
	int parentID =  existingWindowParent + 2; // ADD NUMBER OF UI ELEMENTS HERE
	if(editing_animation.frame_count < 1 || data[parentID].childrenCount < 1)
	{
		printf("!(editing_animation.frame_count < 1 || data[parentID].childrenCount). create window again\n");
		return -1;
	}

	for(int i = 0; i < editing_animation.frame_count; i++)
	{
		int buttonID = -1;
		// TODO: remove out of loop; check lengths
		if(data[parentID].childrenCount <= i)
		{
			printf("Not enough ui elements (bt), resizing\n");
			int copy_id = data[parentID].childrenID[0];
			printf("copy id: %d\n", copy_id);
			ui_element copy = data[copy_id];
			buttonID = add_ui_element(uiDatas, copy, parentID);
			printf("resized\n");
		}else buttonID = data[parentID].childrenID[i];
		/*if(data[parentID + 1].childrenCount <= i)
		{
			printf("Not enough ui elements (sprites), resizing. childrenCount: %d capacity: %d \n", data[parentID + 1].childrenCount, data[parentID + 1].childrenCapacity);
			int copy_id = data[parentID + 1].childrenID[0];
			ui_element copy = data[copy_id];
			copy.sprite = editing_animation.sprites[i];
			//int sprite_button = add_ui_element(uiDatas, copy, parentID + 1);
			printf("resized\n");
		}*/
		printf("reassigned: id: %d flags: %d n: %d \n",buttonID,data[buttonID].flags, data[buttonID].number_in_children );
	}

	*editing_frame_count = editing_animation.frame_count;

	return existingWindowParent;
}
