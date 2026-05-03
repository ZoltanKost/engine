#include "animeditor.h"

static FrameAnimation editingAnimation = {0};
static char current_event_animation_flag = 1 << 6;
void AddFrameToEditingAnimation(int paramsCount)
{
	// TODO: allocations
	return;
	printf("Adding frame...\n");
	editingAnimation.frames = MemRealloc(editingAnimation.frames, editingAnimation.frame_count++);
}

void AddEventToEditingAnimation(int frameNumber)
{
    printf("frames:%d number:%d", (int)editingAnimation.frames, frameNumber);
	editingAnimation.frames[frameNumber].event ^= current_event_animation_flag;
	printf("for frame %d added event %d \n",frameNumber,current_event_animation_flag);
}


int InitEditAnimationWindow(FrameAnimation _editingAnimation, 
							int* editing_frame_count, int parentID, ui_element_datas* uiDatas)
{
    editingAnimation = _editingAnimation;
	// TODO: implement parenting
	if(parentID != 0) return -1;
	// Create UI background
	Rectangle element = {0,SCREEN_HEIGHT - SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT/4};
	Rectangle element1 = {0,0,SCREEN_WIDTH * 4 / 6,SCREEN_HEIGHT/4};
	ui_element el = create_ui_element(element,(Sprite){0},DARKGRAY,0,ui_flag_rawRect, 2,0,0);
	ui_element el1 = create_ui_element(element1,(Sprite){0},GRAY,1,ui_flag_rawRect, layout_flexibleY_in_parent, layout_relative_pos,0);
	int ui = add_ui_element(uiDatas,el,-1);
	int ui1 = add_ui_element(uiDatas,el1,ui);

	// Construct parent rect1

	ui_element el2 = create_ui_element(element1,(Sprite){0},WHITE,2,ui_flag_rawRect, layout_flexibleX_in_parent, layout_relative_pos,0);
	el2.rect.height /= 4;
	el2.rect.width /= 2;
	int ui_top = add_ui_element(uiDatas,el2,ui1);
	//el1.rect.y += el1.rect.height * 2;
	int ui_mid = add_ui_element(uiDatas,el2,ui1);


	// Create button for sprites
	Rectangle buttonRect = {0,0,48,48};
	//buttonRect.y += 40;
	ui_element sprite_button = 
			create_ui_element(buttonRect, (Sprite){0},LIGHTGRAY,4,ui_flag_button,0,1,0);

	buttonRect = (Rectangle){0,0,8,32};
	ui_element buttonEl = create_ui_element(buttonRect,(Sprite){0},LIGHTGRAY,4,ui_flag_rawRect | ui_flag_button,0,1,AddEventToEditingAnimation);
	for(int i = 0; i < editingAnimation.frame_count; i++)
	{
		int buttonE1 = add_ui_element(uiDatas,buttonEl,ui_top);
		//printf("id: %d flags: %d n: %d \n",buttonE1,uiDatas.data[buttonE1].flags, uiDatas->data[buttonE1].number_in_children );
		sprite_button.sprite = editingAnimation.sprites[i];
		int button_sprite = add_ui_element(uiDatas,sprite_button,ui_mid);
	}

	*editing_frame_count = editingAnimation.frame_count;

	return ui;
}

int ReInitEditAnimationWindowWithNewAnimation(FrameAnimation editingAnimation, 
							int* editing_frame_count, int existingWindowParent, ui_element_datas* uiDatas)
{
	printf("Start reinitting frames: %d\n", editingAnimation.frame_count);
	if(existingWindowParent < 0) return -1;
	ui_element* data = uiDatas->data;
	int parentID =  existingWindowParent + 2; // ADD NUMBER OF UI ELEMENTS HERE
	if(editingAnimation.frame_count < 1 || data[parentID].childrenCount < 1)
	{
		printf("!(editingAnimation.frame_count < 1 || data[parentID].childrenCount). create window again\n");
		return -1;
	}

	for(int i = 0; i < editingAnimation.frame_count; i++)
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
			copy.sprite = editingAnimation.sprites[i];
			//int button_sprite = add_ui_element(uiDatas, copy, parentID + 1);
			printf("resized\n");
		}*/
		printf("reassigned: id: %d flags: %d n: %d \n",buttonID,data[buttonID].flags, data[buttonID].number_in_children );
	}

	*editing_frame_count = editingAnimation.frame_count;

	return existingWindowParent;
}

void SwitchEditingAnimationFlag()
{
    do
    {
        if(current_event_animation_flag & ship_flag_remove)
        {
            current_event_animation_flag = 4;
            break;
        }
        current_event_animation_flag <<= 1;
    }while(!(current_event_animation_flag & ship_flag_reset 
        || current_event_animation_flag & ship_flag_shoot
        || current_event_animation_flag & ship_flag_remove));
    printf("flag: %d\n", current_event_animation_flag);
}