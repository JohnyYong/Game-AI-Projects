#include <pch.h>
#include "L_MoveToRandomPosition.h"
#include "Agent/BehaviorAgent.h"

void L_MoveToRandomPosition::on_enter()
{
    // set animation, speed, etc

    targetPoint = RNG::world_position();
    //agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_MoveToRandomPosition::on_update(float dt)
{
    if (agent->GetAgentType() == AgentType::BOSS)
    {
        bool isStunned = agent->get_blackboard().get_value<bool>("IsStunned");
        if (isStunned)
        {
            on_failure();
        }
    }


    const auto result = agent->move_toward_point(targetPoint, dt);

    if (result == true)
    {
        on_success();
    }

    display_leaf_text();
}
