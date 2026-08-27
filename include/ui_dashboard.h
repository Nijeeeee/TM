#ifndef SYSMON_UI_DASHBOARD_H
#define SYSMON_UI_DASHBOARD_H

#include "imgui.h"
#include "system_monitor.h"
#include <string>
#include <chrono>

namespace SysMon {

class UIDashboard {
public:
    explicit UIDashboard(SystemMonitor& monitor);
    ~UIDashboard() = default;

    UIDashboard(const UIDashboard&) = delete;
    UIDashboard& operator=(const UIDashboard&) = delete;

    // Renders the main dashboard GUI
    void Render();

private:
    void SetupDarkTheme();
    void RenderHeader();
    void RenderProcessTab();
    void RenderPerformanceTab();
    void RenderStorageTab();
    void RenderNetworkTab();
    void RenderSpecsTab();

    void RenderTerminateModal();

    SystemMonitor& m_monitor;

    char m_processFilterBuf[128]{ 0 };
    int m_selectedProcessPid{ -1 };
    std::string m_selectedProcessName;
    std::string m_selectedGroupName;
    bool m_isSelectedGroup{ false };

    int m_refreshIntervalMs{ 500 };
    std::chrono::steady_clock::time_point m_lastRefreshTime;

    bool m_showTerminateModal{ false };
    bool m_isTerminatingGroup{ false };
    DWORD m_modalPidToTerminate{ 0 };
    std::string m_modalProcessNameToTerminate;
    std::string m_modalGroupNameToTerminate;
    std::string m_statusMessage;
    bool m_statusIsError{ false };
};

} // namespace SysMon

#endif // SYSMON_UI_DASHBOARD_H
