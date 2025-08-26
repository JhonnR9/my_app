#include <windows.h>
#include "app.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    App app;
    if (!app.Init(hInstance, nCmdShow)) return -1;

    app.Run();

    return 0;
}
/*

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    while (MessageBoxW(NULL, L"ocorreu um erro fatal!", L"System Error", MB_RETRYCANCEL | MB_ICONERROR) == IDRETRY);
    return 0;
}*/
