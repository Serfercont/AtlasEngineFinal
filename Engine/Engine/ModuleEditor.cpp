#include "ModuleEditor.h"
#include "App.h"

#include "ConsoleWindow.h"
#include "HierarchyWindow.h"
#include "InspectorWindow.h"
#include "ProjectWindow.h"

#include <windows.h>
#include <cstring>
#include <algorithm> 

ModuleEditor::ModuleEditor(App* app) : Module(app)
{}

ModuleEditor::~ModuleEditor()
{
    for (auto editorWindow : editorWindows) 
    {
        delete editorWindow;
    }
    editorWindows.clear();
}

bool ModuleEditor::Awake()
{
    LOG(LogType::LOG_INFO, "ModuleEditor");
    bool ret = true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowMenuButtonPosition = ImGuiDir_None;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(app->window->window, app->renderer3D->context);
    ImGui_ImplOpenGL3_Init();

	editorWindows.push_back(new HierarchyWindow(WindowType::HIERARCHY, "Hierarchy"));
	editorWindows.push_back(new InspectorWindow(WindowType::INSPECTOR, "Inspector"));
	editorWindows.push_back(new ConsoleWindow(WindowType::CONSOLE, "Console"));
	editorWindows.push_back(new ProjectWindow(WindowType::PROJECT, "Project"));

    return ret;
}

bool ModuleEditor::CleanUp()
{
    LOG(LogType::LOG_INFO, "Cleaning ModuleEditor");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}

void ModuleEditor::DrawEditor()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Bar
    MainMenuBar();

    // Docking    
    Docking();

	// Draw windows
    for (const auto& editorWindow : editorWindows)
    {
        if (editorWindow->IsEnabled())
            editorWindow->DrawWindow();
    }

	// Preferences
	PreferencesWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::PreferencesWindow()
{
    ImGui::Begin("Preferences");

    if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Show Textures", &drawTextures);

        ImGui::Spacing();
        ImGui::Separator();

        static int w = 0;
        for (int n = 0; n < 3; n++)
        {
            const char* names[] = { "Shaded", "Wireframe", "Shaded Wireframe" };

            if (ImGui::Selectable(names[n], w == n))
            {
                w = n;
                if (n == 0)
                {
                    wireframe = false;
                    shadedWireframe = false;
                }
                else if (n == 1)
                {
                    wireframe = true;
                    shadedWireframe = false;
                }
                else if (n == 2)
                {
                    wireframe = false;
                    shadedWireframe = true;
                }
            }
        }

		ImGui::Spacing();
		ImGui::Separator();

        ImGui::PushItemWidth(200.f);
        ImGui::SliderFloat("Vertex Normals Length", &vertexNormalLength, 0.05f, 0.25f, "%.2f", ImGuiSliderFlags_NoInput);
		ImGui::SliderFloat("Face Normals Length", &faceNormalLength, 0.05f, 0.25f, "%.2f", ImGuiSliderFlags_NoInput);
		ImGui::PopItemWidth();

        ImGui::ColorEdit3("Vertex Color", (float*)&vertexNormalColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::Text("Vertex Normal Color");

        ImGui::ColorEdit3("Face Color", (float*)&faceNormalColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::Text("Face Normal Color");
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Grid", ImGuiTreeNodeFlags_DefaultOpen))
    { 
        ImGui::ColorEdit4("Grid Color", app->renderer3D->grid.lineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

        ImGui::PushItemWidth(100.f);
	    ImGui::SliderFloat("Cell Size", &app->renderer3D->grid.cellSize, 1.f, 10.f, "%1.f");
        ImGui::PopItemWidth();

        float gridSizeOptions[] = { 50.f, 100.f, 150.f, 200.f };
        float currentOption = (app->renderer3D->grid.gridSize / 50.f);

        ImGui::PushItemWidth(100.f);
        if (ImGui::SliderFloat("Grid Size", &currentOption, 1, 4, "%1.f")) {
            app->renderer3D->grid.gridSize = gridSizeOptions[(int)currentOption - 1];
        }
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(100.f);
        ImGui::SliderFloat("Line Width", &app->renderer3D->grid.lineWidth, 1.f, 5.f, "%1.f");
        ImGui::PopItemWidth();

        static int selectedGrid = 1;
        for (int n = 0; n < 3; n++)
        {
            const char* names[] = { "X", "Y", "Z" };

            if (ImGui::Selectable(names[n], selectedGrid == n))
            {
                selectedGrid = n;
                if (n == 0)
                {
                    app->renderer3D->grid.normal = glm::vec3(1, 0, 0);
                }
                else if (n == 1)
                {
                    app->renderer3D->grid.normal = glm::vec3(0, 1, 0);
                }
                else if (n == 2)
                {
                    app->renderer3D->grid.normal = glm::vec3(0, 0, 1);
                }
            }
        }
    }

    ImGui::End();
}

void ModuleEditor::Docking()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Dockspace", NULL, windowFlags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspaceId = ImGui::GetID("Dockspace");
    ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void ModuleEditor::MainMenuBar()
{
    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Assets"))
    {
        if (ImGui::MenuItem("Show in Explorer"))
        {
            char buffer[MAX_PATH];
            GetModuleFileName(NULL, buffer, MAX_PATH);
            std::string::size_type pos = std::string(buffer).find_last_of("\\/");
            std::string exeDir = std::string(buffer).substr(0, pos);

            std::string path = exeDir + "\\..\\..\\Engine\\Assets";

            ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Windows"))
    {
        for (auto& editorWindow : editorWindows)
        {
            bool isEnabled = editorWindow->IsEnabled();
            if (ImGui::MenuItem(editorWindow->GetName().c_str(), NULL, &isEnabled))
            {
                editorWindow->SetEnabled(isEnabled);
            }
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}