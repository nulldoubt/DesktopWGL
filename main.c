#include <Windows.h>
#include <stdio.h>

HWND GetWallpaperArea(void);

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, long uMsg, WPARAM wParam, LPARAM lParam);

int main(void) {
    printf("Starting...\n");

    HWND desktop = GetWallpaperArea();
    if (!desktop) {
        printf("Unable to get wallpaper area\n");
        return 1;
    }

    HDC hdc = GetDC(desktop);
    if (!hdc) {
        printf("Failed to get graphics device context\n");
        return 1;
    }

    HBRUSH hbr = CreateSolidBrush(RGB(0, 30, 130));
    if (!hbr) {
        printf("Failed to create brush\n");
        ReleaseDC(desktop, hdc);
        return 1;
    }

    SelectObject(hdc, hbr);

    RECT rect = {50, 50, 500, 500};

    while ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0) {
        FillRect(hdc, &rect, hbr);
        Sleep(16);
    }

    ReleaseDC(desktop, hdc);

    return 0;
}

HWND GetWallpaperArea() {
    HWND desktop = GetDesktopWindow();
    if (!desktop) {
        printf("Failed to find desktop window handle\n");
        return NULL;
    }

    HWND listview = NULL;
    EnumWindows(EnumProc, (LPARAM) &listview);
    if (!listview) {
        printf("Failed to find window\n");
        return NULL;
    }

    return listview;
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lParam) {
    HWND *parentHandle = (HWND *) lParam;
    char className[256];
    if (GetClassName(hwnd, className, sizeof(className))) {
        if (strcmp(className, "WorkerW") == 0) {
            HWND child = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
            if (child != NULL) {
                *parentHandle = hwnd;
                HWND sysListView = FindWindowEx(child, NULL, "SysListView32", "FolderView");
                if (sysListView != NULL) {
                    printf("Found SysListView32 window!\n");
                    *(HWND *) lParam = sysListView;
                    return FALSE;
                }
            }
        }
    }
    return TRUE; // Continue enumeration
}

LRESULT CALLBACK WindowProc(HWND hwnd, long uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
