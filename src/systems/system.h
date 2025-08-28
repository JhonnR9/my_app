#ifndef SYSTEM_H
#define SYSTEM_H

#include <entt/entt.hpp>

class System {
protected:
    entt::registry *pRegistry;

public :
    explicit System(entt::registry *pRegistry): pRegistry(pRegistry) {
    }

    virtual void Init() =0;
    virtual void Run(float deltaTime) = 0;

    virtual ~System() = default;
};

#endif //SYSTEM_H
