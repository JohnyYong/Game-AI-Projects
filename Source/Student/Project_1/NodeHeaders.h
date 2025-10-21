#pragma once

// Include all node headers in this file

// Example Control Flow Nodes
#include "ControlFlow/C_ParallelSequencer.h"
#include "ControlFlow/C_RandomSelector.h"
#include "ControlFlow/C_Selector.h"
#include "ControlFlow/C_Sequencer.h"

// Student Control Flow Nodes


// Example Decorator Nodes
#include "Decorator/D_Delay.h"
#include "Decorator/D_InvertedRepeater.h"
#include "Decorator/D_RepeatFourTimes.h"

// Student Decorator Nodes
#include "Decorator/D_FindLowestHealthAlly.h"
#include "Decorator/D_CheckBossExist.h"
#include "Decorator/D_CheckSelfLowHP.h"
#include "Decorator/D_BossCheckStunned.h"
#include "Decorator/D_CheckBossInRange.h"

// Example Leaf Nodes
#include "Leaf/L_CheckMouseClick.h"
#include "Leaf/L_Idle.h"
#include "Leaf/L_MoveToFurthestAgent.h"
#include "Leaf/L_MoveToMouseClick.h"
#include "Leaf/L_MoveToRandomPosition.h"
#include "Leaf/L_PlaySound.h"

// Student Leaf Nodes

//For warrior
#include "Leaf/L_MoveToBoss.h"
#include "Leaf/L_WarriorAttackNormal.h"
#include "Leaf/L_WarriorAttackSpecial.h"
#include "Leaf/L_WarriorAttackUltimate.h"

//For Caster
#include "Leaf/L_HealLowestHPAlly.h"
#include "Leaf/L_RunBehindTank.h"

//For tank
#include "Leaf/L_MoveToDefendAlly.h"
#include "Leaf/L_TankStunBoss.h"
#include "Leaf/L_TankDefend.h"

//For Boss
#include "Leaf/L_BossAttackLaser.h"
#include "Leaf/L_BossAttackMelee.h"
#include "Leaf/L_BossFindTarget.h"
#include "Leaf/L_BossMoveToAgent.h"
#include "Leaf/L_BossStunnedIdle.h"
