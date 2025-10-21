/******************************************************************************/
/*!
\file		BehaviorAgent.cpp
\project	CS380/CS580 AI Framework
\author		Dustin Holmes
\summary	Agent that uses behavior trees implementation

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <pch.h>
#include "BehaviorAgent.h"

namespace
{
    const float movementEpsilon = 0.1f;
    const std::wstring debugTextDelimiter(L", ");
    const std::wstring debugNameSeparator(L"_");
}



BehaviorAgent::BehaviorAgent(const char *type, size_t id) : Agent(type, id)
{
    const std::string temp(type);
    debugName = std::wstring(temp.begin(), temp.end()) + debugNameSeparator + std::to_wstring(id);
    set_movement_speed(10.0f);
}

Blackboard &BehaviorAgent::get_blackboard()
{
    return blackboard;
}

BehaviorTree &BehaviorAgent::get_behavior_tree()
{
    return tree;
}

void BehaviorAgent::update(float dt)
{
    tree.update(dt);

}

bool BehaviorAgent::move_toward_point(const Vec3 &point, float dt)
{
    bool result = false;

    const auto currentPos = get_position();

    auto delta = point - currentPos;

    const float length = delta.Length();

    // see if we're close enough to the goal
    if (length <= movementEpsilon)
    {
        // no need to actually move
        result = true;
    }
    else
    {
        // determine how far to actually move
        float actualSpeed = get_movement_speed() * dt;
        
        // see if we even need to move the full distance
        if (length < actualSpeed)
        {
            // this move will put us within range of the target point
            result = true;
            actualSpeed = length;
        }

        delta.Normalize();
        delta *= actualSpeed;

        const auto nextPos = currentPos + delta;
        set_position(nextPos);

        const float yaw = std::atan2(delta.x, delta.z);
        set_yaw(yaw);
    }

    return result;
}

const std::wstring &BehaviorAgent::get_debug_name() const
{
    return debugName;
}

std::wstringstream &BehaviorAgent::get_debug_text()
{
    return debugText;
}

void BehaviorAgent::add_debug_text(const std::wstring &nodeName)
{
    debugText << nodeName << debugTextDelimiter;
}

void BehaviorAgent::SetAgentType(AgentType newType)
{
    agentType = newType;
}

AgentType BehaviorAgent::GetAgentType() const
{
    return agentType;
}

#pragma endregion

//Added by Student (Johny Yong Jun Siang)
void BehaviorAgent::DealDamageToBoss(float amount, BehaviorAgent* attacker)
{
    BehaviorAgent* boss = get_blackboard().get_value<BehaviorAgent*>("boss");

    if (boss) //Safe check
    {
        boss->set_health(boss->get_health() - amount);
        std::cout << "Boss health left: " << boss->get_health() << std::endl;
        boss->get_blackboard().set_value("lastAttacker", attacker);
    }
}

//Added by Student (Johny Yong Jun Siang)
void BehaviorAgent::ReceiveDamage(float amount)
{
    set_health(get_health() - amount);

    if (GetAgentType() == AgentType::WARRIOR)
    {
        std::cout << "WARRIOR health left: ";
    }
    else if (GetAgentType() == AgentType::CASTER)
    {
        std::cout << "CASTER health left: ";
    }
    else if (GetAgentType() == AgentType::TANK)
    {
        std::cout << "TANK health left: ";

    }

    std::cout << get_health() << std::endl;
}

