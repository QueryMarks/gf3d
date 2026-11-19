#include "simple_logger.h"
#include "rpg_battler.h"
#include "gfc_input.h"
#include "gf2d_font.h"


typedef struct {
	RPG_Battler* battlerList;
	RPG_Battler* player;
	RPG_Battler* enemy;

	Uint8 turn_phase;
	Uint8 active;
	Uint8 player_ready;

}RPG_Battle_Manager;




static RPG_Battle_Manager rpg_battle_manager = { 0 };



void rpg_battle_manager_init() {
	slog("starting battle manager");
	rpg_battle_manager.battlerList = gfc_allocate_array(sizeof(RPG_Battler), 128);
	rpg_battle_manager.turn_phase = 0;
	rpg_battle_manager.active = 0;
	slog("done w battle manager");
}

RPG_Battler* rpg_battler_new(Entity* entity) {
	slog("making a new battler");
	int i;
	for (i = 0; i < 128; i++)
	{
		slog("We are at i = %i", i);
		if (rpg_battle_manager.battlerList[i]._inuse)continue;
		slog("making this battler");
		rpg_battle_manager.battlerList[i]._inuse = 1;
		slog("set inuse");
		rpg_battle_manager.battlerList[i].name = "Battler";
		slog("set name");
		rpg_battle_manager.battlerList[i].max_hp = 10;
		slog("set max_hp");
		rpg_battle_manager.battlerList[i].hp = rpg_battle_manager.battlerList[i].max_hp;
		slog("set hp");
		if (entity != NULL)
		{
			rpg_battle_manager.battlerList[i].entity = entity;
			slog("set entity ohhh i get it");
		}
		rpg_battle_manager.battlerList[i].attack = attack;
		return &rpg_battle_manager.battlerList[i];
	}
	

	
}


RPG_Battler* rpg_battle_get_enemy() {
	return rpg_battle_manager.enemy;
}

RPG_Battler* rpg_battle_get_player() {
	return rpg_battle_manager.player;
}

void rpg_battle_start(RPG_Battler* player, RPG_Battler* enemy)
{
	if (!player || !enemy)
	{
		slog("missing player or enemy battler to start battle!");
		return;
	}
	if (rpg_battle_manager.active == 1) {
		slog("Battle already in progress. Hold your horses.");
		return;
	}
	rpg_battle_manager.active = 1;
	rpg_battle_manager.player = player;
	rpg_battle_manager.enemy = enemy;
	rpg_battle_manager.turn_phase = 0;
}

void rpg_battle_end()
{
	if (rpg_battle_manager.player->hp <= 0)
	{
		slog("Player has DIED.");
		exit(0);
		slog_sync();
	}
	else
	{
		slog("Defeated %s!", rpg_battle_manager.enemy->name);
		rpg_battle_manager.active = 0;
		rpg_battle_manager.turn_phase = 0;
		rpg_battle_manager.player->entity->battling = 0;
		rpg_battle_manager.player = NULL;
		rpg_battle_manager.enemy->entity->removeme = 1;
		rpg_battle_manager.enemy->entity->battling = 0;
		rpg_battle_manager.enemy = NULL;
	}
}


void rpg_battle_advance_turn_phase() {
	rpg_battle_manager.turn_phase++;
}
void rpg_battle_think() {
	if (!rpg_battle_manager.active) return;
	RPG_Battler* player = rpg_battle_get_player();
	RPG_Battler* enemy = rpg_battle_get_enemy();
	switch (rpg_battle_manager.turn_phase)
	{
		case 0:
			if (rpg_battle_manager.player_ready == 0) {
				slog("Choose wisely, haunt...");
				break;
			}
			else
			{
				slog("Go for it!!");
				rpg_battle_set_player_ready(0);
				rpg_battle_advance_turn_phase();
			}
		case 1:
			if (gfc_input_command_pressed("action"))
			{
				rpg_battle_advance_turn_phase();
			}
			break;
		case 2:
			player->turn(player);
			rpg_battle_advance_turn_phase();
			break;

		case 3:
			if (gfc_input_command_pressed("action"))
			{
				rpg_battle_advance_turn_phase();
			}
			break;
		case 4:
			enemy->turn(enemy);
			rpg_battle_advance_turn_phase();
			break;
		case 5:
			if (gfc_input_command_pressed("action"))
			{
				rpg_battle_advance_turn_phase();
			}
			break;
		default:
			slog("Round over.");
			rpg_battle_manager.turn_phase = 0;
	}
}

void rpg_battle_update() {
	if (!rpg_battle_manager.active) return;
}
void rpg_battle_draw() {
	if (!rpg_battle_manager.active) return;
	char buffer[64];
	sprintf(buffer, "%s HP: %i", rpg_battle_manager.enemy->name, rpg_battle_manager.enemy->hp);
	gf2d_font_draw_line_tag(buffer, FT_H1, GFC_COLOR_WHITE, gfc_vector2d(800, 10));

	sprintf(buffer, "%s HP: %i", rpg_battle_manager.player->name, rpg_battle_manager.player->hp);


	gf2d_font_draw_line_tag(buffer, FT_H1, GFC_COLOR_WHITE, gfc_vector2d(120, 10));
}

void attack(RPG_Battler* self) {
	//default attack behavior
	RPG_Battler* opponent;
	int damage;
	if (self->is_player)
	{
		opponent = rpg_battle_get_enemy();
	}
	else
	{
		opponent = rpg_battle_get_player();
	}
	if (opponent != NULL)
	{
		damage = self->atk - opponent->def;
		if (damage >= 0)
		{
			slog("%s's attack! Did %i damage to %s.", self->name, damage, opponent->name);
			opponent->hp -= damage;
			if (opponent->hp <= 0)
			{
				rpg_battle_end();
			}
		}
		else
		{
			slog("%s's attack was ineffective...", self->name);
		}
	}
}

void rpg_battle_set_active(Uint8 active)
{
	rpg_battle_manager.active = active;
}

Uint8 rpg_battle_get_active() {
	return rpg_battle_manager.active;
}

void rpg_battle_set_player_ready(Uint8 set)
{
	if (rpg_battle_manager.turn_phase == 0) {
		rpg_battle_manager.player_ready = set;
	}
}