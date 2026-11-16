#include "simple_logger.h"

#include "gfc_types.h"
#include "gf2d_sprite.h"
#include "gf2d_actor.h"

#include "window.h"
#include "element.h"



typedef struct
{
	Sprite* background;
	Actor* backgroundActor;
	Window* windowList;
	Uint32 windowMax;
}WindowManager;


static WindowManager windowManager = { 0 };


void window_manager_init(Uint32 max)
{

	if (!max) {
		slog("no window max for manager");
		return;
	}
	windowManager.windowList = malloc(sizeof(Window) * max);

	if (!windowManager.windowList) {
		slog("failed to allocate %i windows", max);
		return;
	}

	windowManager.windowMax = max;
	windowManager.background = gf2d_sprite_load_image("images/dialoguebox.png");
	//SDL_SetTextureAlphaMod(windowManager.background->texture, 100);
	/*Actor* actor = gf2d_actor_new();
	actor->sprite = sprite;
	windowManager.backgroundActor = actor;*/
	atexit(window_manager_close);
}


void window_manager_close()
{
	int i;
	if (!windowManager.windowList)return;

	for (i = 0; i < windowManager.windowMax; i++)
	{
		if (&windowManager.windowList[i]._inuse != 1)continue;
		window_free(&windowManager.windowList[i]);
	}
	free(windowManager.windowList);
	memset(&windowManager, 0, sizeof(WindowManager));
}

Window* window_new() {
	int i;
	if (!windowManager.windowList) {
		return NULL;
	}
	for (i = 0; i < windowManager.windowMax; i++) {
		if (windowManager.windowList[i]._inuse == 1)continue;
		windowManager.windowList[i]._inuse = 1;
		windowManager.windowList[i].position = gfc_vector2d(200, 200);
		windowManager.windowList[i].sprite = windowManager.background;
		windowManager.windowList[i].spriteActor = windowManager.backgroundActor;
		windowManager.windowList[i].hidden = 0;
		windowManager.windowList[i].elements = malloc(sizeof(Element) * 32);

		//Sprite* buttonSprite = gf2d_sprite_load_all("images/floater.png", 64, 64, 1, 0);
		//button_new(&windowManager.windowList[i], gfc_vector2d(100, 100), buttonSprite, gfc_vector2d(64, 64), onclick_close);

		return &windowManager.windowList[i];
	}
	return NULL;
}

void window_draw(Window* self) {
	if (!self)return;
	if (self->hidden == 0) {
		slog("drawing window??? %s", self->sprite->filename);
		gf2d_sprite_draw_image(self->sprite, self->position);
		//slog("drew window sprite at pos %f, %f", self->position.x, self->position.y);
		if (self->elements != NULL) {
			int j;
			for (j = 0; j < 32; j++) {
				if (self->elements[j]._inuse == 1) {
					//slog("drawing element of window");
					element_draw(&self->elements[j]);
				}

			}
		}
	}

	//slog("about to draw in window_draw");


}

void window_manager_draw_all() {
	//slog("drawing windows");
	int i;
	if (!windowManager.windowList) {
		//slog("no window list to draw");
		return;
	}
	for (i = 0; i < windowManager.windowMax; i++) {
		//slog("drawing window %i", i);
		if (windowManager.windowList[i]._inuse == 1) {
			if (windowManager.windowList[i].sprite != NULL)
			{
				//slog("window_draw about to get called");
				window_draw(&windowManager.windowList[i]);
				//slog("drew this window fully");
			}

		}
	}
}


void window_update(Window* self) {
	if (!self)return;
	if (self->hidden == 0) {
		if (self->elements != NULL) {
			int j;
			for (j = 0; j < 32; j++) {
				if (self->elements[j]._inuse == 1) {
					self->elements[j].update(&self->elements[j]);
				}
			}
		}
	}
}

void window_manager_update_all() {
	//slog("updating windows");
	int i;
	if (!windowManager.windowList) {
		//slog("no window list to draw");
		return;
	}
	for (i = 0; i < windowManager.windowMax; i++) {
		//slog("drawing window %i", i);
		if (windowManager.windowList[i]._inuse == 1) {
			window_update(&windowManager.windowList[i]);
		}
	}
}

void window_free(Window* self) {
	if (!self)return;
	if (self->sprite)gf2d_sprite_free(self->sprite);
	if (self->elements != NULL) {
		int j;
		for (j = 0; j < 32; j++) {
			if (self->elements[j]._inuse == 1) {
				element_free(&self->elements[j]);
			}
		}
	}
	self->_inuse = false;
}

void window_manager_think_all() {
	//no behavior here yet
	return;
}

