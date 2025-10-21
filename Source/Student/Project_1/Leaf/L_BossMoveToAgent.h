#pragma once
#include "BehaviorNode.h"

class L_BossMoveToAgent : public BaseNode<L_BossMoveToAgent>
{

protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
