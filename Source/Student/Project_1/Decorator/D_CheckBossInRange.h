#pragma once
#include "BehaviorNode.h"

class D_CheckBossInRange : public BaseNode<D_CheckBossInRange>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
