#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_audio.h"
#include "SDL_audio.h"

#include "model.h"


typedef struct
{
	Model* sky;
	GFC_TextLine skyFile;
	Model* terrain;
	GFC_TextLine terrainFile;
	GFC_Box bounds;
	GFC_TextLine musicFile;
	//Mix_Music backgroundMusic;


}Level;


#endif