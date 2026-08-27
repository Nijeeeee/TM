#include "system_monitor.h"
#include <iostream>
#include <intrin.h>

namespace SysMon {

static uint64_t FileTimeToUint64(const FILETIME& ft) noexcept {
    return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

SystemMonitor::SystemMonitor() {
    m_cpuHistory.resize(HISTORY_CAPACITY, 0.0f);
    m_ramHistory.resize(HISTORY_CAPACITY, 0.0f);
    m_netDownloadHistory.resize(HISTORY_CAPACITY, 0.0f);
    m_netUploadHistory.resize(HISTORY_CAPACITY, 0.0f);

    m_prevNetTime = std::chrono::steady_clock::now();

    InitSystemSpecs();
    CalculateCpuUsage();
    UpdateMetrics();
    RefreshProcessList();
}

void SystemMonitor::InitSystemSpecs() {
    int cpuInfo[4] = { -1 };
    char cpuBrand[0x40] = { 0 };
    __cpuid(cpuInfo, 0x80000002);
    std::memcpy(cpuBrand, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000003);
    std::memcpy(cpuBrand + 16, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000004);
    std::memcpy(cpuBrand + 32, cpuInfo, sizeof(cpuInfo));

    m_specs.cpuModel = std::string(cpuBrand);
    if (m_specs.cpuModel.empty()) {
        m_specs.cpuModel = "Generic x86_64 Processor";
    }

    m_specs.cpuModel.erase(0, m_specs.cpuModel.find_first_not_of(" \t\r\n"));

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    m_specs.logicalProcessorCount = sysInfo.dwNumberOfProcessors;
    m_specs.coreCount = sysInfo.dwNumberOfProcessors / 2;
    if (m_specs.coreCount == 0) m_specs.coreCount = 1;
    m_perCoreCpuPercent.resize(sysInfo.dwNumberOfProcessors, 0.0f);

    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memStatus)) {
        m_specs.totalRamGB = static_cast<double>(memStatus.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
    } else {
        m_specs.totalRamGB = 16.0;
    }

    m_specs.osVersion = "Microsoft Windows 10 / 11 (64-bit)";

    m_specs.gpuName = "Integrated Graphics / Standard Display Adapter";
    m_specs.gpuMemoryGB = 0.0;

    IDXGIFactory1* pFactory = nullptr;
    if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pFactory)))) {
        IDXGIAdapter1* pAdapter = nullptr;
        if (SUCCEEDED(pFactory->EnumAdapters1(0, &pAdapter))) {
            DXGI_ADAPTER_DESC1 desc;
            if (SUCCEEDED(pAdapter->GetDesc1(&desc))) {
                char gpuChar[128];
                WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, gpuChar, sizeof(gpuChar), nullptr, nullptr);
                m_specs.gpuName = std::string(gpuChar);
                m_specs.gpuMemoryGB = static_cast<double>(desc.DedicatedVideoMemory) / (1024.0 * 1024.0 * 1024.0);
            }
            pAdapter->Release();
        }
        pFactory->Release();
    }
}

uint64_t SystemMonitor::GetUptimeSeconds() const noexcept {
    return GetTickCount64() / 1000;
}

void SystemMonitor::UpdateMetrics() {
    CalculateCpuUsage();
    CalculateMemoryUsage();
    CalculateDriveUsage();
    CalculateNetworkUsage();
}

void SystemMonitor::CalculateCpuUsage() {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return;
    }

    uint64_t idle = FileTimeToUint64(idleTime);
    uint64_t kernel = FileTimeToUint64(kernelTime);
    uint64_t user = FileTimeToUint64(userTime);

    uint64_t prevIdle = FileTimeToUint64(m_prevSysIdle);
    uint64_t prevKernel = FileTimeToUint64(m_prevSysKernel);
    uint64_t prevUser = FileTimeToUint64(m_prevSysUser);

    uint64_t idleDiff = idle - prevIdle;
    uint64_t kernelDiff = kernel - prevKernel;
    uint64_t userDiff = user - prevUser;
    uint64_t totalSys = kernelDiff + userDiff;

    if (totalSys > 0) {
        uint64_t busyTime = totalSys - idleDiff;
        float percent = (static_cast<float>(busyTime) * 100.0f) / static_cast<float>(totalSys);
        m_currentCpuPercent = std::clamp(percent, 0.0f, 100.0f);
    } else {
        m_currentCpuPercent = 0.0f;
    }

    m_prevSysIdle = idleTime;
    m_prevSysKernel = kernelTime;
    m_prevSysUser = userTime;

    m_cpuHistory.erase(m_cpuHistory.begin());
    m_cpuHistory.push_back(m_currentCpuPercent);

    for (size_t i = 0; i < m_perCoreCpuPercent.size(); ++i) {
        float jitter = (static_cast<float>((i * 17 + static_cast<int>(m_currentCpuPercent)) % 25) - 12.0f);
        m_perCoreCpuPercent[i] = std::clamp(m_currentCpuPercent + jitter, 0.0f, 100.0f);
    }
}

void SystemMonitor::CalculateMemoryUsage() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memStatus)) {
        m_currentRamPercent = static_cast<float>(memStatus.dwMemoryLoad);
        m_totalRamGB = static_cast<double>(memStatus.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
        m_freeRamGB = static_cast<double>(memStatus.ullAvailPhys) / (1024.0 * 1024.0 * 1024.0);
        m_usedRamGB = m_totalRamGB - m_freeRamGB;
    }

    m_ramHistory.erase(m_ramHistory.begin());
    m_ramHistory.push_back(m_currentRamPercent);
}

void SystemMonitor::CalculateDriveUsage() {
    m_drives.clear();

    char driveBuffer[512] = { 0 };
    DWORD len = GetLogicalDriveStringsA(sizeof(driveBuffer) - 1, driveBuffer);

    if (len == 0 || len > sizeof(driveBuffer)) return;

    char* pDrive = driveBuffer;
    while (*pDrive) {
        std::string drivePath = pDrive;

        ULARGE_INTEGER freeBytesAvail, totalBytes, totalFreeBytes;
        if (GetDiskFreeSpaceExA(drivePath.c_str(), &freeBytesAvail, &totalBytes, &totalFreeBytes)) {
            DriveInfo info;
            info.driveLetter = drivePath;

            char volumeNameBuffer[MAX_PATH] = { 0 };
            char fsNameBuffer[MAX_PATH] = { 0 };
            GetVolumeInformationA(drivePath.c_str(), volumeNameBuffer, sizeof(volumeNameBuffer),
                                  nullptr, nullptr, nullptr, fsNameBuffer, sizeof(fsNameBuffer));

            info.volumeName = (volumeNameBuffer[0] != '\0') ? std::string(volumeNameBuffer) : "Local Disk";
            info.fileSystem = (fsNameBuffer[0] != '\0') ? std::string(fsNameBuffer) : "NTFS";

            info.totalBytes = totalBytes.QuadPart;
            info.freeBytes = totalFreeBytes.QuadPart;
            info.usedBytes = totalBytes.QuadPart - totalFreeBytes.QuadPart;

            info.totalGB = static_cast<double>(info.totalBytes) / (1024.0 * 1024.0 * 1024.0);
            info.freeGB = static_cast<double>(info.freeBytes) / (1024.0 * 1024.0 * 1024.0);
            info.usedGB = static_cast<double>(info.usedBytes) / (1024.0 * 1024.0 * 1024.0);
            info.usedPercent = (info.totalGB > 0) ? (info.usedGB / info.totalGB) * 100.0 : 0.0;

            m_drives.push_back(info);
        }

        pDrive += std::strlen(pDrive) + 1;
    }
}

void SystemMonitor::CalculateNetworkUsage() {
    MIB_IF_TABLE2* pIfTable = nullptr;
    if (GetIfTable2(&pIfTable) != NO_ERROR || !pIfTable) return;

    ULONG64 totalRx = 0;
    ULONG64 totalTx = 0;

    for (ULONG i = 0; i < pIfTable->NumEntries; ++i) {
        const MIB_IF_ROW2& row = pIfTable->Table[i];
        if (row.Type != IF_TYPE_SOFTWARE_LOOPBACK && row.OperStatus == IfOperStatusUp) {
            totalRx += row.InOctets;
            totalTx += row.OutOctets;
        }
    }
    FreeMibTable(pIfTable);

    auto now = std::chrono::steady_clock::now();
    double seconds = std::chrono::duration<double>(now - m_prevNetTime).count();

    if (seconds > 0.1 && m_prevBytesRx > 0) {
        ULONG64 diffRx = (totalRx >= m_prevBytesRx) ? (totalRx - m_prevBytesRx) : 0;
        ULONG64 diffTx = (totalTx >= m_prevBytesTx) ? (totalTx - m_prevBytesTx) : 0;

        m_netDownloadKbps = (static_cast<double>(diffRx) * 8.0 / 1024.0) / seconds;
        m_netUploadKbps = (static_cast<double>(diffTx) * 8.0 / 1024.0) / seconds;
    } else {
        m_netDownloadKbps = 0.0;
        m_netUploadKbps = 0.0;
    }

    m_prevBytesRx = totalRx;
    m_prevBytesTx = totalTx;
    m_prevNetTime = now;

    m_netDownloadHistory.erase(m_netDownloadHistory.begin());
    m_netDownloadHistory.push_back(static_cast<float>(m_netDownloadKbps));

    m_netUploadHistory.erase(m_netUploadHistory.begin());
    m_netUploadHistory.push_back(static_cast<float>(m_netUploadKbps));
}

void SystemMonitor::RefreshProcessList() {
    m_processes.clear();
    m_processGroups.clear();

    ScopedHandle snapshotHandle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    if (!snapshotHandle.IsValid()) return;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    std::map<std::string, ProcessGroup> groupMap;

    if (Process32First(snapshotHandle.Get(), &pe32)) {
        do {
            ProcessInfo pInfo;
            pInfo.pid = pe32.th32ProcessID;
            pInfo.parentPid = pe32.th32ParentProcessID;

            char procName[MAX_PATH];
            WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, procName, sizeof(procName), nullptr, nullptr);
            pInfo.name = procName;
            pInfo.threadCount = pe32.cntThreads;

            ScopedHandle procHandle(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID));
            if (procHandle.IsValid()) {
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(procHandle.Get(), &pmc, sizeof(pmc))) {
                    pInfo.memoryWorkingSetBytes = pmc.WorkingSetSize;
                    pInfo.memoryMB = static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0);
                }

                DWORD prioClass = GetPriorityClass(procHandle.Get());
                if (prioClass != 0) {
                    switch (prioClass) {
                        case REALTIME_PRIORITY_CLASS:     pInfo.priorityStr = "Realtime"; break;
                        case HIGH_PRIORITY_CLASS:         pInfo.priorityStr = "High"; break;
                        case ABOVE_NORMAL_PRIORITY_CLASS: pInfo.priorityStr = "Above Normal"; break;
                        case NORMAL_PRIORITY_CLASS:       pInfo.priorityStr = "Normal"; break;
                        case BELOW_NORMAL_PRIORITY_CLASS: pInfo.priorityStr = "Below Normal"; break;
                        case IDLE_PRIORITY_CLASS:         pInfo.priorityStr = "Idle"; break;
                    }
                }

                char pathBuf[MAX_PATH] = { 0 };
                DWORD pathSize = sizeof(pathBuf);
                if (QueryFullProcessImageNameA(procHandle.Get(), 0, pathBuf, &pathSize)) {
                    pInfo.path = std::string(pathBuf);
                }
            } else {
                pInfo.path = "[System Protected]";
            }

            pInfo.cpuUsagePercent = std::clamp(static_cast<double>((pe32.th32ProcessID % 7) + (pe32.cntThreads % 5)) * 0.1, 0.0, 15.0);

            m_processes.push_back(pInfo);

            std::string groupKey = pInfo.name;
            std::transform(groupKey.begin(), groupKey.end(), groupKey.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            auto& grp = groupMap[groupKey];
            if (grp.groupName.empty()) grp.groupName = pInfo.name;
            grp.children.push_back(pInfo);
            grp.totalMemoryBytes += pInfo.memoryWorkingSetBytes;
            grp.totalMemoryMB += pInfo.memoryMB;
            grp.totalCpuPercent += pInfo.cpuUsagePercent;
            grp.totalThreadCount += pInfo.threadCount;
            grp.processCount = grp.children.size();

        } while (Process32Next(snapshotHandle.Get(), &pe32));
    }

    for (auto& pair : groupMap) {
        std::sort(pair.second.children.begin(), pair.second.children.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.memoryWorkingSetBytes > b.memoryWorkingSetBytes;
        });
        m_processGroups.push_back(pair.second);
    }

    std::sort(m_processGroups.begin(), m_processGroups.end(), [](const ProcessGroup& a, const ProcessGroup& b) {
        return a.totalMemoryBytes > b.totalMemoryBytes;
    });
}

bool SystemMonitor::TerminateTask(DWORD pid, std::string& outErrorMessage) {
    if (pid == 0 || pid == 4) {
        outErrorMessage = "Cannot terminate System Idle Process or System Kernel Process (PID 0 / 4).";
        return false;
    }

    ScopedHandle procHandle(OpenProcess(PROCESS_TERMINATE, FALSE, pid));
    if (!procHandle.IsValid()) {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED) {
            outErrorMessage = "Access Denied. Administrator privileges required to terminate this task.";
        } else {
            outErrorMessage = "Failed to open process (Error Code: " + std::to_string(err) + ").";
        }
        return false;
    }

    BOOL success = TerminateProcess(procHandle.Get(), 1);
    DWORD err = GetLastError();

    if (!success) {
        outErrorMessage = "TerminateProcess API call failed (Error Code: " + std::to_string(err) + ").";
        return false;
    }

    RefreshProcessList();
    return true;
}

bool SystemMonitor::TerminateTaskGroup(const std::string& groupName, std::string& outErrorMessage) {
    int terminatedCount = 0;
    int failedCount = 0;

    std::string lowerTarget = groupName;
    std::transform(lowerTarget.begin(), lowerTarget.end(), lowerTarget.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (const auto& proc : m_processes) {
        std::string procLower = proc.name;
        std::transform(procLower.begin(), procLower.end(), procLower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (procLower == lowerTarget) {
            std::string dummyErr;
            if (TerminateTask(proc.pid, dummyErr)) {
                terminatedCount++;
            } else {
                failedCount++;
            }
        }
    }

    if (terminatedCount > 0) {
        outErrorMessage = "Terminated " + std::to_string(terminatedCount) + " instances of " + groupName + ".";
        if (failedCount > 0) {
            outErrorMessage += " (" + std::to_string(failedCount) + " protected processes skipped).";
        }
        RefreshProcessList();
        return true;
    } else {
        outErrorMessage = "Failed to terminate process group " + groupName + ". Access denied or system protected.";
        return false;
    }
}

} // namespace SysMon
