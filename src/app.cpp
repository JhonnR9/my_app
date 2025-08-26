//
// Created by jhone on 8/26/2025.
//

#include <d3dcompiler.h>
#include <comdef.h>
#include <algorithm>
#include "app.h"
#include <chrono>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    App *pApp = nullptr;

    if (msg == WM_NCCREATE) {
        const auto *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        pApp = static_cast<App *>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
    } else {
        pApp = reinterpret_cast<App *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pApp) {
        switch (msg) {
            case WM_SIZE: {
                UINT w = LOWORD(lParam);
                UINT h = HIWORD(lParam);
                pApp->Resize(w, h);
                break;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
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
}

App::~App() {
    if (g_pRenderTargetView) {
        g_pRenderTargetView->Release();
        g_pRenderTargetView = nullptr;
    }
    if (pSwapChain) {
        pSwapChain->Release();
        pSwapChain = nullptr;
    }
    if (g_pPixelShader) {
        g_pPixelShader->Release();
        g_pPixelShader = nullptr;
    }
    if (g_pVertexShader) {
        g_pVertexShader->Release();
        g_pVertexShader = nullptr;
    }
    if (g_pVertexLayout) {
        g_pVertexLayout->Release();
        g_pVertexLayout = nullptr;
    }
    if (g_pVertexBuffer) {
        g_pVertexBuffer->Release();
        g_pVertexBuffer = nullptr;
    }
    if (pImmediateContext) {
        pImmediateContext->Release();
        pImmediateContext = nullptr;
    }
    if (pd3dDevice) {
        pd3dDevice->Release();
        pd3dDevice = nullptr;
    }
    if (g_pIndexBuffer) {
        g_pIndexBuffer->Release();
        g_pIndexBuffer = nullptr;
    }
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

        if (!pImmediateContext || !g_pRenderTargetView) continue;

        constexpr float clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

        pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
        pImmediateContext->IASetInputLayout(g_pVertexLayout);
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
        pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

        pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        pImmediateContext->DrawIndexed(6, 0, 0);


        if (const HRESULT hr = pSwapChain->Present(1, 0); FAILED(hr)) {
            MessageBoxW(nullptr, L"Error presenting swap chain", L"Error", MB_ICONERROR);
            continue;
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

BOOL App::InitDirect3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

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
    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.f;
    vp.TopLeftY = 0.f;
    vp.Width = static_cast<FLOAT>(std::max<LONG>(rc.right - rc.left, 1));
    vp.Height = static_cast<FLOAT>(std::max<LONG>(rc.bottom - rc.top, 1));
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    pImmediateContext->RSSetViewports(1, &vp);

    // Create vertex and index buffers
    constexpr Vertex vertices[] = {
        {{-0.5f, 0.5f, 0.0f}, {0.f, 1.f, 1.f, 1.f}},
        {{0.5f, 0.5f, 0.0f}, {1.f, 0.f, 1.f, 1.f}},
        {{0.5f, -0.5f, 0.0f}, {1.f, 1.f, 0.f, 1.f}},
        {{-0.5f, -0.5f, 0.0f}, {0.f, 0.f, 1.f, 1.f}},
    };

    constexpr UINT indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // Index Buffer
    D3D11_BUFFER_DESC ibd{};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinitData{};
    iinitData.pSysMem = indices;

    hr = pd3dDevice->CreateBuffer(&ibd, &iinitData, &g_pIndexBuffer);
    if (FAILED(hr)) {
        _com_error err(hr);
        MessageBoxW(nullptr, err.ErrorMessage(), L"Failed to create index buffer", MB_ICONERROR);
        return false;
    }

    // Vertex Buffer
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = vertices;

    hr = pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
    if (FAILED(hr)) {
        _com_error err(hr);
        MessageBoxW(nullptr, err.ErrorMessage(), L"Failed to create vertex buffer", MB_ICONERROR);
        return false;
    }

    // Compile and create shaders
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* error = nullptr;

    // Vertex Shader
    hr = D3DCompileFromFile(L"resources/shader.hlsl", nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vsBlob, &error);
    if (FAILED(hr)) {
        if (error) {
            MessageBoxA(nullptr, static_cast<char*>(error->GetBufferPointer()), "Vertex Shader Compilation Error", MB_ICONERROR);
            error->Release();
        } else {
            MessageBoxA(nullptr, "Unknown error during vertex shader compilation.", "Vertex Shader Compilation Error", MB_ICONERROR);
        }
        return false;
    }

    hr = pd3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    if (FAILED(hr)) {
        _com_error err(hr);
        MessageBoxW(nullptr, err.ErrorMessage(), L"Failed to create vertex shader", MB_ICONERROR);
        vsBlob->Release();
        return false;
    }

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = pd3dDevice->CreateInputLayout(layout, _countof(layout),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &g_pVertexLayout);

    vsBlob->Release(); // Always release blobs after use

    if (FAILED(hr)) {
        MessageBoxA(nullptr, "Failed to create input layout", "Input Layout Error", MB_ICONERROR);
        return false;
    }

    // Pixel Shader
    hr = D3DCompileFromFile(L"resources/shader.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, &error);
    if (FAILED(hr)) {
        if (error) {
            MessageBoxA(nullptr, static_cast<char*>(error->GetBufferPointer()), "Pixel Shader Compilation Error", MB_ICONERROR);
            error->Release();
        } else {
            MessageBoxA(nullptr, "Unknown error during pixel shader compilation.", "Pixel Shader Error", MB_ICONERROR);
        }
        return false;
    }

    hr = pd3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_pPixelShader);
    psBlob->Release(); // Release after use

    if (FAILED(hr)) {
        _com_error err(hr);
        MessageBoxW(nullptr, err.ErrorMessage(), L"Failed to create pixel shader", MB_ICONERROR);
        return false;
    }

    return true;
}


BOOL App::CreateRenderTarget() {
    ID3D11Texture2D *pBackBuffer = nullptr;
    HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pBackBuffer));
    if (FAILED(hr) || !pBackBuffer) return false;

    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return false;

    pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
    return true;
}

void App::Resize(const UINT width, const UINT height) {
    if (!pSwapChain) return;

    pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
    if (g_pRenderTargetView) {
        g_pRenderTargetView->Release();
        g_pRenderTargetView = nullptr;
    }

    if (const HRESULT hr = pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0); FAILED(hr)) {
        const _com_error err(hr);
        MessageBoxW(nullptr,err.ErrorMessage(),L"Failed to resize swap chain",MB_ICONERROR);
    }
    CreateRenderTarget();

    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<FLOAT>(max(width, 1u));
    vp.Height = static_cast<FLOAT>(max(height, 1u));
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    pImmediateContext->RSSetViewports(1, &vp);
}
