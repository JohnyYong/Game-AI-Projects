#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_TankStunBoss.h"
#include "TreeInfo.h"


void L_TankStunBoss::on_enter()
{
	on_leaf_enter();
    boss = agent->get_blackboard().get_value<BehaviorAgent*>("boss");
    InitStunDrops();
}

void L_TankStunBoss::on_update(float dt)
{
	if (boss)
	{
		Vec3 toTarget = boss->get_position() - agent->get_position();
		float faceYaw = atan2f(toTarget.x, toTarget.z);
		agent->set_yaw(faceYaw);
	}

    UpdateStunDrops(dt);
    if (activeStunDrops.empty())
    {
        boss->set_color(Vec3(1.f, 1.f, 0.f)); //Forcefully make the boss yellow color the moment this attack is successful
        boss->get_blackboard().set_value("IsStunned", true);
        on_success();

    }
	display_leaf_text();
}

void L_TankStunBoss::UpdateStunDrops(float dt)
{
    for (int i = static_cast<int>(activeStunDrops.size()) - 1; i >= 0; --i)
    {
        auto& drop = activeStunDrops[i];
        drop.elapsedTime += dt;

        float t = std::min(drop.elapsedTime / drop.dropDuration, 1.f);
        Vec3 start = drop.targetPos + Vec3(0.f, 15.f, 0.f);
        Vec3 pos = Vec3::Lerp(start, drop.targetPos, t);

        drop.agent->set_position(pos);

        if (t >= 1.f)
        {
            agents->destroy_agent(drop.agent);
            activeStunDrops.erase(activeStunDrops.begin() + i);
        }
    }
}

void L_TankStunBoss::InitStunDrops()
{
    const int innerCount = 6;
    const int outerCount = 10;

    Vec3 bossPos = boss->get_position();
    float height = 15.0f;

    //Inner ring
    for (int i = 0; i < innerCount; ++i)
    {
        float angle = (2 * PI / innerCount) * i;
        Vec3 offset = Vec3(cosf(angle), 0.f, sinf(angle)) * 3.0f;

        Vec3 spawnPos = bossPos + offset + Vec3(0.f, height, 0.f);
        Vec3 landPos = bossPos + offset;

        auto soldier = agents->create_behavior_agent("StunDrop", BehaviorTreeTypes::Idle, Agent::AgentModel::Man);
        soldier->SetAgentType(AgentType::DEFAULT);
        soldier->set_scaling(1.2f);
        soldier->set_color(Vec3(1.f, 0.2f, 0.2f));
        soldier->set_position(spawnPos);

        StunDropEffect effect;
        effect.agent = soldier;
        effect.targetPos = landPos;
        effect.elapsedTime = 0.f;
        effect.dropDuration = 0.6f;

        activeStunDrops.push_back(effect);
    }

    //Outer ring
    for (int i = 0; i < outerCount; ++i)
    {
        float angle = (2 * PI / outerCount) * i;
        Vec3 offset = Vec3(cosf(angle), 0.f, sinf(angle)) * 5.5f;

        Vec3 spawnPos = bossPos + offset + Vec3(0.f, height, 0.f);
        Vec3 landPos = bossPos + offset;

        auto soldier = agents->create_behavior_agent("StunDrop", BehaviorTreeTypes::Idle, Agent::AgentModel::Man);
        soldier->SetAgentType(AgentType::DEFAULT);
        soldier->set_scaling(1.2f);
        soldier->set_color(Vec3(1.f, 0.2f, 0.2f));
        soldier->set_position(spawnPos);

        StunDropEffect effect;
        effect.agent = soldier;
        effect.targetPos = landPos;
        effect.elapsedTime = 0.f;
        effect.dropDuration = 0.7f;

        activeStunDrops.push_back(effect);
    }

    audioManager->PlaySoundEffect(L"Assets\\Audio\\TankStun.wav");
}
