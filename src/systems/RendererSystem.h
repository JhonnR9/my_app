#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <wrl/client.h>
#include <CommonStates.h>
#include <DDSTextureLoader.h>
#include<Effects.h>
#include<SpriteBatch.h>
#include "system.h"
#include <unordered_map>

using Microsoft::WRL::ComPtr;

class RendererSystem : public System {
public:
    explicit RendererSystem(entt::registry *pRegistry);

    void Init() override;

    void Run(float deltaTime) override;

private:
    // DirectXTK helpers
    std::unique_ptr<DirectX::CommonStates> pStates;
    //std::unique_ptr<DirectX::BasicEffect> pBasicEffect;
    std::unique_ptr<DirectX::SpriteBatch> pSpriteBatch;
    //ComPtr<ID3D11InputLayout> pInputLayout;

};


#endif //RENDER_SYSTEM_H
