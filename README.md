# TaskManagerApp (C++ Windows Desktop Task Manager)

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-0078D6.svg)](https://microsoft.com/windows)
[![UI Framework](https://img.shields.io/badge/UI-Dear%20ImGui-FF69B4.svg)](https://github.com/ocornut/imgui)
[![Graphics API](https://img.shields.io/badge/Graphics-DirectX%2011-green.svg)](https://microsoft.com)

A standalone native C++ Windows Task Manager and System Telemetry Desktop Application.

---

## 📂 Directory Layout

```
TaskManagerApp/
├── CMakeLists.txt              # CMake build script
├── README.md                   # Project documentation
├── include/                    # Public C++ Headers
│   ├── system_monitor.h        # RAII Win32 Telemetry Engine
│   └── ui_dashboard.h          # ImGui dashboard UI renderer
├── src/                        # Implementation Sources
│   ├── main.cpp                # Win32 & DirectX 11 entry point
│   ├── system_monitor.cpp      # Win32 kernel metrics calls
│   └── ui_dashboard.cpp        # ImGui tree node table rendering
└── vendor/                     # Third-Party Dependencies
    └── imgui/                  # Dear ImGui library
```

---

## 🛠️ Build Commands

```powershell
cd C:\Users\dfah1\.gemini\antigravity\scratch\TaskManagerApp
cmake -B build -S .
cmake --build build --config Release
.\build\Release\TaskManagerApp.exe
```
