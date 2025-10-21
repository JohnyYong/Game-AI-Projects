#pragma once
#include "BehaviorNode.h"

class D_CheckSelfLowHP : public BaseNode<D_CheckSelfLowHP>
{
public:
    D_CheckSelfLowHP();

protected:
    float threshold;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
