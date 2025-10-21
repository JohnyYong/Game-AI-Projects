#pragma once
#include "BehaviorNode.h"

class L_WarriorAttackSpecial : public BaseNode<L_WarriorAttackSpecial>
{
public:
    struct FlyingBladeEffect
    {
        BehaviorAgent* agent = nullptr;
        Vec3 velocity;
        float lifeTime = 0.f;
        float maxDuration = 1.0f;
        BehaviorAgent* target = nullptr;
    };
    std::vector<FlyingBladeEffect> activeFlyingBlades;
    void UpdateFlyingBlades(float dt);
    void InitFlyingBlades();
    BehaviorAgent* boss;

protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
