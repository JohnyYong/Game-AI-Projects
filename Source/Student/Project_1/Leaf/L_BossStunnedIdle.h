#pragma once
#include "BehaviorNode.h"

class L_BossStunnedIdle : public BaseNode<L_BossStunnedIdle>
{
public:

    float stunnedDuration = 0.f;

protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};
