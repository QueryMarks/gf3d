#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "gfc_types.h"
#include "gfc_primitives.h"
#include "entity.h"

/**
* @purpose like the 2d project, this will allow us to create colliders and a collision system (box based for now, for sure)
**/

typedef struct Collider_S {
	Uint8 _inuse;
	Entity* entity;
	GFC_Box box; //using box colliders for now
	Bool isDynamic; //true if the collider is dynamic
	Uint8 team; //like before, <0 for solid, 1 for player, 2 for enemy/damageable

	void(*free)(struct Collider_S* self);

}Collider;

Collider* collider_new(GFC_Box box, Bool isDynamic, Entity* entity);


void collider_manager_init(Uint32 max);
void collider_manager_close();
void collider_manager_draw_all();
void collider_free(Collider* self);
void collider_manager_check_collisions();
Uint8 checkCollision(struct Collider_S self, struct Collider_S other);

enum TEAM
{
	SOLID,
	PLAYER,
	ENEMY
};

GFC_Box collider_manager_check_static_collisions(GFC_Box box);
GFC_Box collider_manager_check_solid_collisions(GFC_Box box);


GFC_Vector3D box_get_center(GFC_Box my_box);

#endif