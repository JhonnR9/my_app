
#ifndef MY_SCENE_H
#define MY_SCENE_H
#include "scene.h"


class MyScene final : public Scene {
public:
    explicit MyScene(entt::registry *registry);

    void Init() override;

    void Render() override;
};


#endif //MY_SCENE_H
