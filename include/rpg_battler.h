#ifndef __RPG_BATTLER_H__
#define __RPG_BATTLER_H__


#include "gfc_types.h"
#include "entity.h"
#include "gfc_text.h"

typedef enum
{
	STATUS_NONE = 0,
	STATUS_CREMATE = 1,
	STATUS_FLOAT = 2,
	STATUS_INTANGIBLE = 3,
	STATUS_CURSE = 4,
	STATUS_PURPLE = 5
}Status;

typedef struct RPG_Battler_S
{
	Uint8			_inuse;
	int			atk;
	int			def;
	int			lvl;
	int			max_hp;
	int			hp;
	int			spd;
	Uint8			action_choice; //0 if not chosen
	Uint8			status;
	Uint8			is_player; //0 if enemy, 1 if player

	const char*		name;
	int				(*turn)(struct RPG_Battler_S* self);
	int				(*attack)(struct RPG_Battler_S* self);
	Entity*			entity;
}RPG_Battler;

void rpg_battle_manager_init();

RPG_Battler* rpg_battler_new(Entity* entity);

RPG_Battler* rpg_battle_get_enemy();
RPG_Battler* rpg_battle_get_player();

void rpg_battle_start(RPG_Battler* player, RPG_Battler* enemy);
void rpg_battle_end();
void rpg_battle_advance_turn_phase();

void rpg_battle_think();
void rpg_battle_update();
void rpg_battle_draw();

void attack(RPG_Battler* self);

void rpg_battle_set_active(Uint8 active);

Uint8 rpg_battle_get_active();

void rpg_battle_set_player_ready(Uint8 set);


#endif