#include "RendererSystem.h"

#include "components/components.h"

RendererSystem::RendererSystem(entt::registry *pRegistry): System(pRegistry) {
}

void RendererSystem::Init() {
    const auto &pd3dDevice = pRegistry->ctx().get<ComPtr<ID3D11Device> >();
    const auto &pImmediateContext = pRegistry->ctx().get<ComPtr<ID3D11DeviceContext> >();

    pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pImmediateContext.Get());
    pStates = std::make_unique<DirectX::CommonStates>(pd3dDevice.Get());
}

void RendererSystem::Run(float deltaTime) {
    const auto &loadedTextures = pRegistry->ctx().get<std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<
        ID3D11ShaderResourceView> > >();

    const auto view = pRegistry->view<Sprite, Transform>();

    pSpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, pStates->NonPremultiplied());

    for (const auto& [entity, sprite, transform]: view.each()) {
        auto it = loadedTextures.find(sprite.path);
        if (it == loadedTextures.end()) {
            continue;
        }

        const auto texture = it->second.Get();

        DirectX::XMVECTOR posVec = DirectX::XMLoadFloat2(&transform.position);
        const DirectX::XMVECTOR scaleVec = DirectX::XMLoadFloat2(&transform.scale);

        DirectX::XMFLOAT2 position{};
        DirectX::XMFLOAT2 scale{};
        DirectX::XMStoreFloat2(&position, posVec);
        DirectX::XMStoreFloat2(&scale, scaleVec);
        DirectX::XMVECTOR colorVec = DirectX::XMLoadFloat4(&sprite.color);
        DirectX::XMFLOAT2 origin = { 0.0f, 0.0f };

        pSpriteBatch->Draw(
           texture,
             XMLoadFloat2(&position),
           nullptr,
           colorVec,
           transform.rotation,
           XMLoadFloat2(&origin),
           XMLoadFloat2(&scale),
           DirectX::SpriteEffects_None,
           sprite.zOrder
       );
    }
    pSpriteBatch->End();

}
