#pragma once
#include "BehaviorNode.h"

class D_CheckBossExist : public BaseNode<D_CheckBossExist>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
