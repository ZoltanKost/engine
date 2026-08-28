#include "ui.h"

static int standardFontSize = 16;
ui_element_datas init_ui(int initCount)
{
	ui_element_datas result = {
		.data = MemAlloc(sizeof(ui_element) * initCount),
		.count = initCount
	};
	return result;
}
/*
	if an element is dirty => his children are dirty and his siblings should be dirty 
	alghorithm: 
	1. mark self as dirty
	2. mark all the children as dirty
	3. mark siblings as dirty
*/
void mark_as_dirty(ui_element_datas* datas, int id)
{
	ui_element* data = datas->data;
	ui_element dirty_element = data[id];
	if(dirty_element.flags & ui_flag_dirty) return; // already dirty => children are also dirty 

	dirty_element.flags |= ui_flag_dirty;
	data[id] = dirty_element;

	int dirty_parent_id = data[id].parentID;
	ui_element dirty_parent = data[dirty_parent_id];

	if(dirty_element.childrenCount > 0)				 // do not  need mark siblings individually, because they'll be marked later in that call
	mark_as_dirty(datas,dirty_element.childrenID[0]); // todo: reqursive calls, performance

	// mark siblings as dirty
	for(int i = 0; i < dirty_parent.childrenCount; i++)
	{
		int sibling_id = dirty_parent.childrenID[i];
		
		if(i != id) // don't call mark_as_dirty if not necessary 
		{
			mark_as_dirty(datas,sibling_id); // todo: reqursive calls, performance
		}
	}
}
int add_ui_element(ui_element_datas* data, ui_element element, int parentID)
{
	// First, write element down to the array
	ui_element* ui_elements = data->data;
	if(data->firstInactive >= data->count) {
		ui_elements = MemRealloc(ui_elements,data->count * 2 * sizeof(ui_element));
		data->data = ui_elements;
		//printf("\n array reallocated  ");
		data->count = data->count * 2;
	}
	int result_id = data->firstInactive++;
	element.id = result_id;
	ui_elements[result_id] = element;
	printf("added el %d, flags: %d parent: %d \n", result_id, element.flags, parentID);

	// Then, assigned element from the array to the parents children
	// todo: refactor so it doesn't look fucking ugly and unreadable
	// todo: mark elements as dirty
	if(parentID > -1)
	{
		ui_element parent = ui_elements[parentID];
		if(parent.childrenCount < parent.childrenCapacity) // size is fine, just add
		{
			parent.childrenID[parent.childrenCount] = result_id;
			ui_elements[parentID].childrenCount++;
		}else{ // resize and add
			printf("reallocating: %d %d %d \n", parentID, ui_elements[parentID].childrenCount, ui_elements[parentID].childrenCapacity);
			ui_elements[parentID].childrenID = 
				MemRealloc(ui_elements[parentID].childrenID, 
					parent.childrenCapacity*2*(sizeof(int)));
			printf("reallocating\n");
			ui_elements[parentID].childrenCapacity*=2;
			ui_elements[parentID].childrenID[ui_elements[parentID].childrenCount++] = result_id;
		}
		
		printf("child assigned\n");
		ui_elements[result_id].parentID = parentID;
		ui_elements[result_id].layout_flags |= parent.children_layout;
		// TODO: should update ALL numbers in ALL children.  ??
		ui_elements[result_id].number_in_children = ui_elements[parentID].childrenCount - 1;

		mark_as_dirty(data, result_id);
	}
	printf("\nfirst: %d count: %d \n",data->firstInactive, data->count);
	return result_id;
}

ui_element create_ui_element_raw_rect(Rectangle rect, Color color, char flags, char children_flags, char layout_flags)
{
	//TODO: init children array
	return create_ui_element(rect, (Sprite) {0}, color, flags | ui_flag_rawRect, children_flags, layout_flags, 0);
}

ui_element create_ui_element_label(Rectangle rect, Color color, char layout_flags, char * text)
{
	ui_element result = create_ui_element(
		rect, ui_empty_sprite, 
		color, ui_flag_rawRect | ui_flag_text, 
		layout_null, layout_flags, ui_null_callback);
	result.text = text;
	return result;
}
ui_element create_ui_element_button(Rectangle rect, Sprite sprite, Color color, char flags, char layout_flags, void (*callback)(int), char* text)
{
	ui_element result = create_ui_element(rect, sprite, color, flags | ui_flag_button, layout_null, layout_flags, callback);
	result.text = text;
	return result;
}


ui_element create_ui_element_sprite(Rectangle rect, Sprite sprite, Color color, char flags, char layout_flags)
{
	return create_ui_element(
		rect, sprite, color, flags & ~ui_flag_rawRect, layout_null, layout_flags, ui_null_callback);

}

ui_element create_ui_element(Rectangle rect, Sprite sprite, Color color, char flags, char children_layout_flags, char layout_flags, void (*callback)(int))
{
	//TODO: init children array
	if((sprite.texture.id > 0) && (flags & ui_flag_rawRect)) 
	{
		printf("ERROR: shadowing a sprite with a ui_flag_rawRect!\n");
	}
	ui_element result = {0};
	result.rect = rect;
	result.color = color;
	result.flags = flags | 1;
	result.children_layout = children_layout_flags;
	result.layout_flags = layout_flags;
	result.parentID = -1;
	result.childrenCapacity = 8;
	result.childrenID = MemAlloc(8 * sizeof(int*));
	result.callback = callback;
	if(!flags & ui_flag_rawRect)
	{
		result.sprite = sprite;
	}
	return result;
}
// TODO: why keep and update two rects?
// todo: optimisation: split draw_rects and rects
void calculate_ui_positions(ui_element_datas* UIdata, Camera2D cam)
{
	int firstInactive = UIdata->firstInactive;
	ui_element* data = UIdata->data;
	for(int i = 0; i < firstInactive; i++)
	{
		
		ui_element element = data[i];
		
		if(!element.flags & ui_flag_active) continue;

		char dirty = element.flags & ui_flag_dirty;

		if(dirty) printf("Dirty element! %d, %f, %f\n", element.id, element.rect.x, element.rect.y);
		
		Rectangle draw_rect = {
			cam.target.x - cam.offset.x + data[i].rect.x,
			cam.target.y - cam.offset.y + data[i].rect.y,
			data[i].rect.width,
			data[i].rect.height
		};

		if(element.parentID >= 0)
		{
			ui_element parent = data[element.parentID];
			if(dirty) {
				data[i].rect.x = parent.rect.x;
				data[i].rect.y = parent.rect.y;
			}
			draw_rect.x = parent.draw_rect.x;
			draw_rect.y = parent.draw_rect.y;
			unsigned char layout_flags = (unsigned char)data[i].layout_flags;
			switch(layout_flags)
			{
				case layout_flexibleX_in_parent:
					draw_rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								(element.number_in_children + 1)) - element.rect.width * 0.5f;
					if(dirty) data[i].rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								(element.number_in_children + 1)) - element.rect.width * 0.5f;
				break;
				case layout_flexibleY_in_parent:
					draw_rect.y += (parent.rect.height / (parent.childrenCount + 1) *
								(element.number_in_children + 1)) - element.rect.height * 0.5f;
					if(dirty) data[i].rect.y += (parent.rect.height / (parent.childrenCount + 1) *
								(element.number_in_children + 1)) - element.rect.height * 0.5f;
				break;
				case layout_rowX:
				/*
					draw_rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.width * 0.5f;
					if(dirty) data[i].rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.width * 0.5f;
					break;*/
					for(int k = 0; k < element.number_in_children; k++)
					{
						int child_id = parent.childrenID[k];
						draw_rect.x += data[child_id].rect.width;
						if(dirty) data[i].rect.x += data[child_id].rect.width;
					}
				break;
				case layout_rowY:
				/*
					draw_rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.width * 0.5f;
					if(dirty) data[i].rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.width * 0.5f;
					break;*/
					for(int k = 0; k < element.number_in_children; k++)
					{
						int child_id = parent.childrenID[k];
						draw_rect.y += data[child_id].rect.height;
						if(dirty) data[i].rect.y += data[child_id].rect.height;
					}
				break;
			}
		}
		if(dirty) printf("dirty updated: %f, %f \n", data[i].rect.x, data[i].rect.y);
		data[i].flags &= ~ui_flag_dirty;
		data[i].draw_rect = draw_rect;
	}
}
// todo: draw_rect as separate rect array
void draw_ui(ui_element_datas* UIdata, Camera2D cam)
{
	int firstInactive = UIdata->firstInactive;
	ui_element* data = UIdata->data;
	for(int i = 0; i < firstInactive; i++){
		if(!(data[i].flags & ui_flag_active)) continue;
		ui_element el = data[i];
		if(el.flags & ui_flag_rawRect)
		{
			DrawRectangleRec(el.draw_rect, el.color);
		}else
		{
			Sprite sprite = el.sprite;
			Vector2 pivot = {0,
							0};
			DrawRectangleRec(el.draw_rect, el.color);
			DrawTexturePro(sprite.texture, sprite.rect, 
						   el.draw_rect, pivot, 0, WHITE);
		}
		if(el.flags & ui_flag_text) 
		{
			DrawText(el.text, el.draw_rect.x, el.draw_rect.y, standardFontSize, BLACK);
		}
	}
}

ui_cast_result cast_over_ui(ui_element_datas datas, Vector2 screenInput)
{
	int firstInactive = datas.firstInactive;
	ui_element* data = datas.data;
	ui_cast_result result = {-1,0};
	int resultPriority = -1;
	// todo: add camera position to the screenInput to not recalculate draw_rect
	//printf("count: %d \n", firstInactive);
	for(int i = 0; i < firstInactive; i++){
		if((data[i].flags & ui_flag_active) && point_over_rect(screenInput, data[i].rect))
		{
			int addition = 0;
			if(data[i].flags & ui_flag_button) addition += 50; // TODO: implement ui layers
			int currentPriority = data[i].layer + addition;
			printf("id: %d priority: %d\n", i,currentPriority);
			if(currentPriority > resultPriority) 
			{
				result.id = i;
				result.flags = data[i].flags;
				resultPriority = currentPriority;
			}
		}
	}
	printf("\nid %d flags %d: %d\n", result.id, result.flags,resultPriority);
	return result;
}


void remove_inactive_elements(ui_element_datas* ui_elementData)
{
	ui_element* ui_elements = ui_elementData->data;
	for(int i = 0; i < ui_elementData->firstInactive; i++)
	{
		if(!(ui_elements[i].flags & ui_flag_active))
		{
			remove_ui_element(ui_elementData,i);
			i--;
			continue;
		}
	}
}

void process_ui(ui_element_datas* data, float dt, Camera2D cam)
{
	/*for(int i = 0; i < data->firstInactive; i++)
	{
		data->data[i].flags |= ui_flag_dirty; // todo: remove
	}*/
	calculate_ui_positions(data,cam);
	draw_ui(data,cam);
}
// TODO: childrenID are not changing
/*
	removing: 1. get all the children
			  2. set inactive
			  3. repeat fo all the children
*/
void remove_ui_element(ui_element_datas* ui_elements, int id)
{
	printf("ERROR: not implemented\n");
	return;
	ui_element* data = ui_elements->data;
	int firstInactive = ui_elements->firstInactive;
	ui_element removeElement = data[id];
	int * removehildren = removeElement.childrenID;
	int childrenCount = removeElement.childrenCount;
	data[id].flags ^= ui_flag_active;
}
// 
void deactivate_ui_element(ui_element_datas* ui_elements, int id)
{
	ui_element* data = ui_elements->data;
	ui_element removeElement = data[id];
	if(!(removeElement.flags & ui_flag_active))
	{
		printf("ui element already inactive: %d\n", id);
		return;
	}
	int * removeChildren = removeElement.childrenID;
	int childrenCount = removeElement.childrenCount;
	data[id].flags ^= ui_flag_active;
	for(int i = 0; i < childrenCount; i++)
	{
		printf("child: %d\n", removeChildren[i]);
		deactivate_ui_element(ui_elements, removeChildren[i]);
	}
	printf("deactivated ui element: %d\n", id);
}
void activate_ui_element(ui_element_datas* ui_elements, int id)
{
	ui_element* data = ui_elements->data;
	ui_element activateElement = data[id];
	if(activateElement.flags & ui_flag_active)
	{
		printf("ui element already active: %d\n", id);
		return;
	}
	int * activateChildren = activateElement.childrenID;
	int childrenCount = activateElement.childrenCount;
	data[id].flags ^= ui_flag_active;
	for(int i = 0; i < childrenCount; i++)
	{
		printf("child: %d\n", activateChildren[i]);
		activate_ui_element(ui_elements, activateChildren[i]);
	}
	printf("activated ui element: %d\n", id);
}
// TODO: write the "gaps" in array into the sorted binary tree.
// then, when creaeting new element pick the lowest id for the element.
// when removing elements, just write their ids to the tree and make inactive
int remove_children(ui_element_datas* ui_elements, int id)
{
	printf("ERROR: not implemented\n");
	return -1;
	ui_element* data = ui_elements->data;
	int firstInactive = ui_elements->firstInactive;
	ui_element parent = data[id];
	int lastRemoved = -1;
	for(int i = 0; i < parent.childrenCount; i++)
	{	
		int res = remove_children_rm_self(ui_elements, parent.childrenID[i]);
		if(res != -1 && res > lastRemoved) lastRemoved = res;
	}
	parent.childrenCount = 0;
	// shift elements from lastRemoved + 1 to parent.id + 1;
	memmove(&data[parent.id + 1], &data[lastRemoved + 1], firstInactive - (lastRemoved + 1));
	data[parent.id] = parent;
	return -1;
}
int remove_children_rm_self(ui_element_datas* ui_elements, int id)
{
	printf("ERROR: not implemented");
	return -1;
	ui_element* data = ui_elements->data;
	int firstInactive = ui_elements->firstInactive;
	ui_element parent = data[id];
	int lastRemoved = -1;
	for(int i = 0; i < parent.childrenCount; i++)
	{	
		int res = remove_children(ui_elements, parent.childrenID[i]);
		if(res != -1 && res > lastRemoved) lastRemoved = res;
	}

	remove_ui_element_no_replace(ui_elements, parent.id);
	
	return lastRemoved;
}

void remove_ui_element_no_replace(ui_element_datas* ui_elements, int id)
{
	ui_element* data = ui_elements->data;
	data[id] = (ui_element){0};
}

bool point_over_rect(Vector2 p, Rectangle rect)
{
	return p.x >= rect.x && p.x < rect.x + rect.width && 
			p.y >= rect.y && p.y < rect.y + rect.height;
}