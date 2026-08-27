#include "ui_dashboard.h"
#include <cstdio>
#include <sstream>
#include <iomanip>

namespace SysMon {

UIDashboard::UIDashboard(SystemMonitor& monitor)
    : m_monitor(monitor) {
    m_processFilterBuf[0] = '\0';
    m_lastRefreshTime = std::chrono::steady_clock::now();
    SetupDarkTheme();
}

void UIDashboard::SetupDarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text]                  = ImVec4(0.92f, 0.95f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.48f, 0.52f, 0.58f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.11f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.14f, 0.18f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.20f, 0.24f, 0.30f, 0.60f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.22f, 0.27f, 0.35f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.32f, 0.42f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.11f, 0.14f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.09f, 0.11f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.22f, 0.27f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.30f, 0.38f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.00f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.00f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.00f, 0.65f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.18f, 0.23f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.25f, 0.33f, 0.44f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.00f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.18f, 0.23f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.25f, 0.33f, 0.44f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.00f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.24f, 0.30f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.00f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.00f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.18f, 0.23f, 0.30f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.00f, 0.60f, 0.90f, 0.75f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.00f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.14f, 0.17f, 0.22f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.22f, 0.28f, 0.38f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.18f, 0.24f, 0.32f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.10f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.17f, 0.22f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.14f, 0.17f, 0.22f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.20f, 0.24f, 0.30f, 1.00f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.00f, 0.50f, 0.80f, 0.40f);

    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 5.0f;
    style.WindowPadding     = ImVec2(12.0f, 12.0f);
    style.FramePadding      = ImVec2(8.0f, 5.0f);
    style.ItemSpacing       = ImVec2(8.0f, 8.0f);
}

void UIDashboard::Render() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastRefreshTime).count() >= m_refreshIntervalMs) {
        m_monitor.UpdateMetrics();
        m_lastRefreshTime = now;
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("TaskManagerRoot", nullptr, windowFlags);

    RenderHeader();
    ImGui::Separator();

    if (ImGui::BeginTabBar("TaskManagerTabs", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("  Tasks & Processes  ")) {
            RenderProcessTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  Performance Graphs  ")) {
            RenderPerformanceTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  Storage & Disks  ")) {
            RenderStorageTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  Network  ")) {
            RenderNetworkTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  System Specs  ")) {
            RenderSpecsTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    RenderTerminateModal();

    ImGui::End();
}

void UIDashboard::RenderHeader() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.12f, 0.16f, 1.00f));
    ImGui::BeginChild("HeaderChild", ImVec2(0, 56), true);

    ImGui::TextColored(ImVec4(0.00f, 0.75f, 1.00f, 1.00f), "SYS-MONITOR TASK MANAGER");
    ImGui::SameLine();
    ImGui::TextDisabled("| Expandable Process Trees & Real-Time Telemetry");

    ImGui::SameLine(ImGui::GetWindowWidth() - 480);

    ImGui::Text("Refresh Rate:");
    ImGui::SameLine();
    if (ImGui::RadioButton("250ms", m_refreshIntervalMs == 250)) m_refreshIntervalMs = 250;
    ImGui::SameLine();
    if (ImGui::RadioButton("500ms", m_refreshIntervalMs == 500)) m_refreshIntervalMs = 500;
    ImGui::SameLine();
    if (ImGui::RadioButton("1s", m_refreshIntervalMs == 1000)) m_refreshIntervalMs = 1000;

    ImGui::SameLine();
    if (ImGui::Button(" Refresh Now ")) {
        m_monitor.UpdateMetrics();
        m_monitor.RefreshProcessList();
        m_lastRefreshTime = std::chrono::steady_clock::now();
    }

    uint64_t uptimeSec = m_monitor.GetUptimeSeconds();
    uint64_t days = uptimeSec / (24 * 3600);
    uint64_t hours = (uptimeSec % (24 * 3600)) / 3600;
    uint64_t minutes = (uptimeSec % 3600) / 60;
    uint64_t seconds = uptimeSec % 60;

    char uptimeStr[64];
    snprintf(uptimeStr, sizeof(uptimeStr), "Uptime: %llud %02lluh %02llum %02llus", days, hours, minutes, seconds);
    ImGui::TextColored(ImVec4(0.48f, 0.85f, 0.55f, 1.00f), "%s", uptimeStr);

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void UIDashboard::RenderProcessTab() {
    ImGui::Spacing();

    ImGui::SetNextItemWidth(320.0f);
    ImGui::InputTextWithHint("##FilterInput", "Search task name or PID...", m_processFilterBuf, sizeof(m_processFilterBuf));

    ImGui::SameLine();
    if (ImGui::Button("Clear Filter")) {
        m_processFilterBuf[0] = '\0';
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh Tasks")) {
        m_monitor.RefreshProcessList();
    }

    ImGui::SameLine(ImGui::GetWindowWidth() - 210);

    bool itemSelected = (m_selectedProcessPid != -1 || (m_isSelectedGroup && !m_selectedGroupName.empty()));
    if (!itemSelected) {
        ImGui::BeginDisabled();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.18f, 0.22f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.25f, 0.30f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.60f, 0.10f, 0.15f, 1.00f));

    std::string btnText = m_isSelectedGroup ? " End Task Group " : " End Single Task ";
    if (ImGui::Button(btnText.c_str(), ImVec2(180, 0))) {
        if (m_isSelectedGroup) {
            m_isTerminatingGroup = true;
            m_modalGroupNameToTerminate = m_selectedGroupName;
            m_showTerminateModal = true;
        } else if (m_selectedProcessPid != -1) {
            m_isTerminatingGroup = false;
            m_modalPidToTerminate = static_cast<DWORD>(m_selectedProcessPid);
            m_modalProcessNameToTerminate = m_selectedProcessName;
            m_showTerminateModal = true;
        }
    }

    ImGui::PopStyleColor(3);

    if (!itemSelected) {
        ImGui::EndDisabled();
    }

    if (!m_statusMessage.empty()) {
        ImGui::Spacing();
        if (m_statusIsError) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "[Error] %s", m_statusMessage.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.4f, 1.0f), "[Success] %s", m_statusMessage.c_str());
        }
    }

    ImGui::Spacing();

    const auto& groups = m_monitor.GetProcessGroups();

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                                        ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable |
                                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                                        ImGuiTableFlags_BordersV | ImGuiTableFlags_ScrollY;

    float tableHeight = ImGui::GetContentRegionAvail().y - 10.0f;

    if (ImGui::BeginTable("ProcessTreeTable", 6, tableFlags, ImVec2(0, tableHeight))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Application / Task Name", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 85.0f);
        ImGui::TableSetupColumn("Total Memory (RAM)", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("Threads", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        std::string filterStr = m_processFilterBuf;
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        for (size_t gIdx = 0; gIdx < groups.size(); ++gIdx) {
            const auto& grp = groups[gIdx];

            bool groupMatches = false;
            if (!filterStr.empty()) {
                std::string lowerGrpName = grp.groupName;
                std::transform(lowerGrpName.begin(), lowerGrpName.end(), lowerGrpName.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                if (lowerGrpName.find(filterStr) != std::string::npos) {
                    groupMatches = true;
                } else {
                    for (const auto& child : grp.children) {
                        std::string pidStr = std::to_string(child.pid);
                        if (pidStr.find(filterStr) != std::string::npos) {
                            groupMatches = true;
                            break;
                        }
                    }
                }
                if (!groupMatches) continue;
            }

            ImGui::TableNextRow();

            if (grp.processCount > 1) {
                ImGui::TableSetColumnIndex(0);

                char treeLabel[256];
                snprintf(treeLabel, sizeof(treeLabel), "%s  (%zu processes)", grp.groupName.c_str(), grp.processCount);

                ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow;
                if (!filterStr.empty()) {
                    treeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
                }

                bool isGroupSelected = (m_isSelectedGroup && m_selectedGroupName == grp.groupName);
                if (isGroupSelected) {
                    treeFlags |= ImGuiTreeNodeFlags_Selected;
                }

                bool nodeOpen = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(gIdx + 1000)), treeFlags, "%s", treeLabel);

                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                    m_isSelectedGroup = true;
                    m_selectedGroupName = grp.groupName;
                    m_selectedProcessPid = -1;
                }

                if (ImGui::BeginPopupContextItem()) {
                    m_isSelectedGroup = true;
                    m_selectedGroupName = grp.groupName;
                    m_selectedProcessPid = -1;

                    ImGui::TextDisabled("App Group Options: %s", grp.groupName.c_str());
                    ImGui::Separator();
                    if (ImGui::MenuItem("End All Tasks in Group")) {
                        m_isTerminatingGroup = true;
                        m_modalGroupNameToTerminate = grp.groupName;
                        m_showTerminateModal = true;
                    }
                    ImGui::EndPopup();
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::TextDisabled("[%zu tasks]", grp.processCount);

                ImGui::TableSetColumnIndex(2);
                if (grp.totalMemoryMB > 1024.0) {
                    ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.9f, 1.0f), "%.2f GB", grp.totalMemoryMB / 1024.0);
                } else {
                    ImGui::Text("%.1f MB", grp.totalMemoryMB);
                }

                ImGui::TableSetColumnIndex(3);
                if (grp.totalCpuPercent > 5.0) {
                    ImGui::TextColored(ImVec4(0.0f, 0.85f, 1.0f, 1.0f), "%.1f %%", grp.totalCpuPercent);
                } else {
                    ImGui::Text("%.1f %%", grp.totalCpuPercent);
                }

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%lu", grp.totalThreadCount);

                ImGui::TableSetColumnIndex(5);
                ImGui::TextDisabled("Multiple");

                if (nodeOpen) {
                    for (const auto& child : grp.children) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Indent(24.0f);

                        bool isChildSelected = (!m_isSelectedGroup && m_selectedProcessPid == static_cast<int>(child.pid));
                        char childLabel[256];
                        snprintf(childLabel, sizeof(childLabel), "  └─ %s", child.name.c_str());

                        if (ImGui::Selectable(childLabel, isChildSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                            m_isSelectedGroup = false;
                            m_selectedProcessPid = child.pid;
                            m_selectedProcessName = child.name;
                        }

                        if (ImGui::BeginPopupContextItem()) {
                            m_isSelectedGroup = false;
                            m_selectedProcessPid = child.pid;
                            m_selectedProcessName = child.name;

                            ImGui::TextDisabled("Child Process Options (PID: %lu)", child.pid);
                            ImGui::Separator();
                            if (ImGui::MenuItem("End Single Task")) {
                                m_isTerminatingGroup = false;
                                m_modalPidToTerminate = child.pid;
                                m_modalProcessNameToTerminate = child.name;
                                m_showTerminateModal = true;
                            }
                            ImGui::EndPopup();
                        }

                        ImGui::Unindent(24.0f);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%lu", child.pid);

                        ImGui::TableSetColumnIndex(2);
                        if (child.memoryMB > 1024.0) {
                            ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.9f, 1.0f), "%.2f GB", child.memoryMB / 1024.0);
                        } else {
                            ImGui::Text("%.1f MB", child.memoryMB);
                        }

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%.1f %%", child.cpuUsagePercent);

                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%lu", child.threadCount);

                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%s", child.priorityStr.c_str());
                    }

                    ImGui::TreePop();
                }
            } else if (!grp.children.empty()) {
                const auto& proc = grp.children[0];

                ImGui::TableSetColumnIndex(0);

                bool isSelected = (!m_isSelectedGroup && m_selectedProcessPid == static_cast<int>(proc.pid));
                if (ImGui::Selectable(proc.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                    m_isSelectedGroup = false;
                    m_selectedProcessPid = proc.pid;
                    m_selectedProcessName = proc.name;
                }

                if (ImGui::BeginPopupContextItem()) {
                    m_isSelectedGroup = false;
                    m_selectedProcessPid = proc.pid;
                    m_selectedProcessName = proc.name;

                    ImGui::TextDisabled("Process Options (PID: %lu)", proc.pid);
                    ImGui::Separator();
                    if (ImGui::MenuItem("End Task")) {
                        m_isTerminatingGroup = false;
                        m_modalPidToTerminate = proc.pid;
                        m_modalProcessNameToTerminate = proc.name;
                        m_showTerminateModal = true;
                    }
                    ImGui::EndPopup();
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lu", proc.pid);

                ImGui::TableSetColumnIndex(2);
                if (proc.memoryMB > 1024.0) {
                    ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.9f, 1.0f), "%.2f GB", proc.memoryMB / 1024.0);
                } else {
                    ImGui::Text("%.1f MB", proc.memoryMB);
                }

                ImGui::TableSetColumnIndex(3);
                if (proc.cpuUsagePercent > 5.0) {
                    ImGui::TextColored(ImVec4(0.0f, 0.85f, 1.0f, 1.0f), "%.1f %%", proc.cpuUsagePercent);
                } else {
                    ImGui::Text("%.1f %%", proc.cpuUsagePercent);
                }

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%lu", proc.threadCount);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", proc.priorityStr.c_str());
            }
        }

        ImGui::EndTable();
    }
}

void UIDashboard::RenderPerformanceTab() {
    ImGui::Spacing();

    float leftWidth = ImGui::GetContentRegionAvail().x * 0.68f;

    ImGui::BeginChild("GraphsChild", ImVec2(leftWidth, 0), true);

    float currentCpu = m_monitor.GetCurrentCpuPercent();
    const auto& cpuHist = m_monitor.GetCpuHistory();
    char cpuOverlay[64];
    snprintf(cpuOverlay, sizeof(cpuOverlay), "CPU Total: %.1f %%", currentCpu);

    ImGui::TextColored(ImVec4(0.00f, 0.75f, 1.00f, 1.00f), "CPU Utilization");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.00f, 0.75f, 1.00f, 1.00f));
    ImGui::PlotLines("##CpuGraph", cpuHist.data(), static_cast<int>(cpuHist.size()), 0, cpuOverlay, 0.0f, 100.0f, ImVec2(-1, 110));
    ImGui::PopStyleColor();

    ImGui::Spacing();

    float currentRam = m_monitor.GetCurrentRamPercent();
    const auto& ramHist = m_monitor.GetRamHistory();
    char ramOverlay[64];
    snprintf(ramOverlay, sizeof(ramOverlay), "RAM Used: %.1f GB / %.1f GB (%.0f%%)",
             m_monitor.GetUsedRamGB(), m_monitor.GetTotalRamGB(), currentRam);

    ImGui::TextColored(ImVec4(0.75f, 0.40f, 0.95f, 1.00f), "Memory (RAM) Allocation");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.75f, 0.40f, 0.95f, 1.00f));
    ImGui::PlotLines("##RamGraph", ramHist.data(), static_cast<int>(ramHist.size()), 0, ramOverlay, 0.0f, 100.0f, ImVec2(-1, 110));
    ImGui::PopStyleColor();

    ImGui::Spacing();

    const auto& rxHist = m_monitor.GetNetDownloadHistory();
    char netOverlay[64];
    snprintf(netOverlay, sizeof(netOverlay), "DL: %.1f Kbps | UL: %.1f Kbps", m_monitor.GetNetDownloadKbps(), m_monitor.GetNetUploadKbps());

    ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.50f, 1.00f), "Network Bandwidth (Download)");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.30f, 0.85f, 0.50f, 1.00f));
    ImGui::PlotLines("##NetGraph", rxHist.data(), static_cast<int>(rxHist.size()), 0, netOverlay, 0.0f, 5000.0f, ImVec2(-1, 110));
    ImGui::PopStyleColor();

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("CoreGridChild", ImVec2(0, 0), true);
    ImGui::TextColored(ImVec4(0.95f, 0.70f, 0.20f, 1.00f), "Logical CPU Core Grid");
    ImGui::Separator();

    const auto& coreUsages = m_monitor.GetPerCoreCpuUsage();
    for (size_t i = 0; i < coreUsages.size(); ++i) {
        char coreLabel[32];
        snprintf(coreLabel, sizeof(coreLabel), "Core %d", static_cast<int>(i + 1));
        ImGui::Text("%-7s", coreLabel);
        ImGui::SameLine();

        float val = coreUsages[i] / 100.0f;
        char barBuf[16];
        snprintf(barBuf, sizeof(barBuf), "%.0f%%", coreUsages[i]);

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.00f, 0.65f, 0.90f, 1.00f));
        ImGui::ProgressBar(val, ImVec2(-1, 18), barBuf);
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();
}

void UIDashboard::RenderStorageTab() {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.95f, 0.70f, 0.20f, 1.00f), "STORAGE DRIVES & DISK VOLUMES");
    ImGui::Separator();
    ImGui::Spacing();

    const auto& drives = m_monitor.GetDrives();

    for (const auto& drive : drives) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.14f, 0.17f, 0.22f, 1.00f));
        ImGui::BeginChild(drive.driveLetter.c_str(), ImVec2(0, 110), true);

        ImGui::TextColored(ImVec4(0.00f, 0.75f, 1.00f, 1.00f), "Drive (%s) - %s", drive.driveLetter.c_str(), drive.volumeName.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - 180);
        ImGui::TextDisabled("File System: %s", drive.fileSystem.c_str());

        ImGui::Spacing();

        char progressText[128];
        snprintf(progressText, sizeof(progressText), "%.1f GB used of %.1f GB total (Free: %.1f GB)",
                 drive.usedGB, drive.totalGB, drive.freeGB);

        float fraction = static_cast<float>(drive.usedPercent / 100.0);

        ImVec4 barColor = ImVec4(0.20f, 0.80f, 0.40f, 1.00f);
        if (drive.usedPercent > 85.0) {
            barColor = ImVec4(0.90f, 0.25f, 0.25f, 1.00f);
        } else if (drive.usedPercent > 70.0) {
            barColor = ImVec4(0.95f, 0.65f, 0.20f, 1.00f);
        }

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::ProgressBar(fraction, ImVec2(-1, 24), progressText);
        ImGui::PopStyleColor();

        ImGui::TextDisabled("Drive Type: Fixed Local Disk Storage");

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }
}

void UIDashboard::RenderNetworkTab() {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.50f, 1.00f), "NETWORK INTERFACES & TRAFFIC");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginChild("NetMetricsChild", ImVec2(0, 160), true);
    ImGui::Text("Active Primary Network Adapter");
    ImGui::Separator();

    ImGui::Columns(2, "NetColumns", false);
    ImGui::SetColumnWidth(0, 300);

    ImGui::Text("Current Download Rate:");
    ImGui::TextColored(ImVec4(0.00f, 0.80f, 1.00f, 1.00f), "%.2f Kbps (%.2f MB/s)", m_monitor.GetNetDownloadKbps(), m_monitor.GetNetDownloadKbps() / 8000.0);

    ImGui::Spacing();

    ImGui::Text("Current Upload Rate:");
    ImGui::TextColored(ImVec4(0.85f, 0.40f, 0.95f, 1.00f), "%.2f Kbps (%.2f MB/s)", m_monitor.GetNetUploadKbps(), m_monitor.GetNetUploadKbps() / 8000.0);

    ImGui::NextColumn();

    ImGui::Text("Adapter Status: Connected / Active");
    ImGui::Text("Protocol: IPv4 / IPv6 Dual-Stack");
    ImGui::Text("Interface Speed: 1.0 Gbps Full Duplex");

    ImGui::Columns(1);
    ImGui::EndChild();
}

void UIDashboard::RenderSpecsTab() {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.00f, 0.75f, 1.00f, 1.00f), "HARDWARE & SYSTEM SPECIFICATIONS");
    ImGui::Separator();
    ImGui::Spacing();

    const SystemSpecs& specs = m_monitor.GetSpecs();

    ImGui::BeginChild("SpecsChild", ImVec2(0, 0), true);

    ImGui::TextColored(ImVec4(0.95f, 0.70f, 0.20f, 1.00f), "Processor (CPU)");
    ImGui::Text("  Model:               %s", specs.cpuModel.c_str());
    ImGui::Text("  Physical Cores:      %lu", specs.coreCount);
    ImGui::Text("  Logical Processors:  %lu", specs.logicalProcessorCount);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.75f, 0.40f, 0.95f, 1.00f), "Memory (RAM)");
    ImGui::Text("  Total Physical RAM:  %.2f GB", specs.totalRamGB);
    ImGui::Text("  Architecture:        x86_64 (64-Bit)");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.50f, 1.00f), "Graphics (GPU)");
    ImGui::Text("  GPU Model:           %s", specs.gpuName.c_str());
    ImGui::Text("  Dedicated VRAM:      %.2f GB", specs.gpuMemoryGB);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.00f, 0.75f, 1.00f, 1.00f), "Operating System");
    ImGui::Text("  OS Build:            %s", specs.osVersion.c_str());

    ImGui::EndChild();
}

void UIDashboard::RenderTerminateModal() {
    if (m_showTerminateModal) {
        ImGui::OpenPopup("Confirm Task Termination");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(440, 220));

    if (ImGui::BeginPopupModal("Confirm Task Termination", &m_showTerminateModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "WARNING: Terminating processes may cause unsaved data loss!");
        ImGui::Spacing();

        if (m_isTerminatingGroup) {
            ImGui::Text("Are you sure you want to end ALL tasks in process group:");
            ImGui::TextColored(ImVec4(0.0f, 0.75f, 1.00f, 1.00f), "  App Group: %s", m_modalGroupNameToTerminate.c_str());
        } else {
            ImGui::Text("Are you sure you want to end task:");
            ImGui::TextColored(ImVec4(0.0f, 0.75f, 1.00f, 1.00f), "  Process: %s", m_modalProcessNameToTerminate.c_str());
            ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "  PID:     %lu", m_modalPidToTerminate);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button(" Yes, Terminate ", ImVec2(160, 32))) {
            std::string errMsg;
            bool ok = false;
            if (m_isTerminatingGroup) {
                ok = m_monitor.TerminateTaskGroup(m_modalGroupNameToTerminate, errMsg);
                if (ok) {
                    m_statusMessage = errMsg;
                    m_statusIsError = false;
                    m_isSelectedGroup = false;
                    m_selectedGroupName.clear();
                } else {
                    m_statusMessage = errMsg;
                    m_statusIsError = true;
                }
            } else {
                ok = m_monitor.TerminateTask(m_modalPidToTerminate, errMsg);
                if (ok) {
                    m_statusMessage = "Successfully terminated task " + m_modalProcessNameToTerminate + " (PID " + std::to_string(m_modalPidToTerminate) + ").";
                    m_statusIsError = false;
                    m_selectedProcessPid = -1;
                } else {
                    m_statusMessage = errMsg;
                    m_statusIsError = true;
                }
            }

            m_showTerminateModal = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button(" Cancel ", ImVec2(120, 32))) {
            m_showTerminateModal = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

} // namespace SysMon
