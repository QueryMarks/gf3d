#include "simple_logger.h"

#include "entity.h"
#include "model.h"
#include "gfc_color.h"
#include "collider.h"


typedef struct
{
	Entity* entityList;
	Uint32 maxEnts;
}EntityManager;

static EntityManager entity_manager = { 0 };
//forward declares
void entity_think(Entity* ent);
void entity_update(Entity* ent);
void entity_draw(Entity* ent);
int	entity_postupdate(struct Entity_S* self);

//just in case
void entity_collide(Entity* self, void* other)
{
	return;
}

void entity_close()
{
	int i;
	if (!entity_manager.entityList)return;
	for (i = 0; i < entity_manager.maxEnts; i++)
	{
		if (entity_manager.entityList[i]._inuse)entity_free(&entity_manager.entityList[i]);
	}
	free(entity_manager.entityList);
	memset(&entity_manager, 0, sizeof(entity_manager));

}

void entity_init(Uint32 maxEnts) {
	if (!maxEnts)
	{
		slog("failed to initialize entity system: maxEnts is 0");
		return;

	}
	entity_manager.entityList = gfc_allocate_array(sizeof(Entity), maxEnts);
	if (!entity_manager.entityList)
	{
		slog("failed to allocate entity list for %i entities", maxEnts);
		return;
	}
	entity_manager.maxEnts = maxEnts;
	slog("entity system initialized");
	atexit(entity_close);
}

Entity* entity_new()
{
	if (!entity_manager.entityList)
	{
		slog("no entity system set up yet");
		return NULL;
	}
	slog("creating entity before for loop");
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		slog("inside for loop at index %i", i);
		if (entity_manager.entityList[i]._inuse)continue;
		slog("setting inuse");
		entity_manager.entityList[i]._inuse = 1;

		//BOILERPLATE
		slog("boilerplate");
		entity_manager.entityList[i].scale = gfc_vector3d(1, 1, 1);
		entity_manager.entityList[i].colorMod = GFC_COLOR_WHITE;
		entity_manager.entityList[i].entity_type = "EMPTY_ENTITY";
		entity_manager.entityList[i].collide = entity_collide;
		entity_manager.entityList[i].postupdate = entity_postupdate;
		slog("successfully created entity WOWOWOWOWOW AWESOME!!! 1 1 1 !! !11!");
		return &entity_manager.entityList[i];
	}
	return NULL;
}



void entity_draw(Entity* ent)
{
	//slog("we are drawing entity");
	//slog("is this entity in use? %i", ent->_inuse);
	if ((!ent) || (!ent->_inuse))return;
	if (ent->draw)
	{
		if(ent->draw(ent))return;
	}
	//slog("made it to matrix setup");
	gfc_matrix4_from_vectors(
		ent->mat,
		ent->position,
		ent->rotation,
		ent->scale);
	//slog("about to queue render for model %s", ent->model->fileName);
	gf3d_model_queue_render(ent->model, ent->mat, ent->colorMod);
}
void entity_think(Entity* ent)
{
	if (ent->think)
	{
		if (ent->think(ent))return;
	}
	if ((!ent) || (!ent->_inuse))return;
}
void entity_update(Entity* ent)
{
	if (ent->update)
	{
		if (ent->update(ent))return;
	}
	if ((!ent) || (!ent->_inuse))return;
	gfc_vector3d_add(ent->position, ent->position, ent->velocity); 
	//dampening. maybe remove? add a condition on the entity?
	if (gfc_vector3d_magnitude_squared(ent->velocity) > 0.01)
	{
		gfc_vector3d_scale(ent->velocity, ent->velocity, 0.8);
	}
	else gfc_vector3d_clear(ent->velocity);
}
int	entity_postupdate(struct Entity_S* self) {
	if (self->removeme)
	{
		entity_free(self);
	}
}
void entity_system_draw()
{
	//slog("drawing entity 4 sure");

	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (!entity_manager.entityList[i]._inuse || !entity_manager.entityList[i].model)continue;
		entity_draw(&entity_manager.entityList[i]);
	}
}
void entity_system_think()
{
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (!entity_manager.entityList[i]._inuse)continue;
		entity_think(&entity_manager.entityList[i]);
	}
}
void entity_system_update()
{
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (!entity_manager.entityList[i]._inuse)continue;
		entity_update(&entity_manager.entityList[i]);
	}
}
void entity_system_postupdate() {
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (!entity_manager.entityList[i]._inuse)continue;
		entity_postupdate(&entity_manager.entityList[i]);
	}
}

void entity_free(Entity *ent) {
	if (!ent)return;
	if (!ent->_inuse)return;
	model_free(ent->model);
	if (ent->collider)
	{
		collider_free(ent->collider);
	}
	
	memset(ent, 0, sizeof(Entity));
};