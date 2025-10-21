#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_BossAttackLaser.h"
#include "TreeInfo.h"

void L_BossAttackLaser::on_enter()
{
	on_leaf_enter();
    attackTarget = agent->get_blackboard().get_value<BehaviorAgent*>("attackTarget");
    BossLasersInit(); //Initialise the laser
}

void L_BossAttackLaser::on_update(float dt)
{
	if (!attackTarget)
	{
		on_failure();
		display_leaf_text();
		return;
	}

	//agent->PlayAnimation("BossAttackLaser", attackTarget);
    UpdateBossLasers(dt);
	if (attackTarget)
	{
		Vec3 toTarget = attackTarget->get_position() - agent->get_position();
		float faceYaw = atan2f(toTarget.x, toTarget.z);
		agent->set_yaw(faceYaw);
	}

    if (activeBossLasers.empty())
    {
        attackTarget->ReceiveDamage(15.f);
        on_success();
        display_leaf_text();

    }
}


void L_BossAttackLaser::UpdateBossLasers(float dt)
{
    float laserVelocity = 10.f;

    for (int i = static_cast<int>(activeBossLasers.size()) - 1; i >= 0; --i)
    {
        auto& effect = activeBossLasers[i];
        effect.elapsedTime += dt;

        //OrbScaling
        Vec3 startScale(0.5f, 0.5f, 0.5f);
        Vec3 finalScale(1.2f, 1.2f, 1.2f);

        if (effect.orb && !effect.laserFired)
        {
            float t = std::min(effect.elapsedTime / effect.chargeTime, 1.0f);
            Vec3 newScale = startScale * (1.0f - t) + finalScale * t;
            effect.orb->set_scaling(newScale);
        }

        //Charging phase
        if (!effect.laserFired && effect.elapsedTime >= effect.chargeTime)
        {
            //Fire laser
            BehaviorAgent* laser = agents->create_behavior_agent("LaserBeam", BehaviorTreeTypes::Idle, Agent::AgentModel::Man);
            laser->SetAgentType(AgentType::DEFAULT);
            laser->set_scaling(Vec3(0.4f, 0.4f, 15.f));
            laser->set_color(Vec3(1.f, 0.3f, 0.3f));

            Vec3 beamPos = effect.origin;
            laser->set_position(beamPos);
            laser->set_yaw(atan2f(effect.direction.x, effect.direction.z));

            effect.laser = laser;
            effect.laserFired = true;
            audioManager->PlaySoundEffect(L"Assets\\Audio\\BossLaser.wav");
        }

        if (effect.laserFired && effect.laser)
        {
            Vec3 currentPos = effect.laser->get_position();
            Vec3 newPos = currentPos + effect.direction * laserVelocity * dt;
            effect.laser->set_position(newPos);
        }

        if (effect.elapsedTime >= effect.totalTime)
        {
            if (effect.orb) agents->destroy_agent(effect.orb);
            if (effect.laser) agents->destroy_agent(effect.laser);
            activeBossLasers.erase(activeBossLasers.begin() + i);
        }
    }
}

void L_BossAttackLaser::BossLasersInit()
{
    Vec3 bossPos = agent->get_position();
    Vec3 dir = attackTarget ? (attackTarget->get_position() - bossPos) : Vec3(0.f, 0.f, 1.f); // fallback Z+
    float yaw = atan2f(dir.x, dir.z);

    Vec3 offset = dir * 0.2f + Vec3(0.f, 12.f, 0.f); // in front of chest
    Vec3 orbPos = bossPos + offset;

    // Spawn charging orb using boss mesh
    BehaviorAgent* orb = agents->create_behavior_agent("LaserOrb", BehaviorTreeTypes::Idle, Agent::AgentModel::Ball);
    orb->SetAgentType(AgentType::DEFAULT);
    orb->set_scaling(Vec3(0.5f));
    orb->set_color(Vec3(1.f, 0.f, 0.f));
    orb->set_position(orbPos);
    orb->set_yaw(yaw);

    BossLaserEffect effect;
    effect.orb = orb;
    effect.laser = nullptr;
    effect.origin = orbPos;
    effect.direction = dir;
    effect.elapsedTime = 0.f;

    activeBossLasers.push_back(effect);
}
