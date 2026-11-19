#include "simple_logger.h"

#include "enemy.h"
#include "player.h"
#include "collider.h"
#include "rpg_battler.h"

int enemy_think(Entity* self);
int enemy_update(Entity* self);
int enemy_postupdate(Entity* self);
int enemy_free(Entity* self);
int enemy_collide(Entity* self, void* collider);

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
	GFC_Box box = gfc_box(position.x, position.y, position.z, 2, 2, 2);
	self->collider = collider_new(box, true, self);
	collider_reposition(self->collider);
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
	RPG_Battler* battler = self->rpg_battler;
	switch (battler->status)
	{
	case STATUS_NONE:
		self->colorMod = GFC_COLOR_WHITE;
		break;
	case STATUS_FLOAT:
		self->colorMod = GFC_COLOR_LIGHTBLUE;
		break;
	case STATUS_CREMATE:
		self->colorMod = GFC_COLOR_RED;
		break;
	case STATUS_INTANGIBLE:
		self->colorMod = GFC_COLOR_BLACK;
		break;
	case STATUS_CURSE:
		self->colorMod = GFC_COLOR_GREEN;
		break;
	case STATUS_PURPLE:
		self->colorMod = GFC_COLOR_MAGENTA;
		break;
	default:
		break;
	}
	return 0;
}
int enemy_free(Entity* self) {
	return 0;
}
int enemy_collide(Entity* self, void*collider) {
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