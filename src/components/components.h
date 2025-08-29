#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <string>
#include <wrl/client.h>
#include "DDSTextureLoader.h"
#include <SimpleMath.h>

struct UnloadedAtlasTexture {
    std::wstring path;
};

struct LoadedAtlasTexture {
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture;
    std::wstring path;
};

struct Transform {
    DirectX::XMFLOAT2 position = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 scale = { 1.0f, 1.0f };
    FLOAT rotation=0.f;
};

struct Sprite {
    std::wstring path;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 uv;
    FLOAT zOrder=0;
};

#endif //COMPONENTS_H
