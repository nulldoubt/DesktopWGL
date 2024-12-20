#include <Windows.h>
#include <stdio.h>

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

int main(void) {
    printf("Starting...\n");

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(NULL);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "SomeSimpleWindowClass";

    if (!RegisterClassEx(&wc)) {
        printf("Failed to register the window class\n");
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "SomeSimpleWindowClass",
        "Title of the Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL,
        wc.hInstance,
        NULL
    );

    if (!hwnd) {
        printf("Failed to create the window\n");
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    HDC hdc = GetDC(hwnd);

    if (!hdc) {
        printf("Failed to get graphics device context\n");
        DestroyWindow(hwnd);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    HBRUSH hbr = CreateSolidBrush(RGB(0, 30, 130));
    SelectObject(hdc, hbr);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        Rectangle(hdc, 50, 50, 500, 500);
        Sleep(16);

    }

    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}