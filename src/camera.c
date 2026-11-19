#include "simple_logger.h"

#include "camera.h"
#include "gfc_matrix.h"


//CURRENTLY UNUSED. i am setting the camera manually in game.c for now
static Camera camera = { 0 };


Entity* cameraNew(GFC_Vector3D position, Entity* target, GFC_Vector3D position_target, Uint8 follow) {
	Entity* camera_ent = entity_new();
	Camera* new_camera = malloc(sizeof(Camera));
	if (target != NULL)
	{
		new_camera->target = target;
	}
	else
	{
		new_camera->position_target = position_target;
	}
	camera_ent->position = position;
	new_camera->follow = follow;
}

void set_active_camera(Camera* use_camera)
{
	camera.distance = use_camera->distance;
	camera.target = use_camera->target;
	camera.position_target = use_camera->position_target;
	camera.self = use_camera->self;
	use_camera->active = 1;
	camera.active = 1;
	camera.follow = use_camera->follow;
}

camera_matrix4_view(float(*out)[4], GFC_Vector3D position, GFC_Vector3D target, GFC_Vector3D up)
{

}