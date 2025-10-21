#pragma once
#include "BehaviorNode.h"

class L_MoveToBoss : public BaseNode<L_MoveToBoss>
{
protected:
	virtual void on_enter() override;
	virtual void on_update(float dt) override;

private:
	Vec3 targetPos;

};