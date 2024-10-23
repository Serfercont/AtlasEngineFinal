#include "ModuleEditor.h"
#include "App.h"

#include <windows.h>
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

    ImGui::InputText("##Search", searchInput, 256);

    HierarchyTree(app->scene->root, true, searchInput);

    ImGui::End();
}

void ModuleEditor::HierarchyTree(GameObject* node, bool isRoot, const char* searchText)
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
    
    if (!node->isActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }

    if (FilterNode(node, searchText))
    {
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

        // Rename node
        if (node->isEditing)
        {
            strcpy_s(inputName, selectedGameObject->name.c_str());
            ImGui::SetNextItemWidth(ImGui::CalcTextSize(node->name.c_str()).x + 100);
            if (ImGui::InputText("##edit", inputName, sizeof(inputName), inputTextFlags)
                || (!ImGui::IsItemActive() && !ImGui::IsAnyItemActive()))
            {
                if (inputName[0] != '\0') node->name = inputName;
                node->isEditing = false;
            }

            ImGui::SetKeyboardFocusHere(-1);
        }

        // Create child nodes
        if (isOpen && !node->children.empty())
        {
            for (unsigned int i = 0; i < node->children.size(); i++)
            {
                HierarchyTree(node->children[i], false, searchText);
            }
            ImGui::TreePop();
        }

        if (!node->isActive)
        {
            ImGui::PopStyleColor();
        }
    }
    else
    {
        for (unsigned int i = 0; i < node->children.size(); i++)
        {
            HierarchyTree(node->children[i], false, searchText);
        }
    }
}

bool ModuleEditor::FilterNode(GameObject* node, const char* searchText)
{
    std::string nodeNameLower = node->name;
    std::transform(nodeNameLower.begin(), nodeNameLower.end(), nodeNameLower.begin(), ::tolower);

    std::string searchTextLower = searchText;
    std::transform(searchTextLower.begin(), searchTextLower.end(), searchTextLower.begin(), ::tolower);

    return nodeNameLower.find(searchTextLower) != std::string::npos;
}

void ModuleEditor::InspectorWindow()
{
    ImGui::Begin("Inspector");

    if (selectedGameObject != nullptr && selectedGameObject->parent != nullptr)
    {
        ImGui::Checkbox("##Active", &selectedGameObject->isActive);
        ImGui::SameLine();

        strcpy_s(inputName, selectedGameObject->name.c_str());

        if (ImGui::InputText("##InspectorName", inputName, sizeof(inputName), inputTextFlags)
            || (isEditingInspector && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive()))
        {
            if (inputName[0] != '\0') selectedGameObject->name = inputName;
            isEditingInspector = false;
        }

        if (ImGui::IsItemClicked())
        {
            isEditingInspector = true;
            ImGui::SetKeyboardFocusHere(-1);
        }

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

    ImGui::PushStyleColor(ImGuiCol_CheckMark, infoColor);
    ImGui::PushStyleColor(ImGuiCol_Text, infoColor);
    ImGui::Checkbox("Info", &showLogInfo);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_CheckMark, warningColor);
    ImGui::PushStyleColor(ImGuiCol_Text, warningColor);
    ImGui::Checkbox("Warning", &showLogWarnings);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_CheckMark, errorColor);
    ImGui::PushStyleColor(ImGuiCol_Text, errorColor);
    ImGui::Checkbox("Error", &showLogErrors);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        logger.Clear();

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 10));

	string logType;
	ImVec4 logColor;

    for (const auto& log : logger.GetLogs())
    {
        switch (log.type)
        {
        case LogType::LOG_INFO:
            logType = "[INFO]";
            logColor = infoColor;
            if (!showLogInfo) continue;
            break;

        case LogType::LOG_WARNING:
            logType = "[WARNING]";
            logColor = warningColor;
            if (!showLogWarnings) continue;
            break;

        case LogType::LOG_ERROR:
            logType = "[ERROR]";
            logColor = errorColor;
            if (!showLogErrors) continue;
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
                    app->renderer3D->grid.normal = vec3(1, 0, 0);
                }
                else if (n == 1)
                {
                    app->renderer3D->grid.normal = vec3(0, 1, 0);
                }
                else if (n == 2)
                {
                    app->renderer3D->grid.normal = vec3(0, 0, 1);
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
    

    ImGui::EndMainMenuBar();
}