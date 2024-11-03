#include "ModuleEditor.h"
#include "App.h"

#include "GL/glew.h"

#include <iostream>
#include <windows.h>

#include "ConsoleWindow.h"
#include "HierarchyWindow.h"
#include "InspectorWindow.h"
#include "ProjectWindow.h"

#include <cstring>
#include <algorithm> 
#include <psapi.h> 

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
    if (ImGui::BeginMenu("GameObject"))
    {
	    if (ImGui::MenuItem("Create Empty"))
	    {
		    app->scene->CreateGameObject("GameObject", app->scene->root);
	    }
        if (ImGui::BeginMenu("3D Object"))
        {
            const char* objectNames[] = { "Cube", "Sphere", "Capsule", "Cylinder" };
            const char* basePath = "Assets/Models/Primitives/";
            const char* extension = ".fbx";

            for (const char* name : objectNames)
            {
                std::string fullPath = std::string(basePath) + name + extension;

                if (ImGui::MenuItem(name))
                {
                    app->renderer3D->meshLoader.ImportFBX(fullPath.c_str(), app->scene->root);
                    selectedGameObject = app->scene->root->children.back();
                }
            }

            ImGui::EndMenu();
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