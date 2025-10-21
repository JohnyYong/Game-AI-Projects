#pragma once
#include "BehaviorNode.h"

class L_BossFindTarget : public BaseNode<L_BossFindTarget>
{
    int randResult = 0;
    BehaviorAgent* target;
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
