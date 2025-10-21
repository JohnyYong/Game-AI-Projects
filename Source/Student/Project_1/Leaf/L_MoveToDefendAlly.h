#pragma once
#include "BehaviorNode.h"

class L_MoveToDefendAlly : public BaseNode<L_MoveToDefendAlly>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
