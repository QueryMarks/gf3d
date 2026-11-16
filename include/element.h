#ifndef __ELEMENT_H__
#define __ELEMENT_H__


#include "simple_logger.h"
#include "gfc_types.h"

#include "gf2d_actor.h"
#include "gf2d_sprite.h"
#include "window.h"

typedef enum
{
	ELEMENT_IDLE,
	ELEMENT_DISABLE,
	ELEMENT_HIGHLIGHT,
	ELEMENT_HIDDEN,
	ELEMENT_ACTIVE
}ElementState;


typedef enum
{
	ET_LABEL,
	ET_BUTTON,
	ET_TEXT
}ElementTypes;


struct Element_S
{
	Uint8 _inuse;
	int index;
	GFC_TextLine name;
	GFC_Vector2D position;
	
	Actor* actor;
	const char* text;
	Uint8 canHasFocus;
	Uint8 hasFocus;
	GFC_Rect bounds;
	int state;
	int elementType;

	void(*think)(struct Element_S* self);
	void(*update)(struct Element_S* self);
	void(*free)(struct Element_S* self);
	void(*onclick)(struct Element_S* self);
	Window* window;
	void* data;
	float frame;
};


Element* element_new(Window* window);

Element* button_new(
	Window* window,
	GFC_Vector2D position,
	Sprite* sprite,
	GFC_Vector2D size,
	void* onclick);

Element* text_new(
	Window* window,
	GFC_Vector2D position,
	GFC_Vector2D size,
	const char* text
);



void element_draw(Element* self);

void onclick_close(struct Element_S* self);

void onclick_start_level(struct Slement_S* self);

#endif