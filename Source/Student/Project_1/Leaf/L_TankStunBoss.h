#pragma once
#include "BehaviorNode.h"

class L_TankStunBoss : public BaseNode<L_TankStunBoss>
{
public:

    struct StunDropEffect
    {
        BehaviorAgent* agent = nullptr;
        Vec3 targetPos;
        float elapsedTime = 0.f;
        float dropDuration = 0.6f;
    };
    std::vector<StunDropEffect> activeStunDrops;
    void UpdateStunDrops(float dt);
    void InitStunDrops();

    BehaviorAgent* boss;
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
