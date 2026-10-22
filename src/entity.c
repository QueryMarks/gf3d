#include "simple_logger.h"

#include "entity.h"
#include "model.h"
#include "gfc_color.h"


typedef struct
{
	Entity* entityList;
	Uint32 maxEnts;
}EntityManager;

static EntityManager entity_manager = { 0 };

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
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (entity_manager.entityList[i]._inuse)continue;
		entity_manager.entityList[i]._inuse = 1;

		//BOILERPLATE
		entity_manager.entityList[i].scale = gfc_vector3d(1, 1, 1);
		entity_manager.entityList[i].colorMod = GFC_COLOR_WHITE;
		return &entity_manager.entityList[i];
	}
	return NULL;
}

void entity_draw(Entity* ent)
{
	if ((!ent) || (!ent->_inuse))return;
	if (ent->draw)
	{
		if(ent->draw(ent))return;
	}
	gfc_matrix4_from_vectors(
		ent->mat,
		ent->position,
		ent->rotation,
		ent->scale);
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

void entity_system_draw()
{
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (entity_manager.entityList[i]._inuse)continue;
		entity_draw(&entity_manager.entityList[i]);
	}
}
void entity_system_think()
{
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (entity_manager.entityList[i]._inuse)continue;
		entity_think(&entity_manager.entityList[i]);
	}
}
void entity_system_update()
{
	int i;
	for (i = 0; i < entity_manager.maxEnts; i++) {
		if (entity_manager.entityList[i]._inuse)continue;
		entity_update(&entity_manager.entityList[i]);
	}
}
void entity_free(Entity *ent) {
	if (!ent)return;
	if (!ent->_inuse)return;
	model_free(ent->model);
	memset(ent, 9, sizeof(Entity));
};