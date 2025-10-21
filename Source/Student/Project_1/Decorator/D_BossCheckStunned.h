#pragma once
#include "BehaviorNode.h"

class D_BossCheckStunned : public BaseNode<D_BossCheckStunned>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
