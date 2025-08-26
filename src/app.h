//
// Created by jhone on 8/26/2025.
//

#ifndef APP_H
#define APP_H

#include <windows.h>
#include <d3d11.h>
#include <iostream>


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class App {
    struct Vertex {
        float position[3];
        float color[4];
    };

public:
    App();

    ~App();

    BOOL Init(HINSTANCE hInstance, int nCmdShow);
    BOOL bIsRunning = TRUE;
    void Run();
    void Resize(UINT width, UINT height);

private:
    HINSTANCE hInst{};
    WCHAR szTitle[100]{};
    WCHAR szWindowClass[100]{};

    ID3D11Device *pd3dDevice = nullptr;
    ID3D11DeviceContext *pImmediateContext = nullptr;
    IDXGISwapChain *pSwapChain = nullptr;
    ID3D11RenderTargetView *g_pRenderTargetView = nullptr;

    ID3D11Buffer *g_pVertexBuffer = nullptr;
    ID3D11InputLayout *g_pVertexLayout = nullptr;
    ID3D11VertexShader *g_pVertexShader = nullptr;
    ID3D11PixelShader *g_pPixelShader = nullptr;
    ID3D11Buffer *g_pIndexBuffer = nullptr;

    ATOM InitWindowClass(HINSTANCE hInstance);

    BOOL InitDirect3D(HWND hWnd);

    BOOL CreateRenderTarget();

    BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
};



#endif //APP_H
