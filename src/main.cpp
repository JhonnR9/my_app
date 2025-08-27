#include <windows.h>
#include "app.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    App app;
    if (!app.Init(hInstance, nCmdShow)) {
        MessageBoxW(NULL, L"Unable to initialize Window", L" Error",  MB_ICONERROR);
        return -1;
    };

    app.Run();
    return 0;
}
