#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "D_FindLowestHealthAlly.h"

void D_FindLowestHealthAlly::on_enter()
{
	BehaviorNode::on_enter();
}

void D_FindLowestHealthAlly::on_update(float dt)
{
	//Check for allies
	const auto& allies = agent->get_blackboard().get_value<std::vector<BehaviorAgent*>>("allies");

	BehaviorAgent* target = nullptr;
	float lowestHP = 999;

	//Iterate and check through the health of the allies
	for (auto ally : allies)
	{
		if (ally != agent && ally->get_health() >= 0.f && ally->get_health() != ally->get_maxHealth())
		{
			if (ally->get_health() < lowestHP)
			{
				lowestHP = ally->get_health();
				target = ally;
			}
		}
	}


	//Applies only for healing effect
	if (agent->GetAgentType() == AgentType::CASTER)
	{
		if (agent->get_health() < lowestHP)
		{
			lowestHP = agent->get_health();
			target = agent;
		}
	}




	if (target)
	{
		if (target->get_health() == target->get_maxHealth())
		{
			on_failure();
			std::cout << "Everyone on max health\n";
			return;
		}
		agent->get_blackboard().set_value("lowestHealthTarget", target);

		//Decorator node, must have tick
		BehaviorNode* child = children.front();
		child->tick(dt);

		if (child->succeeded()) on_success();
		else if (child->failed()) on_failure();
	}
	else
	{
		std::cout << "Cannot find lowest hp target\n";

		on_failure();
	}
}
