cmake_minimum_required(VERSION 3.16)
project(TaskManagerApp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/system_monitor.cpp
    src/ui_dashboard.cpp
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/backends/imgui_impl_win32.cpp
    vendor/imgui/backends/imgui_impl_dx11.cpp
)

set(HEADERS
    include/system_monitor.h
    include/ui_dashboard.h
    vendor/imgui/imgui.h
    vendor/imgui/backends/imgui_impl_win32.h
    vendor/imgui/backends/imgui_impl_dx11.h
)

add_executable(TaskManagerApp WIN32 ${SOURCES} ${HEADERS})

target_include_directories(TaskManagerApp PRIVATE
    include
    vendor/imgui
    vendor/imgui/backends
)

target_link_libraries(TaskManagerApp PRIVATE
    d3d11.lib
    d3dcompiler.lib
    dxgi.lib
    pdh.lib
    psapi.lib
    iphlpapi.lib
    dwmapi.lib
    user32.lib
    gdi32.lib
)

target_compile_definitions(TaskManagerApp PRIVATE UNICODE _UNICODE NOMINMAX)

if(MSVC)
    target_compile_options(TaskManagerApp PRIVATE /W4 /O2 /permissive-)
endif()
