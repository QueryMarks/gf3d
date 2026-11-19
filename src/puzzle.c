#include "simple_logger.h"

#include "collider.h"
#include "gfc_text.h"
#include "shot.h"
#include "gf2d_font.h"
#include "rpg_battler.h"

int door_collide(Entity* self, void* other);

int awesome_purple_agumon_update(Entity* self);
int awesome_purple_agumon_collide(Entity* self, void* other);

Entity* wood_door_entity_new(GFC_Vector3D position)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->model = model_load("models/door.model");
	self->position = position;
	self->scale = gfc_vector3d(3, 0.5, 5);
	self->solid = 1;
	GFC_Box box = gfc_box(0, 0, 0, 2*3, 2*0.5, 2*5);
	Collider* collider = collider_new(box, true, self);
	collider_reposition(collider);
	self->collider = collider;

	self->collide = door_collide;
	
	return self;
}

Entity* grate_door_entity_new(GFC_Vector3D position)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->model = model_load("models/grate_door.model");
	self->position = position;
	self->scale = gfc_vector3d(3, 0.5, 5);
	self->solid = 1;
	GFC_Box box = gfc_box(0, 0, 0, 2 * 3, 2 * 0.5, 2 * 5);
	Collider* collider = collider_new(box, true, self);
	collider_reposition(collider);
	self->collider = collider;

	return self;
}

Entity* awesome_purple_agumon_entity_new(GFC_Vector3D position)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->model = model_load("models/dino/dino.model");
	self->position = position;
	self->scale = gfc_vector3d(0.3, 0.3, 0.3);
	self->update = awesome_purple_agumon_update;
	self->collide = awesome_purple_agumon_collide;
	GFC_Box box = gfc_box(0, 0, 0, 2, 2, 2);
	Collider* collider = collider_new(box, true, self);
	collider_reposition(collider);
	self->collider = collider;
	self->data = gfc_allocate_array(sizeof(Uint16), 1);
	self->colorMod = gfc_color(0.8, 0, 0.8, 1);

	return self;
}


door_collide(Entity* self, void* other)
{
	
	Collider* other_collider = other;
	if (gfc_stricmp(other_collider->entity->entity_type, "shot") == 0)
	{
		slog("collided with shot");
		Shot_Data* shot_data = other_collider->entity->data;
		if (shot_data->type == SHOT_CREMATE)
		{
			self->removeme = 1;
		}
	}
}


int awesome_purple_agumon_update(Entity* self) {

	Uint16* timer = (Uint16*)self->data;
	if (*timer >= 1) {
		*timer += 1;
		gf2d_font_draw_line_tag("AWESOME PURPLE AGUMON'S PURPLE LONELINESS HAS BEEN CURED", FT_H1, gfc_color(200,0,200,1), gfc_vector2d(10, 350));

		if (*timer >= 300)
		{
			self->removeme = 1;
		}
	
	}
	return 0;
}
int awesome_purple_agumon_collide(Entity* self, void* other) {
	if (!self)return;
	Uint16* timer = (Uint16*)self->data;
	if (*timer > 0)return;
	Collider* other_collider = other;
	RPG_Battler* battler;
	if (other_collider->entity != NULL) {
		battler = other_collider->entity->rpg_battler;
		if (battler != NULL)
		{
			if (battler->status == STATUS_PURPLE)
			{
				*timer = 1;
			}
		}
	}
	return 0;
}
