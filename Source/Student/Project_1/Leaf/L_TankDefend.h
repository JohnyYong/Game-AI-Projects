#pragma once
#include "BehaviorNode.h"

class L_TankDefend : public BaseNode<L_TankDefend>
{
public:
    void UpdateDefendOrbs(float dt);
    void InitDefendOrbs();
    struct DefendOrbEffect
    {
        BehaviorAgent* agent = nullptr;
        BehaviorAgent* caster = nullptr;
        float elapsedTime = 0.f;
        float angleOffset = 0.f;
        float radius = 1.5f;
        float duration = 3.0f;
        Vec3 center;
        Vec3 facingDir;
    };
    std::vector<DefendOrbEffect> activeDefendOrbs;
    bool defendStarted = false;


protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
