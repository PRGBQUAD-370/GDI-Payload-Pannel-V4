#include <windows.h>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <cmath>
#include <mmsystem.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")

std::atomic<bool> running(false);
std::atomic<int> mode(0);

void PlayMP3(const char* file) {
    mciSendString("close music", NULL, 0, NULL);

    char cmd[256];
    wsprintf(cmd, "open \"%s\" type mpegvideo alias music", file);
    mciSendString(cmd, NULL, 0, NULL);

    mciSendString("play music repeat", NULL, 0, NULL);
}

void StopMP3() {
    mciSendString("close music", NULL, 0, NULL);
}

void GDIWorker() {
    HDC hdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    int scroll = 0;

    while (running) {
        switch (mode.load()) {

        case 1: PatBlt(hdc, 0, 0, w, h, PATINVERT); Sleep(80); break;

        case 2:
            for (int x = 0; x < w; x += 8) {
                BitBlt(hdc, x, rand() % 40, 8, h, hdc, x, 0, SRCCOPY);
            }
            Sleep(30);
            break;

        case 3:
            for (int x = 0; x < w; x += 4) {
                int y = (int)(20 * sin(GetTickCount() / 150.0 + x));
                BitBlt(hdc, x, y, 4, h, hdc, x, 0, SRCCOPY);
            }
            Sleep(25);
            break;

        case 4:
            BitBlt(hdc, (int)(12 * sin(GetTickCount() / 120.0)), 0, w, h, hdc, 0, 0, SRCCOPY);
            Sleep(16);
            break;

        case 5:
            BitBlt(hdc, 0, (int)(12 * cos(GetTickCount() / 120.0)), w, h, hdc, 0, 0, SRCCOPY);
            Sleep(16);
            break;

        case 6: // Move Right
            scroll = (scroll + 8) % w;
            BitBlt(hdc, scroll, 0, w - scroll, h, hdc, 0, 0, SRCCOPY);
            BitBlt(hdc, 0, 0, scroll, h, hdc, w - scroll, 0, SRCCOPY);
            Sleep(16);
            break;

        case 7: // Move Left
            scroll = (scroll + 8) % w;
            BitBlt(hdc, 0, 0, w - scroll, h, hdc, scroll, 0, SRCCOPY);
            BitBlt(hdc, w - scroll, 0, scroll, h, hdc, 0, 0, SRCCOPY);
            Sleep(16);
            break;

        case 8: // Screen Shaker
            BitBlt(hdc, (rand() % 20) - 10, 0, w, h, hdc, 0, 0, SRCCOPY);
            Sleep(10);
            break;
        }
    }

    ReleaseDC(NULL, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CREATE:
        CreateWindow("BUTTON","Invert",WS_VISIBLE|WS_CHILD,20,20,200,30,hwnd,(HMENU)1,0,0);
        CreateWindow("BUTTON","Melt",WS_VISIBLE|WS_CHILD,20,55,200,30,hwnd,(HMENU)2,0,0);
        CreateWindow("BUTTON","Wave Melt",WS_VISIBLE|WS_CHILD,20,90,200,30,hwnd,(HMENU)3,0,0);
        CreateWindow("BUTTON","Move Smooth",WS_VISIBLE|WS_CHILD,20,125,200,30,hwnd,(HMENU)4,0,0);
        CreateWindow("BUTTON","Move Up/Down",WS_VISIBLE|WS_CHILD,20,160,200,30,hwnd,(HMENU)5,0,0);
        CreateWindow("BUTTON","Move Right",WS_VISIBLE|WS_CHILD,20,195,200,30,hwnd,(HMENU)6,0,0);
        CreateWindow("BUTTON","Move Left",WS_VISIBLE|WS_CHILD,20,230,200,30,hwnd,(HMENU)7,0,0);
        CreateWindow("BUTTON","Screen Shaker",WS_VISIBLE|WS_CHILD,20,265,200,30,hwnd,(HMENU)8,0,0);

        CreateWindow("BUTTON","Audio 1",WS_VISIBLE|WS_CHILD,250,20,200,30,hwnd,(HMENU)101,0,0);
        CreateWindow("BUTTON","Audio 2",WS_VISIBLE|WS_CHILD,250,55,200,30,hwnd,(HMENU)102,0,0);
        CreateWindow("BUTTON","Audio 3",WS_VISIBLE|WS_CHILD,250,90,200,30,hwnd,(HMENU)103,0,0);
        CreateWindow("BUTTON","Audio 4",WS_VISIBLE|WS_CHILD,250,125,200,30,hwnd,(HMENU)104,0,0);
        CreateWindow("BUTTON","Audio 5",WS_VISIBLE|WS_CHILD,250,160,200,30,hwnd,(HMENU)105,0,0);

        CreateWindow("BUTTON","STOP",WS_VISIBLE|WS_CHILD,150,420,200,40,hwnd,(HMENU)999,0,0);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) >= 1 && LOWORD(wParam) <= 8) {
            mode = LOWORD(wParam);
            if (!running) {
                running = true;
                std::thread(GDIWorker).detach();
            }
        }

        switch (LOWORD(wParam)) {
        case 101: PlayMP3("audio1.mp3"); break;
        case 102: PlayMP3("audio2.mp3"); break;
        case 103: PlayMP3("audio3.mp3"); break;
        case 104: PlayMP3("audio4.mp3"); break;
        case 105: PlayMP3("audio5.mp3"); break;
        case 999:
            running = false;
            StopMP3();
            break;
        }
        break;

    case WM_DESTROY:
        running = false;
        StopMP3();
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "GDIPanel500";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "GDIPanel500",
        "GDI Payload Pannel V4",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        200, 100, 500, 500,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
