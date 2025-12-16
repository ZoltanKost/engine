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

char layout_relative_pos = 1;
char layout_flexibleX_in_parent = 2; 
char layout_flexibleY_in_parent = 4;

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

void remove_ui_element(ui_element_datas* ui_elements, int id);
bool point_over_rect(Vector2 p, Rectangle rect);
void remove_ui_element_no_replace(ui_element_datas* ui_elements, int id);

static unsigned char ui_flag_active = 1;
static unsigned char ui_flag_rawRect = 1<<1;
static unsigned char ui_flag_button = 1<<7;
static Color uiColor = {0x36,0x36,0x36,0xFF};

ui_element_datas init_ui(int initCount)
{
	ui_element_datas result = {
		.data = MemAlloc(sizeof(ui_element) * initCount),
		.count = initCount
	};
	return result;
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
	if(parentID > -1)
	{
		ui_element parent = ui_elements[parentID];
		if(parent.childrenCount < parent.childrenCapacity)
		{
			parent.childrenID[parent.childrenCount] = result_id;
			ui_elements[parentID].childrenCount++;
		}else{
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
		// TODO: should update ALL numbers in ALL children.  
		ui_elements[result_id].number_in_children = ui_elements[parentID].childrenCount;
		//printf("added child %d %d \n", ui_elements[result_id].layout_flags, ui_elements[result_id].number_in_children);
	}
	printf("\nfirst: %d count: %d \n",data->firstInactive, data->count);
	return result_id;
}

ui_element create_ui_element(Rectangle rect, Sprite sprite, Color color, int layer, char flags, char children_flags, char layout_flags, void (*callback)(int))
{
	//TODO: init children array
	ui_element result = {0};
	result.rect = rect;
	result.color = color;
	result.flags = flags | 1;
	result.children_layout = children_flags;
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

void calculate_ui_positions(ui_element_datas* UIdata, Camera2D cam)
{
	int firstInactive = UIdata->firstInactive;
	ui_element* data = UIdata->data;
	for(int i = 0; i < firstInactive; i++)
	{
		ui_element element = data[i];

		Rectangle draw_rect = {
			cam.target.x - cam.offset.x + data[i].rect.x,
			cam.target.y - cam.offset.y + data[i].rect.y,
			data[i].rect.width,
			data[i].rect.height
		};

		if(element.parentID >= 0 && element.layout_flags & layout_relative_pos)
		{
			ui_element parent = data[element.parentID];
			data[i].rect.x = parent.rect.x;
			data[i].rect.y = parent.rect.y;
			draw_rect.x = parent.draw_rect.x;
			draw_rect.y = parent.draw_rect.y;
			if(data[i].layout_flags & layout_flexibleX_in_parent)
			{
				draw_rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.width * 0.5f;
				data[i].rect.x += (parent.rect.width / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.width * 0.5f;
			}else if(data[i].layout_flags & layout_flexibleY_in_parent)
			{
				draw_rect.y += (parent.rect.height / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.height * 0.5f;
				data[i].rect.y += (parent.rect.height / (parent.childrenCount + 1) *
								element.number_in_children) - element.rect.height * 0.5f;
			}
		}
		data[i].draw_rect = draw_rect;
	}
}

void draw_ui(ui_element_datas* UIdata, Camera2D cam)
{
	int firstInactive = UIdata->firstInactive;
	ui_element* data = UIdata->data;
	for(int i = 0; i < firstInactive; i++){
		if(data[i].flags & ui_flag_rawRect)
		{
			DrawRectangleRec(data[i].draw_rect, data[i].color);
		}else
		{
			Sprite sprite = data[i].sprite;
			Vector2 pivot = {0,
							0};
			DrawRectangleRec(data[i].draw_rect, data[i].color);
			DrawTexturePro(sprite.texture, sprite.rect, 
						   data[i].draw_rect, pivot, 0, WHITE);
		}
	}
}

ui_cast_result cast_over_ui(ui_element_datas *datas, Vector2 screenInput)
{
	int firstInactive = datas->firstInactive;
	ui_element* data = datas->data;
	ui_cast_result result = {-1,0};
	int resultPriority = -1;
	//printf("count: %d \n", firstInactive);
	for(int i = 0; i < firstInactive; i++){
		if(data[i].flags & ui_flag_active && point_over_rect(screenInput, data[i].rect))
		{
			int addition = 0;
			if(data[i].flags & ui_flag_button) addition += 50;
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
	calculate_ui_positions(data,cam);
	draw_ui(data,cam);
}
// TODO: childrenID are not changing
void remove_ui_element(ui_element_datas* ui_elements, int id)
{
	printf("ERROR: not implemented");
	return;
	ui_element* data = ui_elements->data;
	int firstInactive = ui_elements->firstInactive;
	data[id] = data[--firstInactive];
	data[firstInactive].flags = 0;
	data[id].id = id;
	ui_elements->firstInactive = firstInactive;
}
// TODO: write the "gaps" in array into the sorted binary tree.
// then, when creaeting new element pick the lowest id for the element.
// when removing elements, just write their ids to the tree and make inactive
int remove_children(ui_element_datas* ui_elements, int id)
{
	printf("ERROR: not implemented");
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