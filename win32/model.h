#ifndef __MODEL_H__
#define __MODEL_H__
#include "simple_logger.h"

#include "gf3d_pipeline.h"
#include "gf3d_mesh.h"

typedef struct
{
	int				_refCount; //Number of entities to draw this
	Texture*		texture;
	Mesh*			mesh;
	GFC_TextLine	fileName;
}Model;


//UBO stuff, ask about this since it confuses me a little
typedef struct
{
	GFC_Matrix4		model;
	GFC_Matrix4		view;
	GFC_Matrix4		proj;
	GFC_Vector4D	color;
}ModelUBO;




void model_init_system(Uint32 modelCount);

Pipeline* model_get_pipeline();


void gf3d_model_queue_render(Model* model, GFC_Matrix4 mat, GFC_Color colormod);




void model_free(Model model);
void model_delete(Model model);
Model* model_load(const char* filename);
Model* model_get_by_filename(const char* filename);


#endif