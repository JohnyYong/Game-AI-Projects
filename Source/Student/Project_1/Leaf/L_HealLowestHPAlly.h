#pragma once
#include "BehaviorNode.h"

class L_HealLowestHPAlly : public BaseNode<L_HealLowestHPAlly>
{
    struct HealingOrbEffect
    {
        BehaviorAgent* agent;
        BehaviorAgent* target;
        float elapsedTime = 0.f;
        float orbitDuration = 1.5f;
        float riseDuration = 1.0f;
        float angleOffset = 0.f;
        float radius = 2.0f;
        float height = 2.5f;
    };

    void UpdateHealingOrbs(float dt);
    void InitHealingOrbs();
    BehaviorAgent* healTarget;
    std::vector<HealingOrbEffect> activeHealingOrbs;

protected:
	virtual void on_enter() override;
	virtual void on_update(float dt) override;
private:
	float healAmount = 15.f;

};