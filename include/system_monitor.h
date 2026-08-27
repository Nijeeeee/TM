#ifndef SYSMON_SYSTEM_MONITOR_H
#define SYSMON_SYSTEM_MONITOR_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <dxgi.h>

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <algorithm>

namespace SysMon {

// RAII Wrapper for Windows HANDLE management
class ScopedHandle {
public:
    explicit ScopedHandle(HANDLE handle = INVALID_HANDLE_VALUE) noexcept : m_handle(handle) {}
    ~ScopedHandle() noexcept { Reset(); }

    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    ScopedHandle(ScopedHandle&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = INVALID_HANDLE_VALUE;
    }

    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            Reset();
            m_handle = other.m_handle;
            other.m_handle = INVALID_HANDLE_VALUE;
        }
        return *this;
    }

    void Reset(HANDLE newHandle = INVALID_HANDLE_VALUE) noexcept {
        if (m_handle != INVALID_HANDLE_VALUE && m_handle != nullptr) {
            CloseHandle(m_handle);
        }
        m_handle = newHandle;
    }

    [[nodiscard]] HANDLE Get() const noexcept { return m_handle; }
    [[nodiscard]] bool IsValid() const noexcept { return m_handle != INVALID_HANDLE_VALUE && m_handle != nullptr; }

private:
    HANDLE m_handle;
};

// Represents a single active task / process
struct ProcessInfo {
    DWORD pid{ 0 };
    DWORD parentPid{ 0 };
    std::string name;
    std::string path;
    size_t memoryWorkingSetBytes{ 0 };
    double memoryMB{ 0.0 };
    DWORD threadCount{ 0 };
    std::string priorityStr{ "Normal" };
    double cpuUsagePercent{ 0.0 };
};

// Represents a process tree group (e.g. Chrome app group with N child processes)
struct ProcessGroup {
    std::string groupName;
    std::vector<ProcessInfo> children;
    size_t totalMemoryBytes{ 0 };
    double totalMemoryMB{ 0.0 };
    double totalCpuPercent{ 0.0 };
    DWORD totalThreadCount{ 0 };
    size_t processCount{ 0 };
};

// Represents a physical / logical drive
struct DriveInfo {
    std::string driveLetter;
    std::string volumeName;
    std::string fileSystem;
    ULONGLONG totalBytes{ 0 };
    ULONGLONG freeBytes{ 0 };
    ULONGLONG usedBytes{ 0 };
    double usedPercent{ 0.0 };
    double totalGB{ 0.0 };
    double freeGB{ 0.0 };
    double usedGB{ 0.0 };
};

// Static hardware specifications
struct SystemSpecs {
    std::string cpuModel;
    DWORD coreCount{ 0 };
    DWORD logicalProcessorCount{ 0 };
    double totalRamGB{ 0.0 };
    std::string osVersion;
    std::string gpuName;
    double gpuMemoryGB{ 0.0 };
};

// Low-level Win32 Telemetry Engine
class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor() = default;

    SystemMonitor(const SystemMonitor&) = delete;
    SystemMonitor& operator=(const SystemMonitor&) = delete;

    // Refresh telemetry metrics
    void UpdateMetrics();

    // Query active task list & aggregate into process trees
    void RefreshProcessList();

    // Process termination actions
    bool TerminateTask(DWORD pid, std::string& outErrorMessage);
    bool TerminateTaskGroup(const std::string& groupName, std::string& outErrorMessage);

    // Telemetry getters
    [[nodiscard]] float GetCurrentCpuPercent() const noexcept { return m_currentCpuPercent; }
    [[nodiscard]] float GetCurrentRamPercent() const noexcept { return m_currentRamPercent; }
    [[nodiscard]] double GetUsedRamGB() const noexcept { return m_usedRamGB; }
    [[nodiscard]] double GetTotalRamGB() const noexcept { return m_totalRamGB; }
    [[nodiscard]] double GetFreeRamGB() const noexcept { return m_freeRamGB; }
    
    [[nodiscard]] double GetNetDownloadKbps() const noexcept { return m_netDownloadKbps; }
    [[nodiscard]] double GetNetUploadKbps() const noexcept { return m_netUploadKbps; }

    [[nodiscard]] const std::vector<float>& GetCpuHistory() const noexcept { return m_cpuHistory; }
    [[nodiscard]] const std::vector<float>& GetRamHistory() const noexcept { return m_ramHistory; }
    [[nodiscard]] const std::vector<float>& GetNetDownloadHistory() const noexcept { return m_netDownloadHistory; }
    [[nodiscard]] const std::vector<float>& GetNetUploadHistory() const noexcept { return m_netUploadHistory; }
    [[nodiscard]] const std::vector<float>& GetPerCoreCpuUsage() const noexcept { return m_perCoreCpuPercent; }

    [[nodiscard]] const std::vector<ProcessInfo>& GetProcesses() const noexcept { return m_processes; }
    [[nodiscard]] const std::vector<ProcessGroup>& GetProcessGroups() const noexcept { return m_processGroups; }
    [[nodiscard]] const std::vector<DriveInfo>& GetDrives() const noexcept { return m_drives; }
    [[nodiscard]] const SystemSpecs& GetSpecs() const noexcept { return m_specs; }
    
    [[nodiscard]] uint64_t GetUptimeSeconds() const noexcept;

private:
    void InitSystemSpecs();
    void CalculateCpuUsage();
    void CalculateMemoryUsage();
    void CalculateDriveUsage();
    void CalculateNetworkUsage();

    static constexpr size_t HISTORY_CAPACITY = 100;

    FILETIME m_prevSysIdle{};
    FILETIME m_prevSysKernel{};
    FILETIME m_prevSysUser{};
    float m_currentCpuPercent{ 0.0f };
    std::vector<float> m_perCoreCpuPercent;
    std::vector<float> m_cpuHistory;

    float m_currentRamPercent{ 0.0f };
    double m_totalRamGB{ 0.0 };
    double m_usedRamGB{ 0.0 };
    double m_freeRamGB{ 0.0 };
    std::vector<float> m_ramHistory;

    ULONG64 m_prevBytesRx{ 0 };
    ULONG64 m_prevBytesTx{ 0 };
    std::chrono::steady_clock::time_point m_prevNetTime;
    double m_netDownloadKbps{ 0.0 };
    double m_netUploadKbps{ 0.0 };
    std::vector<float> m_netDownloadHistory;
    std::vector<float> m_netUploadHistory;

    std::vector<ProcessInfo> m_processes;
    std::vector<ProcessGroup> m_processGroups;
    std::vector<DriveInfo> m_drives;
    SystemSpecs m_specs;
};

} // namespace SysMon

#endif // SYSMON_SYSTEM_MONITOR_H
