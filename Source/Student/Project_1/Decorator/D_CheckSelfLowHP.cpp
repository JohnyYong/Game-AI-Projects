#include <pch.h>
#include "Agent/BehaviorAgent.h"
#include "D_CheckBossExist.h"
#include "D_CheckSelfLowHP.h"

D_CheckSelfLowHP::D_CheckSelfLowHP() : threshold(30.f)
{
}

void D_CheckSelfLowHP::on_enter()
{
	BehaviorNode::on_enter();
	threshold = agent->get_blackboard().get_value<float>("LowHPThreshold");
}

void D_CheckSelfLowHP::on_update(float dt)
{
	if (agent->get_health() <= threshold)
	{
		BehaviorNode* child = children.front();
		child->tick(dt);

		if (child->succeeded()) on_success();
		else if (child->failed()) on_failure();
	}
	else
	{
		on_failure();
	}
}
