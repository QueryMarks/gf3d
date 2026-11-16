#ifndef __WINDOW_H__
#define __WINDOW_H__


#include "gfc_types.h"
#include "gf2d_actor.h"
#include "gfc_shape.h"

#include "SDL_ttf.h"


/**
* @purpose a system for window elements
* this is going to mostly just be my window system from last semester!
**/


typedef struct Element_S Element;
/**
* @purpose This lets me use windows and menus and wenus
**/
typedef struct Window_S {
	Uint8 _inuse; /**<flag for keeping track of memory usage*/
	Uint8 hidden;
	Sprite* sprite;
	Actor* spriteActor;
	Actor* textActor;
	//button goes here when we add it;
	GFC_Vector2D position;
	GFC_Vector2D textPosition;
	TTF_Font* font;

	Element* elements;
	GFC_List* focus_elements;
	Element* focus;

	void(*think)(struct Window_S* self);
	void(*update)(struct Window_S* self);
	void(*free)(struct Entity_S* self);

	void* data;
}Window;

void window_manager_init(Uint32 max);

void window_manager_close();

Window* window_new();

void window_free(Window* self);

void window_add_element(Window* self);

void window_manager_think_all();

void window_manager_update_all();

void window_manager_draw_all();



void write_window_text(Window* self, const char* write);

#endif