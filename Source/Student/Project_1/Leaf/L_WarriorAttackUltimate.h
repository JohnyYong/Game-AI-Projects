#pragma once
#include "BehaviorNode.h"

class L_WarriorAttackUltimate : public BaseNode<L_WarriorAttackUltimate>
{
public:
    void UpdateTornadoBeams(float dt);
    void InitTornadoBeams();

    struct TornadoBeamEffect
    {
        BehaviorAgent* agent = nullptr;
        Vec3 velocity;
        float lifeTime = 0.f;
        float maxDuration = 1.5f;
        BehaviorAgent* caster = nullptr;
    };
    std::vector<TornadoBeamEffect> activeTornadoBeams;
    BehaviorAgent* boss;
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
