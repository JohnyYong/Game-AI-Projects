#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_WarriorAttackSpecial.h"
#include "TreeInfo.h"

void L_WarriorAttackSpecial::on_enter()
{
	on_leaf_enter();
	boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");
	InitFlyingBlades();
}

void L_WarriorAttackSpecial::on_update(float dt)
{
	if (boss)
	{
		Vec3 toTarget = boss->get_position() - agent->get_position();
		float faceYaw = atan2f(toTarget.x, toTarget.z);
		agent->set_yaw(faceYaw);
	}
	UpdateFlyingBlades(dt);

	if (activeFlyingBlades.empty())
	{
		agent->DealDamageToBoss(15.f, agent);
		on_success();
	}
	display_leaf_text();
}

void L_WarriorAttackSpecial::UpdateFlyingBlades(float dt)
{
	for (int i = static_cast<int>(activeFlyingBlades.size()) - 1; i >= 0; --i)
	{
		FlyingBladeEffect& effect = activeFlyingBlades[i];
		if (!effect.agent) continue;

		effect.lifeTime += dt;

		Vec3 current = effect.agent->get_position();
		Vec3 nextPos = current + effect.velocity * dt;
		effect.agent->set_position(nextPos);

		if (effect.lifeTime >= effect.maxDuration)
		{
			agents->destroy_agent(effect.agent);
			activeFlyingBlades.erase(activeFlyingBlades.begin() + i);
		}
	}
}

void L_WarriorAttackSpecial::InitFlyingBlades()
{
	Vec3 origin = agent->get_position();
	Vec3 toTarget = (boss->get_position() - origin);
	float yaw = atan2f(toTarget.x, toTarget.z);

	for (int i = 0; i < 3; ++i)
	{
		BehaviorAgent* blade = agents->create_behavior_agent("FlyingBlade", BehaviorTreeTypes::Idle);
		blade->SetAgentType(AgentType::DEFAULT);
		blade->set_scaling(Vec3(0.2f, 1.2f, 0.2f));
		blade->set_color(Vec3(1.f, 0.9f, 0.3f));

		float lateral = (i - 1) * 10.0f;
		Vec3 bladeStart = origin + Vec3(0.f, lateral, -3.0f);

		blade->set_position(bladeStart);
		blade->set_yaw(yaw);
		blade->set_pitch(PI / 3.f); 

		Vec3 dir = (boss->get_position() - bladeStart);
		Vec3 velocity = dir * 10.f; 

		FlyingBladeEffect effect;
		effect.agent = blade;
		effect.velocity = velocity;
		effect.lifeTime = 0.f;
		effect.maxDuration = 1.0f;
		effect.target = boss;

		activeFlyingBlades.push_back(effect);
	}
}