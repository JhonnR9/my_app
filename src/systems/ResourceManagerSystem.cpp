
#include "ResourceManagerSystem.h"
#include <comdef.h>
#include "WICTextureLoader.h"
#include "components/components.h"

void ResourceLoaderSystem::LoadTextures() {
    const auto view = pRegistry->view<UnloadedAtlasTexture>();

    for (auto [entity, unloaded]: view.each()) {
        auto &path = unloaded.path;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

        if (auto it = loadedTextures.find(path); it != loadedTextures.end()) {
            texture = it->second;
        } else {
            auto &pd3dDevice = pRegistry->ctx().get<Microsoft::WRL::ComPtr<ID3D11Device> >();
            auto &pImmediateContext = pRegistry->ctx().get<Microsoft::WRL::ComPtr<ID3D11DeviceContext> >();

            const HRESULT hr = DirectX::CreateWICTextureFromFile(
                pd3dDevice.Get(),
                pImmediateContext.Get(),
                path.c_str(),
                nullptr,
                texture.GetAddressOf()
            );

            if (FAILED(hr)) {
                const _com_error err(hr);
                MessageBoxW(nullptr, err.ErrorMessage(), L"Failed to create Texture", MB_ICONERROR);
                continue;
            }

            loadedTextures[path] = texture;
        }

        pRegistry->remove<UnloadedAtlasTexture>(entity);

        LoadedAtlasTexture loadedTexture;
        loadedTexture.path = path;
        loadedTexture.pTexture = texture;

        if (pRegistry->all_of<LoadedAtlasTexture>(entity)) {
            pRegistry->replace<LoadedAtlasTexture>(entity, std::move(loadedTexture));
        } else {
            pRegistry->emplace<LoadedAtlasTexture>(entity, std::move(loadedTexture));
        }
    }
}

ResourceLoaderSystem::ResourceLoaderSystem(entt::registry *pRegistry): System(pRegistry) {
}

void ResourceLoaderSystem::Init() {
    pRegistry->ctx().emplace<std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>>(loadedTextures);

    LoadTextures();
}

void ResourceLoaderSystem::Run(float deltaTime) {
    LoadTextures();
}
