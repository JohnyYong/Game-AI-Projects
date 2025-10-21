#pragma once
#include "BehaviorNode.h"

class L_WarriorAttackNormal : public BaseNode<L_WarriorAttackNormal>
{
    void UpdateSwordSwingEffects(float dt);
    void InitSwordSwingEffect();

    struct SwordSwingEffect
    {
        BehaviorAgent* agent = nullptr;
        float elapsedTime = 0.f;
        float duration = 0.4f;
        Vec3 startPos;
        Vec3 endPos;
    };

    std::vector<SwordSwingEffect> activeSwordSwings;
    BehaviorAgent* boss;
    bool attackStarted = false;

protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
