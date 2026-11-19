#include "simple_logger.h"
#include "GFC_Vector.h"
#include "player.h"
#include "gfc_input.h"

#include "collider.h"

#include "rpg_battler.h"
#include "gf2d_font.h"

#include "shot.h"


int player_think(Entity* self);
int player_update(Entity* self);
int player_collide(Entity* self, void* collider);
int player_draw(Entity* self);

int player_turn(RPG_Battler* self);
int player_attack(RPG_Battler* self);


GFC_Vector3D player_check_move(Entity* self, GFC_Vector3D move_position);

Entity* player_spawn(GFC_Vector3D position)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->model = model_load("models/cube_player.model");
	self->position = position;
	self->think = player_think;
	self->update = player_update;
	self->draw = player_draw;
	self->battling = 0;


	//collider setup
	GFC_Box box = gfc_box(position.x, position.y, position.z, 2, 2, 2);
	Collider* collider = collider_new(box, true, self);
	collider->team = 1;
	self->collider = collider;
	collider_reposition(self->collider);

	self->collide = player_collide;

	slog("creating player battler");
	RPG_Battler* battler = rpg_battler_new(self);
	battler->max_hp = 20;
	battler->hp = battler->max_hp;
	battler->atk = 3;
	battler->def = 0;
	slog("before setting name");
	battler->name = "Player";
	battler->attack = player_attack;
	battler->turn = player_turn;
	battler->is_player = 1;
	self->rpg_battler = battler;
	slog("created battler");


	return self;
}


int player_think(Entity* self)
{
	//slog("playor thinkin");
	GFC_Vector3D move = { 0 };
	if (!self)return 0;
	RPG_Battler* battler = (RPG_Battler*)self->rpg_battler;

	/*if (gfc_input_command_down("enter"))
	{
		entity_free(self);
		return;
	}*/

	//rpg logic
	if (rpg_battle_get_active())
	{
		self->velocity = gfc_vector3d(0, 0, 0);
		if (gfc_input_command_pressed("action")) {
			battler->action_choice = 0;
			rpg_battle_set_player_ready(1);
		}
		else if (gfc_input_command_pressed("battle_1"))
		{
			//CREMATE
			battler->action_choice = 1;
			rpg_battle_set_player_ready(1);
		}
		else if (gfc_input_command_pressed("battle_2"))
		{
			//FLOAT
			battler->action_choice = 2;
			rpg_battle_set_player_ready(1);
		}
		else if (gfc_input_command_pressed("battle_3"))
		{
			//INTANGIBLE
			battler->action_choice = 3;
			rpg_battle_set_player_ready(1);
		}
		else if (gfc_input_command_pressed("battle_4"))
		{
			//MIRROR
			battler->action_choice = 4;
			rpg_battle_set_player_ready(1);
		}
		else if (gfc_input_command_pressed("battle_5"))
		{
			//PURPLE
			battler->action_choice = 5;
			rpg_battle_set_player_ready(1);
		}
		else if (gfc_input_command_pressed("battle_6"))
		{
			//CLEANSE
			battler->action_choice = 6;
			rpg_battle_set_player_ready(1);
		}
		else if (gfc_input_command_pressed("battle_7"))
		{
			//CURSE
			battler->action_choice = 7;
			rpg_battle_set_player_ready(1);
		}
	}
	else
	{
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

		if ((move.x != 0) || (move.y != 0))
		{
			//slog("setting velocity");
			gfc_vector3d_normalize(&move);
			gfc_vector3d_scale(move, move, 0.3);
			self->velocity.x = move.x;
			self->velocity.y = move.y;
			//ROTATION
			self->rotation.z = atan2(move.x, -move.y);//(2*M_PI)*360;

		}
		GFC_Vector3D aim = gfc_vector3d(sin(self->rotation.z), -cos(self->rotation.z), 0);
		if (gfc_input_command_pressed("battle_1"))
		{
			//CREMATE
			shot_entity_new(self->position, aim, SHOT_CREMATE, 60, self);
		}
		if (gfc_input_command_down("battle_2"))
		{
			//FLOAT
			self->velocity.z = 0.08;
			battler->status = STATUS_FLOAT;
		}
		if (gfc_input_command_down("battle_3"))
		{
			//INTANGIBLE
			battler->status = STATUS_INTANGIBLE;
		}
		if (gfc_input_command_pressed("battle_4"))
		{
			//MIRROR
			shot_entity_new(self->position, aim, SHOT_MIRROR, 120, self);
		}
		if (gfc_input_command_down("battle_5"))
		{
			battler->status = STATUS_PURPLE;
			//PURPLE
		}
		if (gfc_input_command_down("battle_6"))
		{
			//CLEANSE
			battler->hp = battler->max_hp;
			battler->status = STATUS_NONE;
		}
		int grounded = 1;
		//gravity
		if (grounded == 1) //
		{
			//slog("my velocity is %f", self->velocity.z);
			if (battler->status == STATUS_FLOAT)
			{
				self->velocity.z -= 0.01f;
			}
			else
			{
				self->velocity.z -= 0.03f;
			}

			//slog("NOW my velocity is %f", self->velocity.z);
		}

	}


	


	return 0;

}

int player_update(Entity* self)
{
	GFC_Vector3D new_position;
	gfc_vector3d_add(new_position, self->position, self->velocity);
	if (self->collider)
	{
		new_position = player_check_move(self, new_position);
		if (new_position.z <= -100) {
			//respawn
			new_position = gfc_vector3d(0, 0, 0);
			self->velocity = gfc_vector3d(0, 0, 0);
		}
	}
	self->position = new_position;


	Collider* collider = self->collider;
	if (collider->_inuse == 1)
	{

		collider_reposition(self->collider);


		//ground check rect here
	}

	//camera center on here
	// 
	// 
	
	//dampening
	GFC_Vector2D vel_xy = gfc_vector2d(self->velocity.x, self->velocity.y);
	if (gfc_vector2d_magnitude_squared(vel_xy) > 0.001f)
	{
		gfc_vector2d_scale(vel_xy, vel_xy, 0.8f);
	}
	else gfc_vector2d_clear(vel_xy);
	self->velocity.x = vel_xy.x;
	self->velocity.y = vel_xy.y;

	return 1;
}

int player_draw(Entity* self) {
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
	if (self->battling == 0)
	{

		char buffer[64];
		
		sprintf(buffer, "%s HP: %i", battler->name, battler->hp);
		gf2d_font_draw_line_tag(buffer, FT_H1, GFC_COLOR_WHITE, gfc_vector2d(550, 550));
	}


	//continue drawing player
	return 0;
}
int player_collide(Entity* self, void* collider) {
	
	
	//slog("player collided with something");
	Collider* collided = (Collider*)collider;
	if (collided->isDynamic) {
		//slog("collided with something dynamic");
	}
	RPG_Battler* my_battler = (RPG_Battler*)self->rpg_battler;
	if ((my_battler != NULL) && (collided->entity->rpg_battler != NULL)
		&& (!self->battling) && (!collided->entity->battling))
	{
		slog("Collided with a BATTLER!!!");
		RPG_Battler* battler = (RPG_Battler*)collided->entity->rpg_battler;
		if (battler->is_player != 1) {
			slog("Initiating enemy battle.");
			rpg_battle_start(self->rpg_battler, battler);
		}
	}
}


GFC_Vector3D player_check_move(Entity* self, GFC_Vector3D move_position) {
	if (!self->collider)return;
	RPG_Battler* battler = self->rpg_battler;
	Collider* collider = self->collider;
	if (collider->_inuse != 1)return;
	GFC_Box my_box = box_reposition(collider->box, move_position);
	//box_reposition(my_box, move_position);
	//check for static collisions (level tiles for example)
	GFC_Box found_box = collider_manager_check_static_collisions(my_box);
	//if the box is empty, check for solid entities
	if (found_box.w == 0)
	{
		if (battler->status != STATUS_INTANGIBLE)
		{
			found_box = collider_manager_check_solid_collisions(my_box);
		}
		
	}
	//set a max number of checks to avoid infinite loops, just in case
	int adjusted = 0;
	while (!(found_box.w == 0) && (adjusted <= 10))
	{
		
		//slog("colliding with box");
		//slog("My box  xyz whd: %f %f %f, %f %f %f", my_box.x, my_box.y, my_box.z, my_box.w, my_box.h, my_box.d);

		//slog("found box  xyz whd: %f %f %f, %f %f %f", found_box.x, found_box.y, found_box.z, found_box.w, found_box.h, found_box.d);
		

		double vectorRight = -(my_box.x - (found_box.x + found_box.w));
		double vectorLeft = -((my_box.w + my_box.x) - found_box.x);
		
		double vectorDown = -(my_box.y - (found_box.y + found_box.h));
		double vectorUp = -((my_box.h + my_box.y) - found_box.y);

		double vectorFwd = -(my_box.z - (found_box.z + found_box.d));
		double vectorBack = -((my_box.z + my_box.d) - found_box.z);


		float min_x = min(abs(vectorRight), abs(vectorLeft));
		float min_y = min(abs(vectorDown), abs(vectorUp));
		float min_z = min(abs(vectorFwd), abs(vectorBack));

		//only use the smallest adjustment possible per adjustmetn
		if ((min_x < min_y) && min_x < min_z)
		{
			if (abs(vectorRight) < abs(vectorLeft))
			{
				move_position.x += vectorRight;
			}
			else
			{
				move_position.x += vectorLeft;
			}
			self->velocity.x = 0;
		}
		else if (min_y < min_z)
		{
			if (abs(vectorUp) < abs(vectorDown))
			{
				move_position.y += vectorUp;
			}
			else
			{
				move_position.y += vectorDown;
			}
			self->velocity.y = 0;
		}
		else
		{
			if (abs(vectorFwd) < abs(vectorBack))
			{
				//slog("hit on z moving FWD");
				move_position.z += vectorFwd;
			}
			else
			{
				//slog("hit on z moving BACK");
				move_position.z += vectorBack;
			}
			self->velocity.z = 0;
		}

		//set the new box space for next loop

		my_box = box_reposition(my_box, move_position);

		//check for static collisions (level tiles for example)
		found_box = collider_manager_check_static_collisions(my_box);
		//if the box is empty, check for solid entities
		if (found_box.w == 0)
		{
			if (battler->status != STATUS_INTANGIBLE)
			{
				found_box = collider_manager_check_solid_collisions(my_box);
			}
		}
		adjusted += 1;
	}
	return move_position;
}
//custom behavior here
int player_turn(RPG_Battler* self) {
	RPG_Battler* enemy = rpg_battle_get_enemy();
	switch (self->action_choice)
	{
		case 0:
			player_attack(self);
			break;
		case 1:
			enemy->status = STATUS_CREMATE;
			slog("%s was cremated! Fire damage every turn!", enemy->name);
			break;
		case 2:
			self->status = STATUS_FLOAT;
			slog("%s began floating! DEF went up from the high ground!", self->name);
			break;
		case 3:
			slog("%s went intangible! Immune to regular attacks.", self->name);
			self->status = STATUS_INTANGIBLE;
			break;
			
		case 4:
			slog("MIRROR IMAGE! %s swapped health!", self->name);
			int temp_hp = self->hp;
			self->hp = enemy->hp;
			enemy->hp = temp_hp;
			break;
		case 5: 
			slog("IT'S PURPLE WEEK! The purple takes hold...");
			self->status = STATUS_PURPLE;
			enemy->status = STATUS_PURPLE;
			break;
		case 6:
			slog("Cleanse! Restore status and HP!");
			self->status = STATUS_NONE;
			self->hp = self->max_hp;
			break;
		case 7:
			slog("A thousand curses upon %s!", enemy->name);
			enemy->status = STATUS_CURSE;
			break;

	}
	
}
int player_attack(RPG_Battler* self) {
	attack(self);
}