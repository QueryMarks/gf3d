#include "simple_logger.h"
#include "GFC_Vector.h"
#include "player.h"
#include "gfc_input.h"


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
	//slog("playor thinkin");
	GFC_Vector3D move = { 0 };
	if (!self)return 0;
	if (gfc_input_command_down("item_up"))
	{
		//slog("moving up");
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

	if ((move.x!=0) || (move.y!=0))
	{
		slog("setting velocity");
		gfc_vector3d_normalize(&move);
		self->velocity = move;

	}
	return 0;

}

int player_update(Entity* self)
{
	gfc_vector3d_add(self->position, self->position, self->velocity);
	//dampening
	if (gfc_vector3d_magnitude_squared(self->velocity) > 0.01)
	{
		gfc_vector3d_scale(self->velocity, self->velocity, 0.8);
	}
	else gfc_vector3d_clear(self->velocity);
}

/**/