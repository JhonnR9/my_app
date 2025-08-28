

#ifndef RESOURCE_LOADER_SYSTEM_H
#define RESOURCE_LOADER_SYSTEM_H
#include <unordered_map>
#include <wrl/client.h>

#include "DDSTextureLoader.h"
#include "system.h"


class ResourceLoaderSystem final : public System{
    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> loadedTextures;

    void LoadTextures();
public:
    explicit ResourceLoaderSystem(entt::registry *pRegistry);
    void Init() override;
    void Run(float deltaTime) override;
};



#endif //RESOURCE_LOADER_SYSTEM_H
