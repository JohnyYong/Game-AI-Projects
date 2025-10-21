#pragma once
#include "BehaviorNode.h"

class L_RunBehindTank : public BaseNode<L_RunBehindTank>
{
protected:
	virtual void on_enter() override;
	virtual void on_update(float dt) override;

private:
	Vec3 targetPos;
};