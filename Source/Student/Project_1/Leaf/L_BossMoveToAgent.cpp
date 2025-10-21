#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_BossMoveToAgent.h"

void L_BossMoveToAgent::on_enter()
{
	on_leaf_enter();
}

void L_BossMoveToAgent::on_update(float dt)
{
	BehaviorAgent* target = agent->get_blackboard().get_value<BehaviorAgent*>("attackTarget");

	if (!target)
	{
		on_failure();
		display_leaf_text();
		return;
	}

	Vec3 targetPos = target->get_position();
	agent->move_toward_point(targetPos, dt);

	float dist = Vec3::Distance(agent->get_position(), targetPos);
	if (dist < 4.f)
	{
		on_success();
	}
	else
	{
		on_failure();
	}

}
