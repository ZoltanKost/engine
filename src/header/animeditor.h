#ifndef ANIM_EDITOR
#define ANIM_EDITOR

#include "ui.h"
#include "raylib.h"
#include "animation.h"
#include "ship.h"

#define SCREEN_WIDTH 1368
#define SCREEN_HEIGHT 720 

void AddFrameToEditingAnimation(int paramsCount);
void AddEventToEditingAnimation(int frameNumber);

int InitEditAnimationWindow(FrameAnimation editingAnimation, 
							int* editing_frame_count, int parentID, ui_element_datas* uiDatas);
                            

int ReInitEditAnimationWindowWithNewAnimation(FrameAnimation editingAnimation, 
							int* editing_frame_count, int existingWindowParent, ui_element_datas* uiDatas);
void SwitchEditingAnimationFlag(int id);

void change_frame_attached_events(int id);
void choose_frame_to_edit(int id);
void init_frame_attached_events();
void update_frame_attached_events();

#endif