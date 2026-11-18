#include "simple_logger.h"
#include "GFC_Vector.h"
#include "player.h"
#include "gfc_input.h"

#include "collider.h"


int player_think(Entity* self);
int player_update(Entity* self);
int player_collide(Entity* self, void* collider);

GFC_Vector3D player_check_move(Entity* self, GFC_Vector3D move_position);

Entity* player_spawn(GFC_Vector3D position)
{
	Entity* self;
	self = entity_new();
	if (!self)return NULL;
	self->model = model_load("models/dino/dino.model");
	self->position = position;
	self->think = player_think;
	self->update = player_update;


	//collider setup
	GFC_Box box = gfc_box(position.x, position.y, position.z, 1, 1, 1);
	self->collider = collider_new(box, true, self);

	self->collide = player_collide;


	return self;
}


int player_think(Entity* self)
{
	//slog("playor thinkin");
	GFC_Vector3D move = { 0 };
	if (!self)return 0;
	if (gfc_input_command_down("enter"))
	{
		entity_free(self);
		return;
	}
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
		self->velocity = move;
		//ROTATION
		self->rotation.z = atan2(move.x, -move.y);//(2*M_PI)*360;

	}
	int grounded = 1;
	//gravity
	if (grounded == 1) //
	{
		//slog("my velocity is %f", self->velocity.z);
		//self->velocity.z -= 0.03f;
		//slog("NOW my velocity is %f", self->velocity.z);
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
	}
	self->position = new_position;


	Collider* collider = self->collider;
	if (collider->_inuse == 1)
	{
		collider->box.x = self->position.x;
		collider->box.y = self->position.y;
		collider->box.z = self->position.z;


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
}

int player_collide(Entity* self, void* collider) {
	//slog("player collided with something");
}


GFC_Vector3D player_check_move(Entity* self, GFC_Vector3D move_position) {
	if (!self->collider)return;
	Collider* collider = self->collider;
	if (collider->_inuse != 1)return;
	GFC_Box my_box = collider->box;
	my_box.x = move_position.x;
	my_box.y = move_position.y;
	my_box.z = move_position.z;
	//check for static collisions (level tiles for example)
	GFC_Box found_box = collider_manager_check_static_collisions(my_box);
	//if the box is empty, check for solid entities
	if (my_box.w == 0)
	{
		found_box = collider_manager_check_solid_collisions(my_box);
	}
	//set a max number of checks to avoid infinite loops, just in case
	int adjusted = 0;
	while (!(found_box.w == 0) && (adjusted <= 10))
	{
		/*
		slog("colliding with box");
		slog("My box  xyz whd: %f %f %f, %f %f %f", my_box.x, my_box.y, my_box.z, my_box.w, my_box.h, my_box.d);

		slog("found box  xyz whd: %f %f %f, %f %f %f", found_box.x, found_box.y, found_box.z, found_box.w, found_box.h, found_box.d);
		*/

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

		my_box.y = move_position.y;
		my_box.x = move_position.x;
		my_box.z = move_position.z;

		//check for static collisions (level tiles for example)
		GFC_Box found_box = collider_manager_check_static_collisions(my_box);
		//if the box is empty, check for solid entities
		if (my_box.w == 0)
		{
			found_box = collider_manager_check_solid_collisions(my_box);
		}
		adjusted += 1;
	}
	return move_position;
}
