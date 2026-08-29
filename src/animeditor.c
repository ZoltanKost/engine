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
static ui_element_datas* ui_data_array = 0; // array of ui_elements, to be able to open/close window
static char animeditor_active = 1;

static int chosen_events_window_id = -1;

static char* chosen_animation_flag_string = "test";
static char* switch_animation_flag_string= "switch";
static int events_fields_id = -1;
static int frame_events_parent = -1;
static int frame_sprites_parent = -1;

static int chosen_anim_flag_string_label = 0; // id of label with chosen anim flag


static int current_editing_frame = 0;	// frame, chosen for editing by clicking on the editing button
static int current_event_index = 0;

static int entity_editing = 0;
static int animation_editing_index = 1;
static int entity_anim_event_count = 0;	// how much events are there in total for the entity being editing
static int* entitiy_anim_events;// readonly events of current entity being editing
static char** entity_anim_events_strings; // readonly events of current entity being editing
void AddFrameToEditingAnimation(int paramsCount)
{
	// TODO: allocations
	return;
	printf("Adding frame...\n");
	editing_animation.frames = MemRealloc(editing_animation.frames, editing_animation.frame_count++);
}

void AddEventToEditingAnimation(int eventNumber)
{
    //printf("frames:%d number:%d", (int)editing_animation.frames, frameNumber);
	editing_animation.frames[current_editing_frame].event ^= entitiy_anim_events[eventNumber];
	update_frame_attached_events();
	printf("for frame %d added event %d \n",current_editing_frame,entitiy_anim_events[eventNumber]);
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

	entitiy_anim_events = ship_events; // todo: change dependent on which entity is being editing
	entity_anim_events_strings = ship_event_strings;
	entity_anim_event_count = ship_event_count;
	animation_editing_index = 1; // todo: just for now, has actually be decided by caller
	current_editing_frame = 0;
	// TODO: implement parenting
	if(parentID != 0) 
	{
		printf("Error: parenting of animation window not implemented!");
		return -1;
	}
	Rectangle	animesior_window_position = {0,SCREEN_HEIGHT - SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT/4};
	Rectangle animeditor_body_position = {SCREEN_WIDTH / 6,0,SCREEN_WIDTH * 3 / 6,SCREEN_HEIGHT/4};
	
	int animeditor_main_background;
	// background and button to open/ close
{
	// Create UI background
	

	ui_element el = create_ui_element_raw_rect( // background
		animesior_window_position, DARKGRAY,
		ui_flag_null,
		layout_rowX,
		layout_null); 

	ui_element el1 = create_ui_element_raw_rect(
		animeditor_body_position, GRAY, 
		ui_flag_null,
		layout_flexibleY_in_parent, 
		layout_null); // frames background
	
	animeditor_window_parent = add_ui_element(uiDatas,el,-1);
	animeditor_main_background = add_ui_element(uiDatas,el1,animeditor_window_parent);

	// Button to close and open the window
	ui_element close_animeditor_button =  create_ui_element(
		ui_rect_offset(20,20,20,20), 
		ui_empty_sprite, 
		GRAY, ui_flag_rawRect | ui_flag_button, layout_null, layout_null, 
		&CloseEditAnimationWindow
	);
	add_ui_element(uiDatas,close_animeditor_button,-1);

	// TODO: create scroll window with all animations
}
	// Construct parent for events and buttons
{
	ui_element bkg_parent = create_ui_element_raw_rect(
		animeditor_body_position, WHITE, 
		ui_flag_null, layout_flexibleX_in_parent, layout_null);

	bkg_parent.rect.height /= 4;
	//bkg_parent.rect.width /= 2;
	frame_events_parent = add_ui_element(uiDatas,bkg_parent,animeditor_main_background);  // parent for events
	frame_sprites_parent = add_ui_element(uiDatas,bkg_parent,animeditor_main_background); // parent for sprites
}
	
	init_event_buttons_and_sprites(editing_animation);

	int anim_list_id;
	int events_field_id;
	ui_element anim_list;
	//create another block for ui event data editing
{
	int element_width =SCREEN_WIDTH*1/6;
	ui_element events_field_parent = create_ui_element_raw_rect( // events_parent
		ui_rect(SCREEN_WIDTH*3/6,SCREEN_HEIGHT/4), 
		GREEN, ui_flag_null,
		children_layout_rowX,
		ui_flag_null);
	
	events_field_id = add_ui_element(uiDatas,events_field_parent,animeditor_window_parent);

	ui_element events_fields = create_ui_element_raw_rect( // event names parent
		ui_rect(element_width,SCREEN_HEIGHT/4), 
		YELLOW, ui_flag_null, 
		children_layout_rowY, 
		ui_flag_null);

	anim_list = create_ui_element_raw_rect( // event names parent
		ui_rect(element_width,SCREEN_HEIGHT/4), 
		PINK, ui_flag_null, 
		children_layout_rowY, 
		ui_flag_null);

	
	events_fields_id = add_ui_element(uiDatas,events_fields,events_field_id); // events parent
	anim_list_id = add_ui_element(uiDatas,anim_list,events_field_id); // 
}
	init_frame_attached_events();

	//add_ui_element(uiDatas,event_button,events_fields_id); 
	/*ui_element choose_event_button_el = 
		create_ui_element_button(ui_rect(50,60), ui_empty_sprite, LIGHTGRAY,
							ui_flag_rawRect | ui_flag_text, layout_null,
							change_frame_attached_events, switch_animation_flag_string);
	add_ui_element(uiDatas,choose_event_button_el,anim_list_id); // todo: change to flag which event is enabled?
	
	ui_element label = create_ui_element_label((Rectangle){0,0,50,50}, LIGHTGRAY,  layout_null, chosen_animation_flag_string); 

	chosen_anim_flag_string_label = add_ui_element(uiDatas, label, anim_list_id); // label with textwhich event is chosen 
	*/
	// list of animations
	ui_element animation_button  = create_ui_element_button(
		ui_rect(SCREEN_WIDTH*1/6,30),
		ui_empty_sprite, LIGHTGRAY, 
		ui_flag_text, layout_null, 
		open_animation_to_edit, "");
	FrameAnimation* animations = get_animation_array_of(entity_editing);
	for(int i = 0; i < 3; i++) // todo: ignore idle animation
	{
		animation_button.text = ship_animation_names[i];
		add_ui_element(ui_data_array, animation_button, anim_list_id);
	}
	/*// list of entities
	int entity_list_id = add_ui_element(ui_data_array, anim_list, events_field_id);
	ui_element entity_button  = create_ui_element_button(
		ui_rect(SCREEN_WIDTH*1/6,30),
		ui_empty_sprite, LIGHTGRAY, 
		ui_flag_text, layout_null, 
		change_entity_editing, "");
	
	char** existing_entities = get_entities_strings();
	for(int i = 0; i < EXISTING_ENTITIES_COUNT; i++) // todo: ignore idle animation
	{
		entity_button.text = existing_entities[i];
		add_ui_element(ui_data_array, entity_button, entity_list_id);
	}*/
	// todo add animation disk save functionality

	*editing_frame_count = editing_animation.frame_count;

	return animeditor_window_parent;
}
// todo: assumes there is always 6 animations
void update_event_buttons_and_sprites(FrameAnimation editing_animation)
{
	
	ui_element parent = ui_data_array->data[frame_events_parent];
	int i = 0;
	printf("init already existing buttons\n");
	for(; i < editing_animation.frame_count && i < parent.childrenCount; i++) // init already existing buttons
	{
		printf("%d already exist\n", i);
		// todo: color dependent on event presence
		int index = parent.childrenID[i];
		activate_ui_element(ui_data_array, index);

		int sprite_index = ui_data_array->data[frame_sprites_parent].childrenID[i];
		ui_data_array->data[sprite_index].sprite = editing_animation.sprites[i];
		activate_ui_element(ui_data_array, sprite_index);
	}
	printf("not exceeds childrenCount => more ui frames active, deactivate ui frames!\n");
	for(; i < parent.childrenCount; i++) // if not exceeds childrenCount => more ui frames active, deactivate ui frames!
	{
		printf("%d is too much\n", i);
		// todo: color dependent on event presence
		int index = parent.childrenID[i];
		deactivate_ui_element(ui_data_array, index);

		int sprite_index = ui_data_array->data[frame_sprites_parent].childrenID[i];
		deactivate_ui_element(ui_data_array, sprite_index);
	}
	
	printf("not exceeds frameCount => need more ui frames, instantiate!\n");
	for(; i < editing_animation.frame_count; i++) // if not exceeds frameCount => need more ui frames, instantiate!
	{
		printf("%d has not enough frames!\n", i);

		ui_element sprite_button;
		ui_element event_button_el;

		Rectangle button_rect = {0,0,48,48};
		//button_rect.y += 40;
		sprite_button = 
			create_ui_element_button(
				button_rect, ui_empty_sprite, 
				LIGHTGRAY,
				ui_flag_null,
				layout_null,choose_frame_to_edit,ui_null_text); // sprite button

		button_rect = (Rectangle){0,0,8,32};
		event_button_el = 
		create_ui_element_button(button_rect, ui_empty_sprite, LIGHTGRAY,
							ui_flag_rawRect, layout_null,
							choose_frame_to_edit, ui_null_text); // ui Button // todo: change from adding event directly to opening add event window
	
		// todo: color dependent on event presence
		int event_button = add_ui_element(ui_data_array,event_button_el,frame_events_parent);
		//printf("id: %d flags: %d n: %d \n",event_button,uiDatas.data[event_button].flags, uiDatas->data[event_button].number_in_children );
		sprite_button.sprite = editing_animation.sprites[i];
		int sprite_button_id = add_ui_element(ui_data_array,sprite_button,frame_sprites_parent);
	}
	
}


void init_event_buttons_and_sprites(FrameAnimation editing_animation)
{
	ui_element sprite_button;
	ui_element event_button_el;

	Rectangle button_rect = {0,0,64,64};
	//button_rect.y += 40;
	sprite_button = 
			create_ui_element_button(
				button_rect, ui_empty_sprite, 
				LIGHTGRAY,
				ui_flag_null,
				layout_null,choose_frame_to_edit,ui_null_text); // sprite button

	button_rect = (Rectangle){0,0,8,32};
	event_button_el = 
		create_ui_element_button(button_rect, ui_empty_sprite, LIGHTGRAY,
							ui_flag_rawRect, layout_null,
							choose_frame_to_edit, ui_null_text); // ui Button // todo: change from adding event directly to opening add event window
	
							// for each frame, create an instance of buttons
	for(int i = 0; i < editing_animation.frame_count; i++)
	{
		// todo: color dependent on event presence
		int event_button = add_ui_element(ui_data_array,event_button_el,frame_events_parent);
		//printf("id: %d flags: %d n: %d \n",event_button,uiDatas.data[event_button].flags, uiDatas->data[event_button].number_in_children );
		sprite_button.sprite = editing_animation.sprites[i];
		int sprite_button_id = add_ui_element(ui_data_array,sprite_button,frame_sprites_parent);

		printf("event: %d to %d sprite %d to %d \n", event_button, sprite_button_id,frame_events_parent,frame_sprites_parent);
	}
}

void open_animation_to_edit(int id)
{
	const char* name = ship_animation_names[animation_editing_index];
	char path[30];
	//sprintf(path, "%s",get_ship_type(editing entity id)); // todo: change animation based on which entity type
	WriteFrameAnimationToFile(editing_animation, name);

	animation_editing_index = id;
	FrameAnimation* anims = get_animation_array_of(entity_editing);
	editing_animation = anims[id];
	reinit_animation_editor(editing_animation);
}
// todo: currently editing animations directly, change to  editing_animation
void init_frame_attached_events()
{
	// todo: generalize
	ui_element event_button  = create_ui_element_button(
		ui_rect(SCREEN_WIDTH*1/6,30),
		ui_empty_sprite, RED, 
		ui_flag_text, layout_null, 
		AddEventToEditingAnimation, "");

	for(int i = 0; i < entity_anim_event_count; i++)
	{
		event_button.text = entity_anim_events_strings[i];
		char event_active = entitiy_anim_events[i] & editing_animation.frames[current_editing_frame].event;
		event_button.color = event_active? GREEN : RED;
		add_ui_element(ui_data_array,event_button,events_fields_id); 
	}
}

void update_frame_attached_events()
{
	for(int i = 0; i < entity_anim_event_count; i++)
	{
		ui_element parent = ui_data_array->data[events_fields_id];

		int update_id = parent.childrenID[i];
		ui_element elemnt_to_update = ui_data_array->data[update_id];

		elemnt_to_update.text = entity_anim_events_strings[i];
		
		char event_active = entitiy_anim_events[i] & editing_animation.frames[current_editing_frame].event;
		elemnt_to_update.color = event_active? GREEN : RED;

		ui_data_array->data[update_id] = elemnt_to_update;
	}
}

void choose_frame_to_edit(int id)
{
	int old_id = ui_data_array->data[frame_events_parent].childrenID[current_editing_frame]; // todo this one is somehow choosing the wrong button

	ui_data_array->data[old_id].color = LIGHTGRAY; // deselect old selection

	current_editing_frame = id;

	int new_id = ui_data_array->data[frame_events_parent].childrenID[current_editing_frame];

	ui_data_array->data[new_id].color = GREEN;	// select new frame to edit

	update_frame_attached_events();
	printf("Editing frame: %d\n", id);
}

void change_frame_attached_events(int id)
{
	current_event_index++;

	if (current_event_index >= entity_anim_event_count)
	{
		current_event_index = 0;
	}

	current_event_animation_flag = entitiy_anim_events[current_event_index];
	chosen_animation_flag_string = entity_anim_events_strings[current_event_index];

	printf("event changed: %s, count: %d, current: %d\n",chosen_animation_flag_string, entity_anim_event_count, current_event_index); // TODO here set the image of animation?
    printf("flag: %d\n", current_event_animation_flag);
}

void reinit_animation_editor(FrameAnimation editing_animation)
{
	/*
	1. init events and sprites
	2. init events
	*/
	update_event_buttons_and_sprites(editing_animation);
	update_frame_attached_events();
}
