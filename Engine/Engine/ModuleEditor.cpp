#include "ModuleEditor.h"
#include "App.h"

#include <cstring>
#include <algorithm> 

ModuleEditor::ModuleEditor(App* app) : Module(app)
{}

ModuleEditor::~ModuleEditor()
{}

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

    // Hierarchy
    HierarchyWindow();

    // Project
    ProjectWindow();

    // Inspector
    InspectorWindow();

    // Console
    ConsoleWindow();

	// Preferences
	PreferencesWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::HierarchyWindow()
{
    ImGui::Begin("Hierarchy");

    HierarchyTree(app->scene->root, true);

    ImGui::End();
}

void ModuleEditor::HierarchyTree(GameObject* node, bool isRoot)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (isRoot)
    {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    if (node->children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool isSelected = (selectedGameObject == node);

    if (isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    bool isOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());

    if (ImGui::IsItemClicked())
    {
        if (selectedGameObject && selectedGameObject->isEditing)
        {
            selectedGameObject->isEditing = false;
        }
        selectedGameObject = node;
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        node->isEditing = true;
    }

    // Create child nodes
    if (isOpen && !node->children.empty())
    {
        for (unsigned int i = 0; i < node->children.size(); i++)
        {
            HierarchyTree(node->children[i], false);
        }
        ImGui::TreePop();
    }
}

void ModuleEditor::InspectorWindow()
{
    ImGui::Begin("Inspector");

    if (selectedGameObject != nullptr && selectedGameObject->parent != nullptr)
    {
        ImGui::Checkbox("##Active", &selectedGameObject->isActive);

        for (auto i = 0; i < selectedGameObject->components.size(); i++)
        {
            selectedGameObject->components[i]->OnEditor();
        }
    }

    ImGui::End();
}

void ModuleEditor::ConsoleWindow()
{
    ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 15));

	string logType;
	ImVec4 logColor;

    for (const auto& log : logger.GetLogs())
    {
        switch (log.type)
        {
        case LogType::LOG_INFO:
            logType = "[INFO]";
            logColor = ImVec4(1, 1, 1, 1);
            break;

        case LogType::LOG_WARNING:
            logType = "[WARNING]";
            logColor = ImVec4(1, 1, 0, 1);
            break;

        case LogType::LOG_ERROR:
            logType = "[ERROR]";
            logColor = ImVec4(1, 0, 0, 1);
            break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, logColor);
        ImGui::Text(logType.c_str());
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::Text(log.message.c_str());
    }

	ImGui::PopStyleVar();

    ImGui::End();
}

void ModuleEditor::ProjectWindow()
{
    ImGui::Begin("Project");

    ImGui::End();
}

void ModuleEditor::PreferencesWindow()
{
    ImGui::Begin("Preferences");

    ImGui::ColorEdit4("Grid Color", app->renderer3D->grid.lineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions);

    ImGui::PushItemWidth(100);
	ImGui::SliderFloat("Cell Size", &app->renderer3D->grid.cellSize, 1.0f, 10.0f, "%1.0f");
    ImGui::PopItemWidth();

    float gridSizeOptions[] = { 50.0f, 100.0f, 150.0f, 200.0f };
    float gridSizeIncrements = app->renderer3D->grid.gridSize;
    float currentOption = (gridSizeIncrements / 50.0f) - 1;

    ImGui::PushItemWidth(100);
    if (ImGui::SliderFloat("Grid Size", &currentOption, 0, 3, "%1.0f")) {
        app->renderer3D->grid.gridSize = gridSizeOptions[(int)currentOption];
    }
    ImGui::PopItemWidth();

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

    ImGui::EndMainMenuBar();
}