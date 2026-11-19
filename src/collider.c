#include "simple_logger.h"

//include something here for visualizing...? do we have a simple box outline draw method

#include "entity.h"
//include camera here for draw calls
#include "collider.h"


typedef struct {
	Collider* colliderList;
	Uint32 colliderMax;
}ColliderManager;

static ColliderManager colliderManagerStatic = { 0 };
static ColliderManager colliderManagerDynamic = { 0 }; //static ColliderManager. is colliderManagerDynamic. which is it, huh? which is it?

void collider_manager_init(Uint32 max) {
	if (!max)
	{
		slog("need a max for collider manager initialization");
		return;
	}

	//set up both collider manager lists
	colliderManagerStatic.colliderList = malloc(sizeof(Collider) * max);
	colliderManagerDynamic.colliderList = malloc(sizeof(Collider) * max);

	if (!colliderManagerStatic.colliderList || !colliderManagerDynamic.colliderList)
	{
		slog("one of these collider lists is NO GOOD!!!!!!! DIE");
		return;
	}
	colliderManagerStatic.colliderMax = max;
	colliderManagerDynamic.colliderMax = max;

	atexit(collider_manager_close);
}

void collider_free(Collider* self)
{
	if (!self)return;
	self->_inuse = 0;
	memset(self, 0, sizeof(Collider));
}

void collider_manager_close() {
	int i;
	if (!colliderManagerDynamic.colliderList || !colliderManagerStatic.colliderList)return;
	for (i = 0; i < colliderManagerDynamic.colliderMax; i++)
	{
		collider_free(&colliderManagerDynamic.colliderList[i]);
	}
	free(colliderManagerDynamic.colliderList);
	for (i = 0; i < colliderManagerStatic.colliderMax; i++)
	{
		collider_free(&colliderManagerStatic.colliderList[i]);
	}
	free(colliderManagerStatic.colliderList);

	memset(&colliderManagerDynamic, 0, sizeof(ColliderManager));
	memset(&colliderManagerStatic, 0, sizeof(ColliderManager));
}

Uint8 gfc_box_overlap_excl(GFC_Box a, GFC_Box b) {
	if ((a.x >= b.x + b.w) || (b.x >= a.x + a.w) ||
		(a.y >= b.y + b.h) || (b.y >= a.y + a.h) ||
		(a.z >= b.z + b.d) || (b.z >= a.z + a.d))
	{
		return 0;
	}
	return 1;
}

Uint8 checkCollision(struct Collider_S self, struct Collider_S other) {

	Bool result = gfc_box_overlap_excl(self.box, other.box); //surely it can't be this easy. surely it can't be replace rect with box. but how beautiful if it is
	return result;
}

void collider_manager_check_collisions() {
	//here we go
	int i, j;
	//check all dynamic colliders for the j loop. static colliders will not need to check collisions themselves
	for (j = 0; j < colliderManagerDynamic.colliderMax; j++)
	{
		if (colliderManagerDynamic.colliderList[j]._inuse != 1)continue;

		//////DYNAMIC V DYNAMIC CHECK///////
		for (i = j + 1; i < colliderManagerDynamic.colliderMax; i++) {
			if (colliderManagerDynamic.colliderList[i]._inuse != 1)continue;
			if (checkCollision(colliderManagerDynamic.colliderList[j], colliderManagerDynamic.colliderList[i])) {
				if ((colliderManagerDynamic.colliderList[j].entity != NULL) && (colliderManagerDynamic.colliderList[i].entity != NULL))
				{
					//slog("two colliders are collidening");
					colliderManagerDynamic.colliderList[j].entity->collide(colliderManagerDynamic.colliderList[j].entity, &colliderManagerDynamic.colliderList[i]);
				}
				if ((colliderManagerDynamic.colliderList[j].entity != NULL) && (colliderManagerDynamic.colliderList[i].entity != NULL))
				{
					colliderManagerDynamic.colliderList[i].entity->collide(colliderManagerDynamic.colliderList[i].entity, &colliderManagerDynamic.colliderList[j]);
				}
			}
			else {
				//made you look
			}
		}

		//////DYNAMIC V STATIC CHECK///////
		for (i = 0; i < colliderManagerStatic.colliderMax; i++) {
			if (colliderManagerStatic.colliderList[i]._inuse != 1)continue;
			if (checkCollision(colliderManagerDynamic.colliderList[j], colliderManagerStatic.colliderList[i])) {
				if (!colliderManagerDynamic.colliderList[j].entity)continue;
				if (colliderManagerDynamic.colliderList[j].entity->collide) {
					colliderManagerDynamic.colliderList[j].entity->collide(colliderManagerDynamic.colliderList[j].entity, &colliderManagerStatic.colliderList[i]);
				}
			}
		}
	}
}



//Returns a pointer to the first overlapping collider in the list if the given rect would intersect with a static collider. Returns an empty box if no collider would
//AWESOME NOTE: may need to commandeer gfc_box_overlap as last semester to get an exclusive version instead of inclusive. see collider.c in 2dgame, ok? you did this before
GFC_Box collider_manager_check_static_collisions(GFC_Box box) {
	//Collider *collider = collider_new(rect, true, NULL);
	for (int i = 0; i < colliderManagerStatic.colliderMax; i++) {
		if (colliderManagerStatic.colliderList[i]._inuse != 1)continue;
		if (gfc_box_overlap_excl(box, colliderManagerStatic.colliderList[i].box)) {
			//slog("collided with static box");
			return colliderManagerStatic.colliderList[i].box;
		}
	}
	return (gfc_box(0, 0, 0, 0, 0, 0));
}

//Returns a pointer to the first overlapping collider in the list if the given rect would intersect with a dynamic (but solid) collider. Returns an empty box if no collider would
//second hearse same as the first
GFC_Box collider_manager_check_solid_collisions(GFC_Box box) {
	//Collider *collider = collider_new(rect, true, NULL);
	for (int i = 0; i < colliderManagerDynamic.colliderMax; i++) {
		if (colliderManagerDynamic.colliderList[i]._inuse != 1)continue;
		if (colliderManagerDynamic.colliderList[i].entity->solid != 1)continue;
		if (gfc_box_overlap_excl(box, colliderManagerDynamic.colliderList[i].box)) {
			//slog("collided with solid box");
			return colliderManagerDynamic.colliderList[i].box;
		}
	}
	return (gfc_box(0, 0, 0, 0, 0, 0));
}

//returns a shiny new collider for all the world to see. almost identical to last semester
Collider* collider_new(GFC_Box box, Bool isDynamic, Entity* entity) {
	//if the collider is dynamic do the appropriate stuff for that
	int i;
	ColliderManager* colliderManager;
	if (isDynamic) {
		colliderManager = &colliderManagerDynamic;
	}
	//if not
	else {
		colliderManager = &colliderManagerStatic;
	}
	if (!colliderManager->colliderList) {
		slog("no collider manager list yet!");
		return NULL;
	}
	for (i = 0; i < colliderManager->colliderMax; i++) {
		if (colliderManager->colliderList[i]._inuse == 1)continue;
		colliderManager->colliderList[i]._inuse = 1;
		colliderManager->colliderList[i].isDynamic = isDynamic;
		colliderManager->colliderList[i].box = box;
		colliderManager->colliderList[i].entity = entity;
		colliderManager->colliderList[i].free = collider_free;
		colliderManager->colliderList[i].team = 0;
		return &colliderManager->colliderList[i];
	}
	return NULL;
}

void collider_manager_draw_all() {
	//if i can find a quick and dirty way to draw boxes, put it here.
}

GFC_Vector3D box_get_center(GFC_Box my_box)
{
	GFC_Vector3D my_center = gfc_vector3d(
		my_box.x + 0.5 * my_box.w,
		my_box.y + 0.5 * my_box.h,
		my_box.z + 0.5 * my_box.d);
	return my_center;
}

GFC_Box box_scale(GFC_Box my_box, GFC_Vector3D scale)
{
	GFC_Box new_box = my_box;
	new_box.w *= scale.x;
	new_box.h *= scale.y;
	new_box.d *= scale.z;

	//scale back the new_box.x,y,z by 1/2 the width difference? maybe?
	new_box.x = my_box.x + (my_box.w - new_box.w) * 1 / 2;
	new_box.y = my_box.y + (my_box.h - new_box.h) * 1 / 2;
	new_box.z = my_box.z + (my_box.d - new_box.d) * 1 / 2;
	return new_box;
}

//sets a collider's position halfway because of centered models etc
void collider_reposition(Collider* self)
{
	self->box.x = self->entity->position.x - (0.5f * self->box.w);// *self->entity->scale.x);
	self->box.y = self->entity->position.y - (0.5f * self->box.h);// *self->entity->scale.y);
	self->box.z = self->entity->position.z - (0.5f * self->box.d);// *self->entity->scale.z);
}

GFC_Box box_reposition(GFC_Box box, GFC_Vector3D position)
{
	box.x = position.x - (0.5f * box.w);
	box.y = position.y - (0.5f * box.h);
	box.z = position.z - (0.5f * box.d);
	return box;
}