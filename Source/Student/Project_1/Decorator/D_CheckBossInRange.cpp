#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "D_CheckBossInRange.h"

void D_CheckBossInRange::on_enter()
{
	BehaviorNode::on_enter();
}

void D_CheckBossInRange::on_update(float dt)
{
    BehaviorAgent* boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");

    BehaviorNode* child = children.front();

    if (boss)
    {
        float distSq = Vec3::Distance(agent->get_position(), boss->get_position());
        
        if (distSq <= 10.0f) 
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

    display_leaf_text();
}
