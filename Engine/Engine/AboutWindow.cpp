#include "AboutWindow.h"

AboutWindow::AboutWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

	UpdateMouseState();

    // Title
    ImGui::Text("SpectraEngine");
    ImGui::Separator();

    // Description
    ImGui::Text("Description");
    ImGui::TextWrapped("SpectraEngine is a 3D game engine developed in C++ with OpenGL. It can render geometry with controls and information displays.");
    ImGui::TextWrapped("GitHub: ");
	ImGui::SameLine();
    ImGui::TextWrapped("https://github.com/CITM-UPC/SpectraEngine");

    ImGui::Separator();

    // Team Members
    ImGui::Text("Team Members");
    ImGui::BulletText("Guillem Alqueza Mayench: https://github.com/guillemalqueza");
    ImGui::BulletText("Miguel Iglesias Abarca: https://github.com/MiguelIglesiasAbarca");
    ImGui::BulletText("Lluc Cid: https://github.com/Lluccib");

    ImGui::Separator();

    // Viewer Controls
    ImGui::Text("Viewer Controls");
    ImGui::BulletText("Right-click: Move the current view.");
    ImGui::BulletText("Right-click + WASD: Fly around the scene.");
    ImGui::BulletText("Alt + Right-click or Mouse Wheel: Zoom in or out.");
    ImGui::BulletText("Alt + Left-click: Orbit the camera around a pivot point.");
    ImGui::BulletText("Alt + Mouse Wheel: Pan the camera.");
    ImGui::BulletText("F: Frame the selected object.");

    ImGui::Separator();

    // Libraries and Dependencies
    ImGui::Text("Libraries and Dependencies");
    ImGui::BulletText("SDL2:");
    ImGui::SameLine();
    ImGui::TextWrapped("https://github.com/libsdl-org/SDL");
    ImGui::BulletText("OpenGL:");
    ImGui::SameLine();
    ImGui::TextWrapped("https://www.opengl.org/");
    ImGui::BulletText("ImGui:");
    ImGui::SameLine();
    ImGui::TextWrapped("https://github.com/ocornut/imgui");
    ImGui::BulletText("GLM:");
    ImGui::SameLine();
    ImGui::TextWrapped("https://github.com/g-truc/glm");
    ImGui::BulletText("Assimp:");
    ImGui::SameLine();
    ImGui::TextWrapped("https://github.com/assimp/assimp");
    ImGui::BulletText("DeviL:");
    ImGui::SameLine();
    ImGui::TextWrapped("https://github.com/DentonW/DevIL");

    ImGui::End();
}