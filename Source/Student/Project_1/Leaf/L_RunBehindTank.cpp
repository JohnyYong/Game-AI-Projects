#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_RunBehindTank.h"

void L_RunBehindTank::on_enter()
{
	BehaviorNode::on_leaf_enter();

	BehaviorAgent* tank = agent->get_blackboard().get_value<BehaviorAgent*>("tank");
	if (!tank)
	{
		on_failure();
		display_leaf_text();
		return;
	}

	targetPos = tank->get_position();

	//Vec3 tankPos = tank->get_position();
	//Vec3 tankDir = tank->get_forward_vector();

	//targetPos = tankPos - tankDir * 10.f;
}

void L_RunBehindTank::on_update(float dt)
{
	const Vec3 currPos = agent->get_position();
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
