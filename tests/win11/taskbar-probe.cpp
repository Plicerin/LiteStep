#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shobjidl.h>
#include <commctrl.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"LiteStepTaskbarProbe";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_INFORMATION);
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0, wc.lpszClassName, L"LiteStep ITaskbarList3 Probe",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 720, 360,
        nullptr, nullptr, instance, nullptr);

    if (!hwnd) {
        CoUninitialize();
        return 2;
    }

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    ITaskbarList3* taskbar = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&taskbar));

    if (SUCCEEDED(hr) && taskbar) {
        taskbar->HrInit();
        taskbar->SetProgressState(hwnd, TBPF_NORMAL);

        HICON info = LoadIcon(nullptr, IDI_INFORMATION);
        taskbar->SetOverlayIcon(hwnd, info, L"LiteStep overlay probe");
        taskbar->SetThumbnailTooltip(hwnd, L"LiteStep Windows 11 ITaskbarList3 probe");

        RECT clip = { 0, 0, 320, 180 };
        taskbar->SetThumbnailClip(hwnd, &clip);

        THUMBBUTTON button = {};
        button.dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
        button.iId = 1001;
        button.hIcon = info;
        button.dwFlags = THBF_ENABLED;
        lstrcpynW(button.szTip, L"LiteStep probe button", ARRAYSIZE(button.szTip));
        taskbar->ThumbBarAddButtons(hwnd, 1, &button);
    }

    DWORD start = GetTickCount();
    MSG msg = {};
    while (GetTickCount() - start < 15000) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                start = 0;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (taskbar) {
            ULONGLONG elapsed = GetTickCount() - start;
            ULONGLONG value = (elapsed / 150) % 101;
            taskbar->SetProgressValue(hwnd, value, 100);
        }

        Sleep(50);
    }

    if (taskbar) {
        taskbar->SetProgressState(hwnd, TBPF_NOPROGRESS);
        taskbar->SetOverlayIcon(hwnd, nullptr, L"");
        taskbar->SetThumbnailClip(hwnd, nullptr);
        taskbar->Release();
    }

    DestroyWindow(hwnd);
    CoUninitialize();
    return 0;
}
