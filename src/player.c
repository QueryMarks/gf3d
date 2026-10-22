#include "simple_logger.h"
#include "GFC_Vector.h"
#include "player.h"



int player_think(Entity* self);
int player_update(Entity* self);

Entity* player_spawn(GFC_Vector3D position)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->model = model_load("models/dino/dino.model");
	self->position = position;
	self->think = player_think;
	self->update = player_update;


	return self;
}


int player_think(Entity* self)
{
	GFC_Vector3D move = { 0 };
	if (!self)return 0;
	if (gfc_input_command_down("item_up"))
	{
		move.y = 1;
	}
	if (gfc_input_command_down("item_down"))
	{
		move.y = -1;
	}
	if (gfc_input_command_down("item_left"))
	{
		move.x = -1;
	}
	if (gfc_input_command_down("item_right"))
	{
		move.x = 1;
	}

	if ((move.x) || (move.y))
	{
		gfc_vector3d_normalize(&move);
		self->velocity = move;

	}
	return 0;

}

int player_update(Entity* self)
{
	return;
}

/**/