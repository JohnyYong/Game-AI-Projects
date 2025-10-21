#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_WarriorAttackUltimate.h"
#include "TreeInfo.h"

void L_WarriorAttackUltimate::on_enter()
{
	on_leaf_enter();
    boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");
    InitTornadoBeams();
}

void L_WarriorAttackUltimate::on_update(float dt)
{
	if (boss)
	{
		Vec3 toTarget = boss->get_position() - agent->get_position();
		float faceYaw = atan2f(toTarget.x, toTarget.z);
		agent->set_yaw(faceYaw);
	}

	UpdateTornadoBeams(dt);

	if (activeTornadoBeams.empty())
	{
		agent->DealDamageToBoss(25.f, agent);
		on_success();
	}
	display_leaf_text();
}

void L_WarriorAttackUltimate::UpdateTornadoBeams(float dt)
{
    for (int i = static_cast<int>(activeTornadoBeams.size()) - 1; i >= 0; --i)
    {
        TornadoBeamEffect& effect = activeTornadoBeams[i];
        if (!effect.agent) continue;

        effect.lifeTime += dt;
        Vec3 pos = effect.agent->get_position();
        effect.agent->set_position(pos + effect.velocity * dt);

        //The rotation
        float currRoll = effect.agent->get_roll();
        effect.agent->set_roll(currRoll + dt * 100.f);

        if (effect.lifeTime >= effect.maxDuration)
        {
            agents->destroy_agent(effect.agent);
            activeTornadoBeams.erase(activeTornadoBeams.begin() + i);
        }
    }
}

void L_WarriorAttackUltimate::InitTornadoBeams()
{
    Vec3 origin = agent->get_position();
    Vec3 toTarget = (boss->get_position() - origin);

    float yaw = atan2f(toTarget.x, toTarget.z);
    for (int i = 0; i < 5; ++i)
    {
        float layerOffset = static_cast<float>(i) * 0.2f;
        float scale = 1.5f - i * 0.4f;
        Vec3 spawnPos = origin + toTarget * layerOffset + Vec3(0.f, 2.5f, 0.f);

        BehaviorAgent* beam = agents->create_behavior_agent("TornadoBeam", BehaviorTreeTypes::Idle);
        beam->SetAgentType(AgentType::DEFAULT);
        beam->set_scaling(Vec3(scale, scale * 2.f, scale));
        beam->set_color(Vec3(0.6f, 1.f - i * 0.1f, 1.f));

        beam->set_position(spawnPos);
        beam->set_roll(PI * 0.5f);
        beam->set_yaw(yaw);
        beam->set_pitch(0.f);

        TornadoBeamEffect effect;
        effect.agent = beam;
        effect.velocity = toTarget * 3.f;
        effect.lifeTime = 0.f;
        effect.maxDuration = 1.5f;
        effect.caster = agent;

        activeTornadoBeams.push_back(effect);
    }
    audioManager->PlaySoundEffect(L"Assets\\Audio\\UltimateAttack.wav");

}


