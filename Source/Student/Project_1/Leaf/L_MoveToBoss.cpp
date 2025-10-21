#include "pch.h"
#include "L_MoveToBoss.h"
#include "Agent/BehaviorAgent.h"

void L_MoveToBoss::on_enter()
{

	BehaviorNode::on_leaf_enter();
}

void L_MoveToBoss::on_update(float dt)
{
	const Vec3 currPos = agent->get_position();
	BehaviorAgent* boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");
	targetPos = boss->get_position();
	const float distance = Vec3::Distance(currPos, targetPos);

	float stopDistance = 10.f;


	if (distance <= stopDistance)
	{
		on_success();
	}
	else
	{
		agent->move_toward_point(targetPos, dt);
	}

	display_leaf_text();
}
