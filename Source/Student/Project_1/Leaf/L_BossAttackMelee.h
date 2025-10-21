#pragma once
#include "BehaviorNode.h"

class L_BossAttackMelee : public BaseNode<L_BossAttackMelee>
{

    BehaviorAgent* attackTarget;
    struct BossMeleeStabEffect
    {
        BehaviorAgent* agent = nullptr;
        Vec3 origin;
        Vec3 target;
        float elapsedTime = 0.f;
        float duration = 0.5f;
        BehaviorAgent* targetAgent = nullptr;
        bool hasHit = false;
    };
    void UpdateStabProjectiles(float dt);
    void InitStabProjectiles();
    std::vector<BossMeleeStabEffect> activeStabObjects;

protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
