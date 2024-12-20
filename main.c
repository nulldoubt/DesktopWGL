#include <Windows.h>
#include <stdio.h>
#include <gl/gl.h>

HWND GetWallpaperArea(void);

BOOL CALLBACK FindWorker(HWND wnd, LPARAM lp);

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
    HWND worker;
    HWND progman = GetShellWindow();
    if (!progman) {
        printf("Unable to find progman\n");
        return NULL;
    }

    SendMessageA(progman, 0x052C, 0xD, 0);
    SendMessageA(progman, 0x052C, 0xD, 1);

    EnumWindows(FindWorker, (LPARAM)&worker);

    if (!worker) {
        printf("Couldn't spawn WorkerW window, trying old method...\n");
        SendMessageA(progman, 0x052C, 0, 0);
        EnumWindows(FindWorker, (LPARAM)&worker);
    }

    if (!worker) {
        printf("Couldn't spawn worker, falling back to progman\n");
        worker = progman;
    }

    return worker;
}

BOOL CALLBACK FindWorker(HWND wnd, LPARAM lp) {
    HWND *pworker = (HWND*)lp;

    if (!FindWindowExA(wnd, 0, "SHELLDLL_DefView", 0)) {
        return TRUE;
    }

    *pworker = FindWindowExA(0, wnd, "WorkerW", 0);
    if (*pworker)
        return FALSE;

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
