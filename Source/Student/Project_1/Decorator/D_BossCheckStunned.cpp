#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "D_BossCheckStunned.h"

void D_BossCheckStunned::on_enter()
{
	BehaviorNode::on_enter();
	if (agent->get_health() <= 10.f)
	{
		agent->set_health(agent->get_maxHealth()); //Due to time constraint decided to just recover boss to full HP when dying
	}
}

void D_BossCheckStunned::on_update(float dt)
{
	bool isStunned = agent->get_blackboard().get_value<bool>("IsStunned");

	Vec3 bossColor(0.3f, 0.f, 0.f); //Dark reddish initially
	;
	if (isStunned)
	{
		BehaviorNode* child = children.front();
		child->tick(dt);

		if (child->succeeded()) { on_success(); } //Set to yellow
		else if (child->failed()) { on_failure();	agent->set_color(bossColor); } //Return to normal color
	}
	else
	{
		agent->set_color(bossColor); //Return to normal color
		on_failure();
	}
	display_leaf_text();
}
