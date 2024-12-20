#include <Windows.h>
#include <stdio.h>

HWND findDesktopWindow(void);

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, long uMsg, WPARAM wParam, LPARAM lParam);

int main(void) {
    printf("Starting...\n");

    HWND desktop = findDesktopWindow();
    if (!desktop) {
        printf("Failed to find desktop window handle\n");
        return 1;
    }

    HDC hdc = GetDC(desktop);

    if (!hdc) {
        printf("Failed to get graphics device context\n");
        return 1;
    }

    HBRUSH hbr = CreateSolidBrush(RGB(0, 30, 130));
    SelectObject(hdc, hbr);

    while ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0) {
        Rectangle(hdc, 50, 50, 500, 500);
        Sleep(16);
    }

    ReleaseDC(desktop, hdc);

    return 0;
}

HWND findDesktopWindow() {
    HWND progman = GetShellWindow();

    SendMessageTimeoutA(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);

    HWND wallpaper_hwnd = NULL;
    EnumWindows(EnumProc, (LPARAM)&wallpaper_hwnd);

    if (!wallpaper_hwnd)
        return NULL;

    return wallpaper_hwnd;
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lParam) {
    char className[256];
    if (GetClassName(hwnd, className, sizeof(className))) {
        if (strcmp(className, "WorkerW") == 0) {
            *(HWND*)lParam = hwnd;
            return FALSE;
        }
    }
    return TRUE;
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