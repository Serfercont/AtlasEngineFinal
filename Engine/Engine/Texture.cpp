#include "Texture.h"

Texture::Texture(GLuint id, int width, int height, const char* path) : textureId(id), textureWidth(width), textureHeight(height), texturePath(path)
{
}

Texture::~Texture()
{
}