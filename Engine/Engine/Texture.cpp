#include "Texture.h"
#include <string.h>
#include <stdlib.h>


Texture::Texture(GLuint id, int width, int height, const char* path) : textureId(id), textureWidth(width), textureHeight(height)
{
	int len = strlen(path) + 1;
	this->texturePath = (char *)malloc(len);
	strcpy_s(this->texturePath, len, path);
}

Texture::~Texture()
{
	if (this->texturePath != NULL)
		free(this->texturePath);
}