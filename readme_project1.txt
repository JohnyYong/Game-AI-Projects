Student Name: Johny Yong Jun Siang

Project Name: Grand Battle

What I implemented: A simulation of a party consisting of 3 members (Warrior, Caster, Tank) battling a Boss

I also added:
- A health variable for the behavior agents as well, with the getters and setters.
- An AgentType Enum class to differentiate the different agents

Directions (if needed):

What I liked about the project and framework:
The nodes are really simple to create once I understood how the files work. 

What I disliked about the project and framework:
The behavior tree editor messing up the structure when I drag the nodes in sometimes.

Any difficulties I experienced while doing the project:
Moving the new nodes within the behavior tree editor sometimes messed up the entire structure which wasted me some time.
Trying to imagine the pitch, fork and roll based on the camera angle is hard.
Trying to animate the models using logic is more difficult than implementing the behavior trees

Hours spent: 11

New selector node (name):
- NIL

New decorator nodes (names):
- D_BossCheckStunned.h/.cpp
- D_CheckBossExist.h/.cpp
- D_CheckSelfLowHP.h/.cpp
- D_FindLowestHealthAlly.h/.cpp
- D_CheckBossInRange.h/.cpp

Total new decorator nodes: 5

10 total nodes (names):
- L_BossAttackLaser.h/.cpp
- L_BossAttackMelee.h/.cpp
- L_BossFindTarget.h/.cpp
- L_BossStunnedIdle.h/.cpp
- L_MoveToBoss.h/.cpp
- L_WarriorAttackNormal.h/.cpp
- L_WarriorAttackSpecial.h/.cpp
- L_WarriorAttackUltimate.h/.cpp
- L_HealLowestHPAlly.h/.cpp
- L_RunBehindTank.h/.cpp
- L_MoveToDefendAlly.h/.cpp
- L_TankStunBoss.h/.cpp
- L_TankDefend.h/.cpp

Total new leaf nodes: 13

4 Behavior trees (names):
Warrior.bht
Caster.bht
Tank.bht
Boss.bht

Extra credit: