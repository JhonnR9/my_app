

#ifndef APP_H
#define APP_H

#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <CommonStates.h>
#include <Effects.h>
#include <SpriteBatch.h>
#include "scenes/scene.h"

using Microsoft::WRL::ComPtr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class App {

public:
    App();
    ~App()=default;
    // For game loop
    BOOL bIsRunning {TRUE};

    // Lifecycle
    BOOL Init(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Draw();
    void Resize(UINT width, UINT height);
    static App* GetAppFromWndProc(HWND hWnd, UINT msg, LPARAM lParam);
private:
    // Flags end window control
    HINSTANCE hInst{};
    WCHAR szTitle[100]{};
    WCHAR szWindowClass[100]{};
    BOOL useVsync {TRUE};

    // DirectX context
    ComPtr<ID3D11Device> pd3dDevice;
    ComPtr<ID3D11DeviceContext> pImmediateContext;
    ComPtr<IDXGISwapChain> pSwapChain;
    ComPtr<ID3D11RenderTargetView> pRenderTargetView;

    // DirectXTK helpers -> FIXME move this for render backend system
    std::unique_ptr<DirectX::CommonStates> pStates;
    std::unique_ptr<DirectX::BasicEffect> pBasicEffect;
    std::unique_ptr<DirectX::SpriteBatch> pSpriteBatch;
    ComPtr<ID3D11InputLayout> pInputLayout;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSpriteTexture;



    ATOM InitWindowClass(HINSTANCE hInstance);
    BOOL InitDirect3D(HWND hWnd);
    BOOL CreateRenderTarget();
    BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
    static DXGI_SWAP_CHAIN_DESC CreateSwapChainDesc(HWND hWnd);
    static D3D11_VIEWPORT CreateViewport(FLOAT width, FLOAT height);

    // Scene Control -> FIXME chance this for accept multiple scenes
    std::unique_ptr<Scene> pScene;

    // EnTT
    std::unique_ptr<entt::registry> pRegistry;


};


#endif //APP_H
