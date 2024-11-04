#include "PerformanceWindow.h"

#include <psapi.h> 

PerformanceWindow::PerformanceWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
    vendor = glGetString(GL_VENDOR);
    renderer = glGetString(GL_RENDERER);
    version = glGetString(GL_VERSION);
    glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);

    WCHAR buffer[256];
    DWORD bufferSize = sizeof(buffer);
    HKEY key;

    GetSystemInfo(&sysInfo);

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &key) == ERROR_SUCCESS) 
    {
        if (RegQueryValueExW(key, L"ProcessorNameString", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) 
        {
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
            cpuName.resize(size_needed);
            WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &cpuName[0], size_needed, nullptr, nullptr);
        }
        RegCloseKey(key);
    }
}

PerformanceWindow::~PerformanceWindow()
{
}

void PerformanceWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

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

        ImGui::Text("Vendor:");
        ImGui::SameLine();
        ImGui::TextColored(dataTextColor, "%s", vendor);

        ImGui::Text("GPU:");
        ImGui::SameLine();
        ImGui::TextColored(dataTextColor, "%s", renderer);

        ImGui::Text("OpenGL version supported:");
        ImGui::SameLine();
        ImGui::TextColored(dataTextColor, "%s", version);

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
        sprintf_s(overlay, "Memory Usage %.2f%% (%d MB)", memoryUsePercentage, memoryUse / 1024);
        ImGui::PlotLines("##MemoryUsage", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 100.0f, ImVec2(0, 80.0f));

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("CPU"))
    {
        ImGui::SeparatorText("Information");
        
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

        float memoryUsePercentage = ((float)(statex.ullTotalPhys - statex.ullAvailPhys) / statex.ullTotalPhys) * 100.0f;

        static float totalMemoryValues[100] = { 0 };
        static int totalValuesOffset = 0;

        totalMemoryValues[totalValuesOffset] = memoryUsePercentage;
        totalValuesOffset = (totalValuesOffset + 1) % IM_ARRAYSIZE(totalMemoryValues);

        char totalOverlay[32];
        sprintf_s(totalOverlay, "Memory Usage %.2f%% (%d MB)", memoryUsePercentage, memoryusage);

        ImGui::PlotLines("##MemoryUsage", totalMemoryValues, IM_ARRAYSIZE(totalMemoryValues), totalValuesOffset, totalOverlay, 0.0f, 100.0f, ImVec2(0, 80.0f));

        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) 
        {
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

	ImGui::End();
}