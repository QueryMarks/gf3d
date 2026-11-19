#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "entity.h"


//CURRENTLY UNUSED. i am setting the camera manually in game.c for now
typedef struct
{
	float distance;
	Entity* target;
	GFC_Vector3D position_target;
	Entity* self;
	Uint8 active; //0 for false, 1 for true
	Uint8 follow; //0 for stationary, 1 if it should follow position (translate same as player, for example)

}Camera;

Entity* cameraNew();


#endif