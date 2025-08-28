

#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <string>
#include <wrl/client.h>
#include "DDSTextureLoader.h"

struct UnloadedTexture {
    std::wstring path;
};

struct LoadedTexture {
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture;
    std::wstring path;
};


#endif //COMPONENTS_H
