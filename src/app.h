//
// Created by jhone on 8/26/2025.
//

#ifndef APP_H
#define APP_H

#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <CommonStates.h>
#include <Effects.h>
#include <SpriteBatch.h>

using Microsoft::WRL::ComPtr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class App {

public:
    App();

    ~App()=default;

    BOOL Init(HINSTANCE hInstance, int nCmdShow);

    BOOL bIsRunning {TRUE};

    void Run();
    void Draw();
    void Resize(UINT width, UINT height);
    static App* GetAppFromWndProc(HWND hWnd, UINT msg, LPARAM lParam);
private:
    HINSTANCE hInst{};
    WCHAR szTitle[100]{};
    WCHAR szWindowClass[100]{};

    ComPtr<ID3D11Device> pd3dDevice = nullptr;
    ComPtr<ID3D11DeviceContext> pImmediateContext = nullptr;
    ComPtr<IDXGISwapChain> pSwapChain = nullptr;
    ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;

    // DirectXTK helpers
    std::unique_ptr<DirectX::CommonStates> states;
    std::unique_ptr<DirectX::BasicEffect> basicEffect;
    ComPtr<ID3D11InputLayout> inputLayout;
    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    ComPtr<ID3D11ShaderResourceView> spriteTexture;

    ATOM InitWindowClass(HINSTANCE hInstance);

    BOOL InitDirect3D(HWND hWnd);

    BOOL CreateRenderTarget();

    BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
    static DXGI_SWAP_CHAIN_DESC CreateSwapChainDesc(HWND hWnd);
    static D3D11_VIEWPORT CreateViewport(FLOAT width, FLOAT height);

};


#endif //APP_H
