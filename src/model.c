
#include "gf3d_buffers.h"
#include "gf3d_vgraphics.h"
#include "gf3d_swapchain.h"
#include "model.h"

//Add model manager here
typedef struct
{
	Model* model_list;
	Uint32 model_count;
	Pipeline* pipe;
	Uint32 chainLength;
	VkDevice device;
	Texture* defaultTexture;
}ModelManager;

static ModelManager model_manager = { 0 };

void model_system_init(Uint32 model_count) {

	if (!model_count)
	{
		slog("need model count to be nonzero");
		return;
	}
	//if model manager already exists
	if (model_manager.model_count != 0)
	{
		slog("Model manager already exists.");
		return;
	}


	Uint32 count;
	//model_manager.chainLength = gf3d_swapchain_get_chain_length();
	model_manager.model_list = (Model*) gfc_allocate_array(sizeof(Model), model_count);

	gf3d_mesh_init(1024);
	gf3d_mesh_get_attribute_descriptions(&count);
	slog("got attribute descriptions");

	model_manager.model_count = model_count;
	slog("about to set pipe from config");
	model_manager.pipe = gf3d_pipeline_create_from_config(
		gf3d_vgraphics_get_default_logical_device(),
		"config/model_pipeline.cfg",
		gf3d_vgraphics_get_view_extent(),
		model_count,
		gf3d_mesh_get_bind_description(),
		gf3d_mesh_get_attribute_descriptions(NULL),
		count, //ask about this one
		sizeof(ModelUBO),
		VK_INDEX_TYPE_UINT32
	);

	slog("pipe set from config");

	model_manager.device = gf3d_vgraphics_get_default_logical_device();
	model_manager.defaultTexture = gf3d_texture_load("images/default.png");



	slog("model system initiated");

}

Pipeline* model_get_pipeline()
{
	return model_manager.pipe;
}

ModelUBO model_get_ubo(GFC_Matrix4 modelMat, GFC_Color colormod)
{
	ModelUBO ubo = { 0 };
	GFC_Matrix4 view;
	GFC_Matrix4 projection;


	memcpy(ubo.model, modelMat, sizeof(GFC_Matrix4));

	gf3d_vgraphics_get_view(&view);

	memcpy(ubo.view, view, sizeof(GFC_Matrix4));

	gf3d_vgraphics_get_projection_matrix(&projection);
	memcpy(ubo.proj, projection, sizeof(GFC_Matrix4));
	ubo.color = gfc_color_to_vector4f(colormod);


	return ubo;

}
void gf3d_model_queue_render(Model* model, GFC_Matrix4 mat, GFC_Color colormod)
{
	if (!model)
	{
		slog("model missing from model queue render");
		return;
	}
	ModelUBO ubo = model_get_ubo(mat, colormod);
	gf3d_mesh_queue_render(model->mesh, model_manager.pipe, &ubo, model->texture);
}




void model_free(Model* model)
{
	//don't actually delete the model here in case another entity is using it
	if (model->_refCount == 0)
	{
		slog("model is already not being used???? let's free it anyway");
		//idk if this is necessary but i'd imagine we could end up here if something goes wrong
	}
	else
	{
		model->_refCount--;
	}
	
	if (model->_refCount == 0)
	{
		model_delete(model);
	}
};


void model_delete(Model* model)
{
	if (!model)return;
	gf3d_texture_free(model->texture);
	gf3d_mesh_free(model->mesh);
	
	memset(model, 0, sizeof(model));
};

void model_close() {

	gf3d_texture_free(model_manager.defaultTexture);
	int i;
	for (i = 0; i < model_manager.model_count; i++)
	{
		model_free(&model_manager.model_list[i]);
	}
	//set count back to 0 so we can make a new model manager later if necessary
	model_manager.model_count = 0;
}


Model* model_new() {
	slog("MAKING A NEW MODEL!!!!1!!1111");
	int i;
	for (i = 0; i < model_manager.model_count; i++)
	{
		if ((model_manager.model_list[i]._refCount == 0))// && (strlen(model_manager.model_list[i].fileName) == 0))
		{
			//found a valid model slot
			//model_delete(&model_manager.model_list[i]);
			model_manager.model_list[i]._refCount = 1;
			return &model_manager.model_list[i];
		}
		//need a different one in case these aren't initialized, i guess?
		if ((!model_manager.model_list[i]._refCount))
		{ 
			//model_delete(&model_manager.model_list[i]);
			model_manager.model_list[i]._refCount = 1;
			return &model_manager.model_list[i];
		}
		slog("looped, found no model space");
	}
	slog("no open model space");
	return NULL;
}
Model* model_get_by_filename(const char* filename) {
	if (!filename)return NULL;
	for (int i = 0; i < model_manager.model_count; i++)
	{
		if ((model_manager.model_list[i]._refCount == 0) ||
			(model_manager.model_list[i].fileName == NULL))
		{
			continue;
		}

		if (gfc_strlcmp(filename, model_manager.model_list[i].fileName))
		{
			return(&model_manager.model_list[i]);
		}


	}
	return NULL;
}
Model* model_load(const char* filename)
{

	const char* str = NULL;
	const char* str2 = NULL;
	SJson* json;
	SJson* data;
	Model* model;
	Mesh* mesh;
	Texture* texture;

	if (!filename)return NULL;

	model = model_get_by_filename(filename);

	//are we already using this model?
	if (model)
	{
		slog("Model already exists with this filename!");
		model->_refCount++;
		return model;
	}


	json = sj_load(filename);
	if (!json)
	{
		slog("failed to load json model");
		return NULL;
	}
	slog("found the json info for this model");

	data = sj_object_get_value(json, "model");
	if (!json)
	{
		slog("failed to load json model data");
		return NULL;
	}
	slog("found the json data info for this model");
	str = sj_object_get_string(data, "obj");
	mesh = gf3d_mesh_load_obj(str);


	slog("succeeded at gf3d_mesh_load_obj");
	if (!mesh)
	{
		slog("failed to load mesh");
		sj_free(json);
		return NULL;
	}

	str2 = sj_object_get_string(data, "texture");
	if (str2)
	{
		texture = gf3d_texture_load(str2);
		if (!texture)
		{
			slog("failed to load texture");
			texture = model_manager.defaultTexture;
		}
		else
		{
			slog("texture was loaded yahoooooo");
		}
	}
	else
	{
		slog("failed to find a texture string. using default texture");
		texture = model_manager.defaultTexture;
		
	}

	model = model_new();
	if (!model)
	{
		slog("model was not loaded correctly, memleaks");
	}
	model->mesh = mesh;
	if (!model->mesh)
	{
		slog("mesh was not pointered correctly");
	}
	model->texture = texture;
	if (!model->texture)
	{
		slog("texture was not pointered correctly");
	}
	gfc_line_cpy(model->fileName, filename);
	slog("returning model:");
	slog(filename);
	return model;
}




