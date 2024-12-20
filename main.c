#include <Windows.h>
#include <stdio.h>
#include <gl/gl.h>

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

    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    HBRUSH hbr = CreateSolidBrush(RGB(0, 30, 130));
    if (!hbr) {
        printf("Failed to create brush\n");
        ReleaseDC(desktop, hdc);
        return 1;
    }

    int pixel_format = ChoosePixelFormat(hdc, &pfd);
    if (!pixel_format || !SetPixelFormat(hdc, pixel_format, &pfd)) {
        printf("Failed to set pixel format\n");
        ReleaseDC(desktop, hdc);
        return 1;
    }

    HGLRC hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        printf("Failed to create OpenGL context\n");
        ReleaseDC(desktop, hdc);
        return 1;
    }

    if (!wglMakeCurrent(hdc, hglrc)) {
        printf("Failed to make OpenGL context\n");
        wglDeleteContext(hglrc);
        ReleaseDC(desktop, hdc);
        return 1;
    }

    while ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0) {
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SwapBuffers(hdc);
        Sleep(16);
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
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
                    *(HWND *) lParam = sysListView;
                    return FALSE;
                }
            }
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
