#ifndef __SHOT_H__
#define __SHOT_H__

#include "entity.h"
#include "gfc_vector.h"

Entity* shot_entity_new(GFC_Vector3D position, GFC_Vector3D direction, int type, int timer, Entity* owner);

typedef struct Shot_Data_S {
	Uint8 type; //type of shot, use the enum
	int timer; //will die at 0
	Entity* owner; //reference to the player
}Shot_Data;

enum SHOTTYPE {
	SHOT_CREMATE,
	SHOT_MIRROR
};




#endif