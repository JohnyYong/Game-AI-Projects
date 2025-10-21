#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_BossFindTarget.h"

void L_BossFindTarget::on_enter()
{
	randResult = RNG::range(0, 2);
	on_leaf_enter();
}

void L_BossFindTarget::on_update(float dt)
{
	const auto& heroParty = agent->get_blackboard().get_value<std::vector<BehaviorAgent*>>("heroParty");

	switch (randResult)
	{
	case 0:
		//Find last attacker
		target = agent->get_blackboard().get_value<BehaviorAgent*>("lastAttacker");
		break;
	case 1:
	{
		//Find closest target here
		float closestDist = FLT_MAX;

		for (BehaviorAgent* hero : heroParty)
		{
			float dist = Vec3::Distance(agent->get_position(), hero->get_position());

			if (dist < closestDist)
			{
				closestDist = dist;
				target = hero;
			}
		}
	
	}

		break;
	case 2:
		//Random Target
		target = heroParty.at(RNG::range(0, 2));
		break;
	}

	if (target)
	{
		agent->get_blackboard().set_value("attackTarget", target);
		on_success();

		if (target->GetAgentType() == AgentType::WARRIOR)
		{
			std::cout << "Successfully found WARRIOR to attack\n";

		}
		else if (target->GetAgentType() == AgentType::TANK)
		{
			std::cout << "Successfully found TANK to attack\n";

		}
		else if (target->GetAgentType() == AgentType::CASTER)
		{
			std::cout << "Successfully found CASTER to attack\n";

		}
	}
	else
	{
		on_failure();
	}
	display_leaf_text();
}
