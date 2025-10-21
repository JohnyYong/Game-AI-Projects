#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_TankDefend.h"
#include "TreeInfo.h"


void L_TankDefend::on_enter()
{
	on_leaf_enter();
    activeDefendOrbs.clear();
    InitDefendOrbs();
    defendStarted = true;
}

void L_TankDefend::on_update(float dt)
{
	BehaviorAgent* boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");
	if (boss)
	{
		Vec3 toTarget = boss->get_position() - agent->get_position();
		float faceYaw = atan2f(toTarget.x, toTarget.z);
		agent->set_yaw(faceYaw);
	}

    if (!defendStarted) return;
    UpdateDefendOrbs(dt);
    if (activeDefendOrbs.empty())
    {
        on_success();
        defendStarted = false;
    }

	display_leaf_text();
}

void L_TankDefend::UpdateDefendOrbs(float dt)
{
    for (int i = static_cast<int>(activeDefendOrbs.size()) - 1; i >= 0; --i)
    {
        auto& orb = activeDefendOrbs[i];
        orb.elapsedTime += dt;

        if (orb.elapsedTime >= orb.duration)
        {
            agents->destroy_agent(orb.agent);
            activeDefendOrbs.erase(activeDefendOrbs.begin() + i);
            continue;
        }

        float t = orb.elapsedTime;
        float angle = orb.angleOffset + t * 2.5f; //Spin speed

        Vec3 offset = Vec3(
            cosf(angle) * orb.facingDir.x + sinf(angle) * 0.f,
            sinf(angle) * 1.0f, //Verticle Spin
            cosf(angle) * orb.facingDir.z
        ) * orb.radius;

        orb.agent->set_position(orb.center + offset);
    }
}

void L_TankDefend::InitDefendOrbs()
{
    const int orbCount = 6;
    float yaw = agent->get_yaw(); 
    Vec3 facing = Vec3(sinf(yaw), 0.f, cosf(yaw));
    Vec3 up = Vec3(0.f, 1.f, 0.f);
    Vec3 right = up.Cross(facing);
    Vec3 center = agent->get_position() + facing * 2.f + Vec3(0.f, 12.0f, 0.f); 

    for (int i = 0; i < orbCount; ++i)
    {
        auto orb = agents->create_behavior_agent("DefendOrb", BehaviorTreeTypes::Idle, Agent::AgentModel::Ball);
        orb->SetAgentType(AgentType::DEFAULT);
        orb->set_scaling(0.2f);
        orb->set_color(Vec3(0.3f, 0.8f, 1.f));

        orb->set_position(center);

        DefendOrbEffect effect;
        effect.agent = orb;
        effect.caster = agent;
        effect.elapsedTime = 0.f;
        effect.angleOffset = (2 * PI / orbCount) * i;
        effect.radius = 5.f;
        effect.duration = 3.0f;
        effect.center = center;
        effect.facingDir = right;

        activeDefendOrbs.push_back(effect);
    }
    audioManager->PlaySoundEffect(L"Assets\\Audio\\Defend.wav");
}
