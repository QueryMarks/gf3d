#include "simple_logger.h"
#include "collider.h"
#include "shot.h"

#include "rpg_battler.h"

float shot_speed = 0.5;

int shot_think(Entity* self);
int shot_update(Entity* self);
int shot_free(Entity* self);
int shot_collide(Entity* self, void* collider);
int shot_postupdate(Entity* self);

Entity* shot_entity_new(GFC_Vector3D position, GFC_Vector3D direction, int type, int timer, Entity* owner)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->entity_type = "shot";
	self->model = model_load("models/cube_shot.model");
	self->scale = gfc_vector3d(0.5, 0.5, 0.5);
	self->position = position;
	self->think = shot_think;
	self->update = shot_update;
	self->postupdate = shot_postupdate;
	slog("made it past functions part");

	gfc_vector3d_scale(self->velocity, direction, 0.5);
	
	Collider* collider = collider_new(gfc_box(0, 0, 0, 1, 1, 1), true, self);

	self->collide = shot_collide;
	collider->team = 1;
	self->collider = collider;
	

	slog("made it past collider  part");
	Shot_Data* shot_data = malloc(sizeof(Shot_Data));

	if (shot_data) {
		shot_data->timer = timer;
		shot_data->type = type;
		shot_data->owner = owner;
	}
	self->data = shot_data;


	slog("made it past shot_data part");
	if (type == SHOT_CREMATE)
	{
		self->colorMod = GFC_COLOR_RED;
	}

	slog("returning");
	return self;
}

int shot_think(Entity* self)
{
	if (!self)return;
	return 0;
}

int shot_update(Entity* self)
{	
	Shot_Data* shot_data = self->data;
	if (shot_data->timer <= 0) {
		self->removeme = 1;
	}
	else
	{
		shot_data->timer -= 1;
	}

	gfc_vector3d_add(self->position, self->position, self->velocity);
	collider_reposition(self->collider);
	return 1;
}

int shot_free(Entity* self) {
	if (!self)return 0;
	//collider_free(self->collider);
	//probably free stuff here if necessary
}

int shot_collide(Entity* self, void* collider)
{
	if (!self || !collider)return 0;
	Collider* other = collider;
	Shot_Data* shot_data = self->data;
	RPG_Battler* other_battler;
	if (other->entity == NULL)
	{
		return;
	}
	other_battler = other->entity->rpg_battler;
	if (other->team == 0)
	{
		if (shot_data->type == SHOT_CREMATE)
		{

			if (other_battler != NULL)
			{
				other_battler->status = STATUS_CREMATE;
			}
		}
		else if (shot_data->type == SHOT_MIRROR)
		{
			if (other_battler != NULL)
			{
				GFC_Vector3D temp;
				temp = shot_data->owner->position;
				shot_data->owner->position = other->entity->position;
				other->entity->position = temp;
				collider_reposition(collider);
				collider_reposition(shot_data->owner->collider);
				self->removeme = 1;
			}
		}
	}
	return 0;

}


int shot_postupdate(Entity* self) {
	if (self->removeme == 1)
	{
		shot_free(self);
	}
	return 1;
}