#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include <SDL2/SDL_events.h>
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "Mesh.h"

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(16 * 80, 10 * 80);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

float rotationAngleX = 0.0f;
float rotationAngleY = 0.0f;

float scale = 0.05f;

bool keyUpPressed = false;
bool keyDownPressed = false;
bool keyLeftPressed = false;
bool keyRightPressed = false;

#define CHECKERS_WIDTH 128*2
#define CHECKERS_HEIGHT 128*2

GLubyte checkerImage[CHECKERS_WIDTH][CHECKERS_HEIGHT][4];
uint textureId;

std::vector<Mesh*> mesh;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)WINDOW_SIZE.x / (double)WINDOW_SIZE.y, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1.5, 1.5, 1.5,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

static void DrawCube() {
	glLineWidth(2.0f);

	float vertices[8][3] = {
		{0.0f, 0.0f, 0.0f}, // v0
		{0.0f, 0.3f, 0.0f}, // v1
		{0.3f, 0.3f, 0.0f}, // v2
		{0.3f, 0.0f, 0.0f}, // v3
		{0.0f, 0.0f, 0.3f}, // v4
		{0.0f, 0.3f, 0.3f}, // v5
		{0.3f, 0.3f, 0.3f}, // v6
		{0.3f, 0.0f, 0.3f}  // v7
	};

	glBegin(GL_TRIANGLES);

	// Frontal
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[2]);

	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[3]);

	// Derecha
	glColor3f(0.0f, 1.0f, 0.0f);

	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);

	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[7]);
	glVertex3fv(vertices[3]);

	// Trasera
	glColor3f(0.0f, 0.0f, 1.0f);

	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[4]);

	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[7]);

	// Izquierda
	glColor3f(1.0f, 0.0f, 1.0f);

	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);

	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[5]);

	// Superior
	glColor3f(0.0f, 1.0f, 1.0f);

	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[6]);

	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[2]);

	// Inferior
	glColor3f(1.0f, 1.0f, 0.0f);

	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[3]);

	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[3]);
	glVertex3fv(vertices[7]);

	glEnd();
}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (keyUpPressed) {
		rotationAngleX -= 1.0f;
	}
	if (keyDownPressed) {
		rotationAngleX += 1.0f;
	}
	if (keyLeftPressed) {
		rotationAngleY -= 1.0f;
	}
	if (keyRightPressed) {
		rotationAngleY += 1.0f;
	}


	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(1.5, 1.5, 1.5,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	glTranslatef(0.15f, 0.15f, 0.15f);
	glRotatef(rotationAngleX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotationAngleY, 0.0f, 1.0f, 0.0f);
	glTranslatef(-0.15f, -0.15f, -0.15f);

	//DrawCube();

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		glScaled(scale, scale, scale);
		mesh[i]->DrawMesh();
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Menu")) {
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImportFBX(const char* path, std::vector<Mesh*>& meshes, int textureId)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (!scene) {
		fprintf(stderr, "Error en carregar el fitxer: %s\n", aiGetErrorString());
	}

	if (scene != nullptr && scene->HasMeshes())
	{
		aiMesh* newMesh = nullptr;
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			newMesh = scene->mMeshes[i];
			Mesh* mesh = new Mesh();
			// Vertices
			mesh->verticesCount = newMesh->mNumVertices;
			mesh->vertices = new float[mesh->verticesCount * 3];
			memcpy(mesh->vertices, newMesh->mVertices, sizeof(float) * mesh->verticesCount * 3);
			// Faces
			if (newMesh->HasNormals())
			{
				mesh->normalsCount = mesh->verticesCount;
				mesh->normals = new float[mesh->normalsCount * 3];
				memcpy(mesh->normals, newMesh->mNormals, sizeof(float) * mesh->normalsCount * 3);
			}
			if (newMesh->HasFaces())
			{
				mesh->indicesCount = newMesh->mNumFaces * 3;
				mesh->indices = new uint[mesh->indicesCount];
				for (uint i = 0; i < newMesh->mNumFaces; ++i)
				{
					if (newMesh->mFaces[i].mNumIndices != 3)
					{
					}
					else
					{
						memcpy(&mesh->indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			if (newMesh->HasTextureCoords(0))
			{
				mesh->texCoordsCount = newMesh->mNumVertices;
				mesh->texCoords = new float[newMesh->mNumVertices * 2];
				for (unsigned int i = 0; i < mesh->texCoordsCount; i++)
				{
					mesh->texCoords[i * 2] = newMesh->mTextureCoords[0][i].x;
					mesh->texCoords[i * 2 + 1] = newMesh->mTextureCoords[0][i].y;
				}
				mesh->textureID = textureId;
			}

			mesh->InitMesh();
			meshes.push_back(mesh);
		}
		aiReleaseImport(scene);
	}
}

void LoadTextureImage(const char* file)
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage((const wchar_t*)file);
	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		mesh[i]->textureID = ilutGLBindTexImage();
	}
	ilDeleteImages(1, &image);
}

static bool processEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_UP) {
				keyUpPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				keyDownPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_LEFT) {
				keyLeftPressed = true;
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				keyRightPressed = true;
			}

			if (event.key.keysym.sym == SDLK_q) {
				scale += 0.01f;
			}
			else if (event.key.keysym.sym == SDLK_w) {
				scale -= 0.01f;
			}
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_UP) {
				keyUpPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				keyDownPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_LEFT) {
				keyLeftPressed = false;
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				keyRightPressed = false;
			}
			break;
		case (SDL_DROPFILE):
		{
			std::string droppedFileDir(event.drop.file);

			if (droppedFileDir.substr(droppedFileDir.find(".") + 1) == "png")
			{
				LoadTextureImage(event.drop.file);
			}

			SDL_free(event.drop.file);
			break;
		}
		default:
			ImGui_ImplSDL2_ProcessEvent(&event);
			break;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

	init_openGL();

	ImportFBX("LaserGun_P1.fbx", mesh, textureId);

	while (processEvents()) {
		const auto t0 = hrclock::now();

		display_func();

		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}

	return 0;
}