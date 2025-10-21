#include <pch.h>
#include "Projects/ProjectOne.h"
#include "Agent/CameraAgent.h"
void ProjectOne::setup()
{

    srand(static_cast<unsigned int>(time(0))); //RNG prep

    //Maybe make classes for the entities, easier to manage rather than coding it here
    // Create an agent (using the default "Agent::AgentModel::Man" model)

    //All default agent begin with 100 health (Maybe we just make the damage dealt by the AI be normal?)

    //The default boss
    //"TiamatAgent"


    /*
        Enemy is static at first, however would shoot long range attacks at a certain distance
        At a certain HEALTH (Need to implement the health system into the 
    */

    //Initial spawning and position 

    //Initialization of agents
    auto warrior = agents->create_behavior_agent("Warrior", BehaviorTreeTypes::Warrior);
    auto caster = agents->create_behavior_agent("Caster", BehaviorTreeTypes::Caster);
    auto tank = agents->create_behavior_agent("Tank", BehaviorTreeTypes::Tank);
    auto bossObject = agents->create_behavior_agent("Boss", BehaviorTreeTypes::Boss); //For Tiamat

#pragma region Boss
    Vec3 bossPosition(50.f, 0.f, 50.f);
    bossObject->set_position(bossPosition); //Tiamat should be at center
    bossObject->set_scaling(3.f); //Slightly bigger at the beginning
    Vec3 bossColor(0.3f, 0.f, 0.f); //Dark reddish initially/
    bossObject->SetAgentType(AgentType::BOSS);
    bossObject->set_color(bossColor);
    bossObject->set_movement_speed(25.f);
    bossObject->set_maxHealth(2000.f);
    bossObject->set_health(bossObject->get_maxHealth());
    bossObject->set_yaw(-PI * 0.5f); //Make the object face the the screen

    std::vector<BehaviorAgent*> heroParty = { warrior, caster, tank };
    bossObject->get_blackboard().set_value("heroParty", heroParty);
    bossObject->get_blackboard().set_value("IsStunned", false);
    bossObject->get_blackboard().set_value("lastAttacker", tank);


#pragma endregion

#pragma region Warrior
    Vec3 warriorStartPos(10.f, 0.f, 10.f);
    warrior->set_position(warriorStartPos);
    warrior->set_color(Vec3(0.f, 0.f, 1.f));
    warrior->set_scaling(1.5f);
    warrior->set_movement_speed(25.f);
    warrior->SetAgentType(AgentType::WARRIOR);
    
    //Blackboard val (Warrior)
    warrior->get_blackboard().set_value("boss", bossObject);
    warrior->get_blackboard().set_value("boss_threshold", 20.f);
#pragma endregion
    
#pragma region Tank
    tank->set_position(Vec3(70.f, 0.f, 20.f));
    tank->set_scaling(2.f);
    tank->set_color(Vec3(0.f, 1.f, 0.f));
    tank->set_movement_speed(15.f);
    tank->SetAgentType(AgentType::TANK);
    tank->get_blackboard().set_value("LowHPThreshold", 30.f);
    std::vector<BehaviorAgent*> tankAllies = { warrior, caster };
    tank->get_blackboard().set_value("allies", tankAllies);
    tank->get_blackboard().set_value("boss", bossObject);

#pragma endregion

#pragma region Caster
    caster->set_position(Vec3(20.f, 0.f, 30.f));  
    caster->set_scaling(1.5f);
    caster->set_color(Vec3(1.f, 0.8f, 0.8f));   
    caster->set_movement_speed(30.0f);
    caster->SetAgentType(AgentType::CASTER);
    caster->get_blackboard().set_value("LowHPThreshold", 20.f);
    std::vector<BehaviorAgent*> allies = { warrior, tank };
    caster->get_blackboard().set_value("allies", allies);
    caster->get_blackboard().set_value("tank", tank);
#pragma endregion

    // You can change properties here or at runtime from a behavior tree leaf node
    // Look in Agent.h for all of the setters, like these:
    // man->set_color(Vec3(1, 0, 1));
    // man->set_scaling(Vec3(7,7,7));
    // man->set_position(Vec3(100, 0, 100));

    // Create an agent with a different 3D model:
    // 1. (optional) Add a new 3D model to the framework other than the ones provided:
    //    A. Find a ".sdkmesh" model or use https://github.com/walbourn/contentexporter
    //       to convert fbx files (many end up corrupted in this process, so good luck!)
    //    B. Add a new AgentModel enum for your model in Agent.h (like the existing Man or Tree).
    // 2. Register the new model with the engine, so it associates the file path with the enum
    //    A. Here we are registering all of the extra models that already come in the package.
    Agent::add_model("Assets\\tree.sdkmesh", Agent::AgentModel::Tree);
    Agent::add_model("Assets\\car.sdkmesh", Agent::AgentModel::Car);
    Agent::add_model("Assets\\bird.sdkmesh", Agent::AgentModel::Bird);
    Agent::add_model("Assets\\ball.sdkmesh", Agent::AgentModel::Ball);
    Agent::add_model("Assets\\hut.sdkmesh", Agent::AgentModel::Hut);

#pragma region Terrain
    // You can technically load any map you want, even create your own map file,
    // but behavior agents won't actually avoid walls or anything special, unless you code
    // that yourself (that's the realm of project 2)
    terrain->goto_map(0);

    // You can also enable the pathing layer and set grid square colors as you see fit.
    // Works best with map 0, the completely blank map
    terrain->pathLayer.set_enabled(true);
    terrain->pathLayer.set_value(0, 0, Colors::Red);
#pragma endregion


    // Camera position can be modified from this default
    auto camera = agents->get_camera_agent();
    camera->set_position(Vec3(-62.0f, 70.0f, terrain->mapSizeInWorld * 0.5f));
    camera->set_pitch(0.610865); // 35 degrees

    // Sound control (these sound functions can be kicked off in a behavior tree node - see the example in L_PlaySound.cpp)
    audioManager->SetVolume(0.5f);
    audioManager->PlaySoundEffect(L"Assets\\Audio\\retro.wav");
    // Uncomment for example on playing music in the engine (must be .wav)
     audioManager->PlayMusic(L"Assets\\Audio\\GrandBattleBGM.wav");
    // audioManager->PauseMusic(...);
    // audioManager->ResumeMusic(...);
    // audioManager->StopMusic(...);
}