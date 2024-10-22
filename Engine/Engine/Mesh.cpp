#include "Mesh.h"

void Mesh::InitMesh()
{
	//Vertices
	glGenBuffers(1, (GLuint*)&(verticesId));
	glBindBuffer(GL_ARRAY_BUFFER, verticesId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesCount * 3, vertices, GL_STATIC_DRAW);

	//Indices
	glGenBuffers(1, (GLuint*)&(indicesId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indicesCount, indices, GL_STATIC_DRAW);

	//Normals
	glGenBuffers(1, (GLuint*)&(normalsId));
	glBindBuffer(GL_ARRAY_BUFFER, normalsId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalsCount * 3, &normals[0], GL_STATIC_DRAW);

	//Texture Coords
	glGenBuffers(1, (GLuint*)&(texCoordsId));
	glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoordsCount * 2, &texCoords[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::DrawMesh(GLuint textureID, bool drawTextures, bool wireframe, bool shadedWireframe)
{
	if (!shadedWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (drawTextures && !wireframe && textureID != 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}
	else
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, verticesId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (normalsCount > 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, normalsId);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}
	else
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (shadedWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3f(0.0f, 1.0f, 0.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, verticesId);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, NULL);

		glPopAttrib();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void Mesh::CleanUpMesh()
{
	glDeleteBuffers(1, &verticesId);
	glDeleteBuffers(1, &indicesId);
	glDeleteBuffers(1, &normalsId);
	glDeleteBuffers(1, &texCoordsId);

	delete[] vertices;
	delete[] indices;
	delete[] normals;
	delete[] texCoords;

	vertices = nullptr;
	indices = nullptr;
	normals = nullptr;
	texCoords = nullptr;
}