#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_HealLowestHPAlly.h"
#include "TreeInfo.h"

void L_HealLowestHPAlly::on_enter()
{
	on_leaf_enter();
	healTarget = agent->get_blackboard().get_value<BehaviorAgent*>("lowestHealthTarget");
	InitHealingOrbs();
}

void L_HealLowestHPAlly::on_update(float dt)
{

	if (healTarget->get_health() == healTarget->get_maxHealth())
	{
		on_failure();
		display_leaf_text();
		return;
	}

	if (healTarget)
	{
		UpdateHealingOrbs(dt);

		Vec3 toTarget = healTarget->get_position() - agent->get_position();
		float faceYaw = atan2f(toTarget.x, toTarget.z);
		agent->set_yaw(faceYaw);

		if (activeHealingOrbs.empty())
		{
			if (healTarget->get_health() + healAmount > healTarget->get_maxHealth())
			{
				healTarget->set_health(100.f);

			}
			else
			{
				healTarget->set_health(healTarget->get_health() + healAmount);
			}
			on_success();
		}
	}
	else
	{
		on_failure();
	}

	display_leaf_text();
}

void L_HealLowestHPAlly::UpdateHealingOrbs(float dt)
{
	for (auto it = activeHealingOrbs.begin(); it != activeHealingOrbs.end();)
	{
		HealingOrbEffect& orb = *it;
		orb.elapsedTime += dt;

		if (orb.elapsedTime < orb.orbitDuration)
		{
			float angle = orb.angleOffset + orb.elapsedTime * 10.0f; // Rotation speed
			float x = cosf(angle) * orb.radius;
			float z = sinf(angle) * orb.radius;
			float y = 1.0f + sinf(orb.elapsedTime * 3.f) * 0.5f; // Bounce while orbiting
			orb.agent->set_position(orb.target->get_position() + Vec3(x, y, z));
		}
		else if (orb.elapsedTime < orb.orbitDuration + orb.riseDuration)
		{
			float riseTime = orb.elapsedTime - orb.orbitDuration;
			Vec3 currentPos = orb.agent->get_position();
			currentPos.y += dt * 10.0f; // Rise speed
			orb.agent->set_position(currentPos);
		}
		else
		{
			agents->destroy_agent(orb.agent);
			it = activeHealingOrbs.erase(it);
			continue;
		}

		++it;
	}
}

void L_HealLowestHPAlly::InitHealingOrbs()
{
	const int orbCount = 6;
	for (int i = 0; i < orbCount; ++i)
	{
		auto orb = agents->create_behavior_agent("HealOrb", BehaviorTreeTypes::Idle, Agent::AgentModel::Ball);
		orb->SetAgentType(AgentType::DEFAULT);
		orb->set_scaling(0.1f);
		orb->set_color(Vec3(0.4f, 1.f, 0.8f)); // Aqua glow
		orb->set_position(healTarget->get_position() + Vec3(0.f, 1.f, 0.f));

		HealingOrbEffect effect;
		effect.agent = orb;
		effect.target = agent;
		effect.elapsedTime = 0.f;
		effect.orbitDuration = 1.5f;
		effect.riseDuration = 1.0f;
		effect.angleOffset = (2 * PI / orbCount) * i;
		effect.radius = 2.0f;
		effect.height = 150.f;

		activeHealingOrbs.push_back(effect);
	}
	audioManager->PlaySoundEffect(L"Assets\\Audio\\Heal.wav");

}
