#include <pch.h>
#include "Agent/BehaviorAgent.h"
#include "D_CheckBossExist.h"

void D_CheckBossExist::on_enter()
{
    BehaviorNode::on_enter();
}

void D_CheckBossExist::on_update(float dt)
{
    BehaviorNode* child = children.front();

    BehaviorAgent* boss = nullptr;

    boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");

    //Boss still exists and alive
    if (boss != nullptr)
    {
        child->tick(dt);

        if (child->succeeded())
            on_success();
        else if (child->failed())
            on_failure();
    }
    else
    {
        on_failure();
    }
}
