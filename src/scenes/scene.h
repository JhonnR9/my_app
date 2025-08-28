//
// Created by jhone on 8/27/2025.
//

#ifndef SCENE_H
#define SCENE_H
#include <entt/entt.hpp>

class Scene {
protected:
    entt::registry *pRegistry{nullptr};

public:
    explicit Scene(entt::registry *pRegistry): pRegistry(pRegistry) {
    };

    virtual ~Scene() = default;

    virtual void Init() =0;

    virtual void Render() =0;
};


#endif //SCENE_H
