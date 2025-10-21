#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_MoveToDefendAlly.h"

void L_MoveToDefendAlly::on_enter()
{
	on_leaf_enter();
}

void L_MoveToDefendAlly::on_update(float dt)
{
	BehaviorAgent* targetAlly = agent->get_blackboard().get_value<BehaviorAgent*>("lowestHealthTarget");

	if (targetAlly == nullptr)
	{
		on_failure();
		display_leaf_text();
		return;

	}

	Vec3 defendPos = Vec3(targetAlly->get_position().x + 10.f, 0.f, targetAlly->get_position().z + 10.f);
	agent->move_toward_point(defendPos, dt);
	on_success();
	display_leaf_text();
}
