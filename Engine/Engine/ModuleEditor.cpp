#include "ModuleEditor.h"
#include "App.h"

#include "GL/glew.h"

#include <iostream>
#include <windows.h>

#include <cstring>
#include <algorithm> 
#include <psapi.h> 

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

    if (ImGui::CollapsingHeader("System", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::TreeNode("GPU")) 
        {
            static float values[100];
            static int values_offset = 0;

            GLint memoryTotal = 0;
            glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &memoryTotal);

            GLint memoryAvailable = 0;
            glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &memoryAvailable);

            GLint memoryUse = memoryTotal - memoryAvailable;
            GLfloat memoryUsePercentage = (static_cast<GLfloat>(memoryUse) / static_cast<GLfloat>(memoryTotal)) * 100;

            values[values_offset] = memoryUsePercentage;

            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);

            ImGui::SeparatorText("Information");

            const GLubyte* vendor = glGetString(GL_VENDOR);
            ImGui::Text("Vendor:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%s", vendor);

            const GLubyte* renderer = glGetString(GL_RENDERER);
            ImGui::Text("GPU:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%s", renderer);

            const GLubyte* version = glGetString(GL_VERSION);
            ImGui::Text("OpenGL version supported:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%s", version);

            const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
            ImGui::Text("GLSL:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%s", glsl);

            ImGui::SeparatorText("VRam");

            ImGui::Text("GPU Memory:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%d MB", memoryTotal / 1024);

            ImGui::Text("Available GPU Memory:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%d MB", memoryAvailable / 1024);
      
            char overlay[32];
            sprintf_s(overlay, "Memory Usage %.2f %% (%d MB)", memoryUsePercentage, memoryUse / 1024);
            ImGui::PlotLines("##MemoryUsage", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 100.0f, ImVec2(0, 80.0f));

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("CPU"))
        {
            ImGui::SeparatorText("Information");
            std::string cpuName;

            WCHAR buffer[256];
            DWORD bufferSize = sizeof(buffer);
            HKEY key;

            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);

            if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &key) == ERROR_SUCCESS) {
                if (RegQueryValueExW(key, L"ProcessorNameString", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) {
                    int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
                    cpuName.resize(size_needed);
                    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &cpuName[0], size_needed, nullptr, nullptr);
                }
                RegCloseKey(key);
            }
            ImGui::Text("CPU Name:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%s", cpuName.c_str());

            ImGui::Text("Total Number of Processors:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%d Cores", sysInfo.dwNumberOfProcessors);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("MEMORY"))
        {

            ImGui::SeparatorText("Information");
            MEMORYSTATUSEX statex;
            statex.dwLength = sizeof(statex);
            GlobalMemoryStatusEx(&statex);

            ImGui::Text("Total Memory:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%d MB", statex.ullTotalPhys / (1024 * 1024));

            ImGui::Text("Free Memory:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%d MB", statex.ullAvailPhys / (1024 * 1024));

            int memoryusage = (statex.ullTotalPhys - statex.ullAvailPhys) / (1024 * 1024);
            ImGui::Text("Used Memory:");
            ImGui::SameLine();
            ImGui::TextColored(dataTextColor, "%d MB", (statex.ullTotalPhys - statex.ullAvailPhys) / (1024 * 1024));

            float memoryUsePercentage = ((float)(statex.ullTotalPhys - statex.ullAvailPhys) / statex.ullTotalPhys) * 100.0f;

            static float totalMemoryValues[100] = { 0 };
            static int totalValuesOffset = 0;

            totalMemoryValues[totalValuesOffset] = memoryUsePercentage;
            totalValuesOffset = (totalValuesOffset + 1) % IM_ARRAYSIZE(totalMemoryValues);

            char totalOverlay[32];
            sprintf_s(totalOverlay, "Memory Usage %.2f %% (%d MB)", memoryUsePercentage, memoryusage);

            ImGui::PlotLines("##MemoryUsage", totalMemoryValues, IM_ARRAYSIZE(totalMemoryValues), totalValuesOffset, totalOverlay, 0.0f, 100.0f, ImVec2(0, 80.0f));

            PROCESS_MEMORY_COUNTERS_EX pmc;
            if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                SIZE_T privateMemoryUsage = pmc.PrivateUsage;
                float privateMemoryUsageMB = static_cast<float>(privateMemoryUsage) / (1024.0f * 1024.0f);

                ImGui::Text("Engine Memory Usage:");
                ImGui::SameLine();
                ImGui::TextColored(dataTextColor, "%.2f MB", privateMemoryUsageMB);

                static float privateMemoryValues[100] = { 0 }; 
                static int privateValuesOffset = 0;

                privateMemoryValues[privateValuesOffset] = privateMemoryUsageMB;
                privateValuesOffset = (privateValuesOffset + 1) % IM_ARRAYSIZE(privateMemoryValues);

            }

            ImGui::TreePop();
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

    ImGui::EndMainMenuBar();
}