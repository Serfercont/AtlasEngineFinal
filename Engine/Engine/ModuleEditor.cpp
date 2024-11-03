#include "ModuleEditor.h"
#include "App.h"

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

	hierarchyWindow = new HierarchyWindow(WindowType::HIERARCHY, "Hierarchy");
	editorWindows.push_back(hierarchyWindow);
	inspectorWindow = new InspectorWindow(WindowType::INSPECTOR, "Inspector");
	editorWindows.push_back(inspectorWindow);
	consoleWindow = new ConsoleWindow(WindowType::CONSOLE, "Console");
	editorWindows.push_back(consoleWindow);
	projectWindow = new ProjectWindow(WindowType::PROJECT, "Project");
	editorWindows.push_back(projectWindow);
	performanceWindow = new PerformanceWindow(WindowType::PERFORMANCE, "Performance");
	editorWindows.push_back(performanceWindow);
	preferencesWindow = new PreferencesWindow(WindowType::PREFERENCES, "Preferences");
	editorWindows.push_back(preferencesWindow);

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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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