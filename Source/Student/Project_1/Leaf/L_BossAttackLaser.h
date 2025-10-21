#pragma once
#include "BehaviorNode.h"

class L_BossAttackLaser : public BaseNode<L_BossAttackLaser>
{
public:
    struct BossLaserEffect
    {
        BehaviorAgent* orb = nullptr;
        BehaviorAgent* laser = nullptr;

        Vec3 origin;
        Vec3 direction;
        float elapsedTime = 0.f;
        float chargeTime = 0.8f;
        float laserTime = 0.4f;
        float totalTime = 1.2f;
        bool laserFired = false;
    };
    void UpdateBossLasers(float dt);
    void BossLasersInit();

    BehaviorAgent* attackTarget;
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
    std::vector<BossLaserEffect> activeBossLasers;

};
