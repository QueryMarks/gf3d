#include "level.h"
#include "gf3d_obj_load.h"


Uint8 level_line_test(Level* level, GFC_Edge3D edge, GFC_Vector3D *contact)
{
	MeshPrimitive* prim;
	GFC_Triangle3D t;
	int i, c;

	GFC_Vector3D out = { -1 };
	if ((!level)||(!level->terrain)||(!level->terrain->mesh)) return -1;

	c = gfc_list_count(level->terrain->mesh->primitives);
	for (i = 0; i < c; i++)
	{
		prim = gfc_list_nth(level->terrain->mesh->primitives, i);
		if (!prim)continue;
		if (!prim->objData)continue;
		for (int j = 0; j < prim->faceCount; j++) {
			t.a = prim->objData->faceVertices[prim->objData->outFace->verts[0]].vertex;
			t.b = prim->objData->faceVertices[prim->objData->outFace->verts[1]].vertex;
			t.c = prim->objData->faceVertices[prim->objData->outFace->verts[2]].vertex;
			if (gfc_triangle_edge_test(edge, t, &out))
			{
				if (contact)*contact = out;
				return 1;
			}
		}
	}
	return 0;



	
}

Level* level_new()
{
	Level* level;
	level = gfc_allocate_array(sizeof(Level), 1);
	//gfc_matrix4_identity(level->terrainMatrix);    ????

	return level;
}

Level* level_load(const char* filename) {
	SJson* json;
	SJson* data;
	const char* str;
	Level* level;
	level = level_new();
	json = sj_load(filename);
	data = sj_object_get_value(json, "level");

	if (!data)return NULL;

	str = sj_object_get_value(json, "sky");
	if (str) {
		level->sky = model_load(str);
		if (level->sky)gfc_line_cpy(level->skyFile, str);
	}
	str = sj_object_get_value(json, "terrain");
	if (str) {
		level->terrain = model_load(str);
		if (level->terrain)gfc_line_cpy(level->terrainFile, str);
	}


	level->bounds = gfc_box_from_config(sj_object_get_value(json, "bounds"));
	level = level_new();

	str = sj_object_get_value(data, "music");
	if (str) {
		//level->backgroundMusic = gfc_sound_load_music(str);
		if (level->terrain)gfc_line_cpy(level->musicFile, str);
	}

}

void level_free(Level* level)
{
	if (!level)return;
	model_free(level->sky);
	model_free(level->terrain);
	Mix_HaltMusic();
	//Mix_FreeMusic(level->backgroundMusic);
}