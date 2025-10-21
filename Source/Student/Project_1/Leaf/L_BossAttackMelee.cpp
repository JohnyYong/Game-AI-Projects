#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_BossAttackMelee.h"
#include "TreeInfo.h"

void L_BossAttackMelee::on_enter()
{
	on_leaf_enter();	
	attackTarget = agent->get_blackboard().get_value<BehaviorAgent*>("attackTarget");
	InitStabProjectiles();
}

void L_BossAttackMelee::on_update(float dt)
{
	if (!attackTarget)
	{
		on_failure();
		display_leaf_text();
		return;
	}
	
	Vec3 toTarget = attackTarget->get_position() - agent->get_position();
	float faceYaw = atan2f(toTarget.x, toTarget.z);
	agent->set_yaw(faceYaw);

	UpdateStabProjectiles(dt);
	if (activeStabObjects.empty())
	{
		attackTarget->ReceiveDamage(20.f);
		on_success();
	}
	display_leaf_text();
}


void L_BossAttackMelee::UpdateStabProjectiles(float dt)
{
	for (int i = static_cast<int>(activeStabObjects.size()) - 1; i >= 0; --i)
	{
		auto& effect = activeStabObjects[i];
		effect.elapsedTime += dt;

		float t = effect.elapsedTime / effect.duration;
		float lungeT = (t < 0.5f) ? (t * 2.f) : (1.f - t) * 2.f;

		Vec3 pos = Vec3::Lerp(effect.origin, effect.target, lungeT);
		effect.agent->set_position(pos);

		if (effect.elapsedTime >= effect.duration)
		{
			agents->destroy_agent(effect.agent);
			activeStabObjects.erase(activeStabObjects.begin() + i);
		}
	}
}

void L_BossAttackMelee::InitStabProjectiles()
{
	Vec3 bossPos = agent->get_position();
	Vec3 toTarget = (attackTarget->get_position() - bossPos);
	float yaw = atan2f(toTarget.x, toTarget.z);
	Vec3 forward = toTarget;
	Vec3 right = Vec3(-forward.z, 0.f, forward.x);

	const int prongCount = 3;
	const float spacing = 0.2f;
	const float stabLength = 1.5f;

	for (int i = 0; i < prongCount; ++i)
	{
		// -1 (left), 0 (center), +1 (right)
		float offsetIndex = (float)(i - 1);

		Vec3 lateralOffset = right * spacing * offsetIndex;
		Vec3 verticalOffset = Vec3(0.f, 8.f, 0.f); //Raising
		Vec3 spawnPos = bossPos + verticalOffset + lateralOffset;

		BehaviorAgent* stab = agents->create_behavior_agent("StabProng", BehaviorTreeTypes::Idle);
		stab->SetAgentType(AgentType::DEFAULT);
		stab->set_scaling(Vec3(0.5f, 1.5f, 0.5f));
		stab->set_color(Vec3(1.f, 0.2f, 0.2f));
		stab->set_pitch(PI * 0.5f); //Apparently lays flat
		stab->set_yaw(yaw);

		BossMeleeStabEffect effect;
		effect.agent = stab;
		effect.origin = spawnPos;
		effect.target = spawnPos + forward * stabLength;
		effect.elapsedTime = 0.f;
		effect.duration = 0.25f;
		effect.targetAgent = attackTarget;
		effect.hasHit = false;

		activeStabObjects.push_back(effect);
	}
}

