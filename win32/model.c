Model* model_load(const char* filename)
{
	const char* str = NULL;
	SJson* json* data;
	Model* model;
	if (!filename)return NULL;
	model = model_get_by_filename(filename);
	if (model)
	{
		model->_refcount++;
		return model
	}
	json = sj_load(filename);
	if (!json)
	{
		slog("failed to load json model");
		return NULL;
	}

}