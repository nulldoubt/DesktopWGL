#include <Windows.h>
#include <stdio.h>
#include <gl/gl.h>

HWND GetWallpaperArea();

BOOL CALLBACK FindWorker(HWND hwnd, LPARAM lParam);

void RestoreDesktopWallpaper(void);

int Cleanup(int status, HWND hwnd, HDC hdc, HGLRC hglrc);

int main(void) {
    printf("Starting...\n");

    HWND desktop = GetWallpaperArea();
    if (!desktop) {
        printf("Unable to get wallpaper area\n");
        return Cleanup(1, NULL, NULL, NULL);
    }

    HDC hdc = GetDC(desktop);
    if (!hdc) {
        printf("Failed to get graphics device context\n");
        return Cleanup(1, desktop, NULL, NULL);
    }

    PIXELFORMATDESCRIPTOR pfd = {0};
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
        return Cleanup(1, desktop, hdc, NULL);
    }

    HGLRC hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        printf("Failed to create OpenGL context\n");
        return Cleanup(1, desktop, hdc, NULL);
    }

    if (!wglMakeCurrent(hdc, hglrc)) {
        printf("Failed to make context current\n");
        return Cleanup(1, desktop, hdc, hglrc);
    }

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        glClearColor(0.33f, 0.5f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!SwapBuffers(hdc)) {
            printf("Failed to swap buffers\n");
            break;
        }

        Sleep(16);
    }

    return Cleanup(0, desktop, hdc, hglrc);
}

HWND GetWallpaperArea() {
    HWND progman = FindWindowA("Progman", NULL);
    if (!progman) {
        printf("Progman window not found.\n");
        return NULL;
    }

    SendMessageA(progman, 0x052C, 0xD, 0);
    SendMessageA(progman, 0x052C, 0xD, 1);

    HWND worker = NULL;
    EnumWindows(FindWorker, (LPARAM) &worker);

    if (!worker) {
        printf("Couldn't find WorkerW window, falling back to Progman.\n");
        return progman;
    }

    return worker;
}

BOOL CALLBACK FindWorker(HWND hwnd, LPARAM lParam) {
    HWND *worker = (HWND *) lParam;
    if (!FindWindowExA(hwnd, 0, "SHELLDLL_DefView", 0))
        return TRUE;
    *worker = FindWindowExA(0, hwnd, "WorkerW", 0);
    if (*worker)
        return FALSE;
    return TRUE;
}

void RestoreDesktopWallpaper(void) {
    HWND progman = FindWindowA("Progman", NULL);
    if (!progman) {
        printf("Failed to find Progman window during restoration.\n");
        return;
    }

    SendMessageA(progman, 0x052C, 0, 0);

    SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, NULL, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

int Cleanup(int status, HWND hwnd, HDC hdc, HGLRC hglrc) {
    if (hglrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hglrc);
    }
    if (hdc && hwnd) {
        ReleaseDC(hwnd, hdc);
    }
    RestoreDesktopWallpaper();
    return status;
}
