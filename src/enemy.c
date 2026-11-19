#include "simple_logger.h"

#include "enemy.h"
#include "player.h"
#include "collider.h"
#include "rpg_battler.h"

int enemy_think(Entity* self);
int enemy_update(Entity* self);
int enemy_postupdate(Entity* self);
int enemy_free(Entity* self);
int enemy_collide(Entity* self);

int enemy_attack(RPG_Battler* self);
int enemy_turn(RPG_Battler* enemy);

Entity* enemy_entity_new(GFC_Vector3D position)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->model = model_load("models/cube_enemy.model");
	self->position = position;
	self->think = enemy_think;
	self->update = enemy_update;
	self->postupdate = enemy_postupdate;

	//collider setup
	GFC_Box box = gfc_box(position.x, position.y, position.z, self->model->mesh->bounds.w, self->model->mesh->bounds.h, self->model->mesh->bounds.d);
	self->collider = collider_new(box, true, self);
	self->collide = enemy_collide;

	//rpgbattler setup
	RPG_Battler* battler = rpg_battler_new(self);
	battler->atk = 2;
	battler->def = 0;
	battler->name = "Enemy";
	battler->attack = enemy_attack;
	battler->turn = enemy_turn;
	self->rpg_battler = battler;
	
	return self;
}

int enemy_think(Entity* self) {
	return 0;
}
int enemy_update(Entity* self) {
	return 0;
}
int enemy_free(Entity* self) {
	return 0;
}
int enemy_collide(Entity* self) {
	return 0;
}

int enemy_turn(RPG_Battler* self) {
	enemy_attack(self);
	return 0;
}

int enemy_attack(RPG_Battler* self)
{
	attack(self);
	return 0;
}
int enemy_postupdate(Entity* self)
{
	if (self->removeme)
	{
		entity_free(self);
	}
}