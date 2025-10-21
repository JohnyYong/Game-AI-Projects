#include "pch.h"
#include "Agent/BehaviorAgent.h"
#include "L_BossStunnedIdle.h"

void L_BossStunnedIdle::on_enter()
{
	on_leaf_enter();
	stunnedDuration = RNG::range(1.f, 3.f);
}

void L_BossStunnedIdle::on_update(float dt)
{
	stunnedDuration -= dt;
	//agent->PlayAnimation("StunAnimation", agent);
	if (stunnedDuration <= 0.f)
	{
		on_success();
		agent->get_blackboard().set_value("IsStunned", false); //Stop the stun
		return;
	}
	
	display_leaf_text();
}
