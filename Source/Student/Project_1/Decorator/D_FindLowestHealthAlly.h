#pragma once
#include "BehaviorNode.h"

class D_FindLowestHealthAlly : public BaseNode<D_FindLowestHealthAlly>
{

protected:
	void on_enter() override;
	void on_update(float dt) override;
};