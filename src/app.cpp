
#include <d3dcompiler.h>
#include <comdef.h>
#include <algorithm>
#include "app.h"
#include <chrono>
#include <SimpleMath.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>

#include "scenes/my_scene.h"

using namespace DirectX;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (App *pApp = App::GetAppFromWndProc(hWnd, msg, lParam)) {
        switch (msg) {
            case WM_SIZE: {
                UINT w = LOWORD(lParam);
                UINT h = HIWORD(lParam);
                pApp->Resize(w, h);
                break;
            }
            case WM_DESTROY:
                pApp->bIsRunning = FALSE;
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


App::App() {
    wcscpy_s(szTitle, L"My App");
    wcscpy_s(szWindowClass, L"D3D11WindowClass");
    pRegistry = std::make_unique<entt::registry>();
    pScene = std::make_unique<MyScene>(pRegistry.get());
}

ATOM App::InitWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
    wcex.lpszClassName = szWindowClass;
    return RegisterClassExW(&wcex);
}

BOOL App::InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    HWND hWnd = CreateWindowW(
        szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, this);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    if (!InitDirect3D(hWnd)) return FALSE;

    return TRUE;
}

DXGI_SWAP_CHAIN_DESC App::CreateSwapChainDesc(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    return sd;
}

D3D11_VIEWPORT App::CreateViewport(FLOAT width, FLOAT height) {
    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.f;
    vp.TopLeftY = 0.f;
    vp.Width = (std::max<FLOAT>(width, 1));
    vp.Height =(std::max<FLOAT>(height, 1));
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    return vp;
}

App *App::GetAppFromWndProc(HWND hWnd, UINT msg, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        const auto *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        auto pApp = static_cast<App *>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
        return pApp;
    }
    return reinterpret_cast<App *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

BOOL App::Init(HINSTANCE hInstance, int nCmdShow) {
    InitWindowClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;
    return TRUE;
}

void App::Run() {
    MSG msg{};
    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    while (bIsRunning) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        if (!pImmediateContext || !pRenderTargetView) continue;

        Draw();
        if (const HRESULT hr = pSwapChain->Present(useVsync, 0); FAILED(hr)) {
            MessageBoxW(nullptr, L"Error presenting swap chain", L"Error", MB_ICONERROR);
            break;
        }

        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();

        if (elapsedTime >= 1.0f) {
            const float fps = static_cast<float>(frameCount) / elapsedTime;
            WCHAR fpsText[64];
            swprintf_s(fpsText, L"My App - FPS: %.2f", fps);
            SetWindowTextW(FindWindowW(szWindowClass, nullptr), fpsText);

            frameCount = 0;
            startTime = currentTime;
        }
    }
}

void App::Draw() {
    constexpr FLOAT clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

    pImmediateContext->ClearRenderTargetView(pRenderTargetView.Get(), clearColor);

    pImmediateContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
    pBasicEffect->Apply(pImmediateContext.Get());
    pImmediateContext->IASetInputLayout(pInputLayout.Get());

    pSpriteBatch->Begin();

    pSpriteBatch->Draw(pSpriteTexture.Get(), XMFLOAT2(100, 100));

    pSpriteBatch->End();
}

BOOL App::InitDirect3D(HWND hWnd) {
    const DXGI_SWAP_CHAIN_DESC sd = CreateSwapChainDesc(hWnd);

    constexpr D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL obtained{};

    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        levels, _countof(levels), D3D11_SDK_VERSION,
        &sd, &pSwapChain, &pd3dDevice, &obtained, &pImmediateContext);

    if (FAILED(hr)) {
        _com_error err(hr);
        MessageBoxW(nullptr, err.ErrorMessage(), L"Failed to create D3D11 device and swap chain", MB_ICONERROR);
        return false;
    }

    if (!CreateRenderTarget()) {
        MessageBoxW(nullptr, L"Failed to create render target", L"Error", MB_ICONERROR);
        return false;
    }

    // Set viewport
    RECT rc{};
    GetClientRect(hWnd, &rc);
    const FLOAT Width = static_cast<FLOAT>(std::max<LONG>(rc.right - rc.left, 1));
    const FLOAT Height = static_cast<FLOAT>(std::max<LONG>(rc.bottom - rc.top, 1));

    const D3D11_VIEWPORT vp = CreateViewport(Width, Height);
    pImmediateContext->RSSetViewports(1, &vp);

    pStates = std::make_unique<CommonStates>(pd3dDevice.Get());

    pBasicEffect = std::make_unique<BasicEffect>(pd3dDevice.Get());
    pBasicEffect->SetVertexColorEnabled(true);

    pRegistry->ctx().emplace<ComPtr<ID3D11Device>>(pd3dDevice);
    pRegistry->ctx().emplace<ComPtr<ID3D11DeviceContext>>(pImmediateContext);


    void const *shaderByteCode;
    size_t byteCodeLength;
    pBasicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    pd3dDevice->CreateInputLayout(VertexPositionColor::InputElements,
                                  VertexPositionColor::InputElementCount,
                                  shaderByteCode, byteCodeLength,
                                  &pInputLayout);

    pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pImmediateContext.Get());

    hr = CreateWICTextureFromFile(
        pd3dDevice.Get(),
        pImmediateContext.Get(),
        L"resources/enemy2.png",
        nullptr,
        &pSpriteTexture
    );

    if (FAILED(hr)) {
        MessageBoxW(nullptr, L"Failed to load texture", L"Erro", MB_ICONERROR);
        return FALSE;
    }
    return true;
}


BOOL App::CreateRenderTarget() {
    ID3D11Texture2D *pBackBuffer = nullptr;
    HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pBackBuffer));
    if (FAILED(hr) || !pBackBuffer) return false;

    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return false;
    pImmediateContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);

    return true;
}

void App::Resize(const UINT width, const UINT height) {
    if (!pSwapChain) return;

    pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
    pRenderTargetView.Reset();

    if (const HRESULT hr = pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0); FAILED(hr)) {
        const _com_error err(hr);
        MessageBoxW(nullptr, err.ErrorMessage(), L"Failed to resize swap chain",MB_ICONERROR);
    }
    CreateRenderTarget();

    // Set viewport
    const D3D11_VIEWPORT vp = CreateViewport(static_cast<FLOAT>(width), static_cast<FLOAT>(height));
    pImmediateContext->RSSetViewports(1, &vp);
}
