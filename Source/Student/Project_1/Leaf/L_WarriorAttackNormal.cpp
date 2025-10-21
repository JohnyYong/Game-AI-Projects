#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_WarriorAttackNormal.h"
#include "TreeInfo.h"

void L_WarriorAttackNormal::on_enter()
{
	attackStarted = true;

	on_leaf_enter();
	boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");
	InitSwordSwingEffect();
}

void L_WarriorAttackNormal::on_update(float dt)
{	

	if (boss)
	{
		Vec3 toTarget = boss->get_position() - agent->get_position();
		float faceYaw = atan2f(toTarget.x, toTarget.z);
		agent->set_yaw(faceYaw);
	}
	if (!attackStarted) return;

	UpdateSwordSwingEffects(dt);

	if (activeSwordSwings.empty())
	{
		agent->DealDamageToBoss(5.f, agent);
		on_success();
		attackStarted = false;
	}

	display_leaf_text();
	
}

void L_WarriorAttackNormal::UpdateSwordSwingEffects(float dt)
{
	for (int i = static_cast<int>(activeSwordSwings.size()) - 1; i >= 0; --i)
	{
		SwordSwingEffect& effect = activeSwordSwings[i];
		effect.elapsedTime += dt;

		float t = effect.elapsedTime / effect.duration;
		t = std::min(t, 1.0f);

		Vec3 pos = Vec3::Lerp(effect.startPos, effect.endPos, t);
		effect.agent->set_position(pos);

		float rotation = sinf(t * 3.1415f) * 1.0f;
		effect.agent->set_roll(rotation);

		if (t >= 1.0f)
		{
			agents->destroy_agent(effect.agent);
			activeSwordSwings.erase(activeSwordSwings.begin() + i);
		}
	}
}

void L_WarriorAttackNormal::InitSwordSwingEffect()
{
	BehaviorAgent* swing = agents->create_behavior_agent("SwingEffect", BehaviorTreeTypes::Idle);
	swing->SetAgentType(AgentType::DEFAULT);
	swing->set_scaling(Vec3(0.3f, 2.0f, 0.3f));
	swing->set_color(Vec3(1.0f, 1.0f, 0.3f));

	Vec3 start = agent->get_position();
	Vec3 startPos = start + Vec3(0.f, 6.f, 0.f);
	Vec3 endPos = start + Vec3(0.f, 0.5f, 2.5f);
	swing->set_position(startPos);

	swing->set_yaw(agent->get_yaw());
	swing->set_pitch(PI / 2.f);
	swing->set_roll(PI / 4.f);

	SwordSwingEffect effect;
	effect.agent = swing;
	effect.elapsedTime = 0.f;
	effect.duration = 0.4f;
	effect.startPos = startPos;
	effect.endPos = endPos;

	activeSwordSwings.push_back(effect);
	audioManager->PlaySoundEffect(L"Assets\\Audio\\NormalAttack.wav");
}
