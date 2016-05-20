//
//  WorldTest.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/5/20.
//
//

#include <iostream>

#include "WorldTest.h"
#include "UnderworldClient.h"
#include "Faction.h"
#include "Unit.h"


void WorldTest::test() {
    
    UnderWorld::Core::GameContentSetting contentSetting;
    
    contentSetting.setFactionTypeKey("狼人族");
    
    UnderWorld::Core::UnitSetting core;
    core.setUnitTypeName("狼人基地");
    contentSetting.setCore(core);
    
    UnderWorld::Core::UnitSetting tower;
    tower.setUnitTypeName("狼人箭塔");
    contentSetting.setTower(tower);

    UnderWorld::Core::GameSettings gs;
    std::string commonTechTree;
    UnderworldClient::loadCommonTechTree(commonTechTree);
    gs.setTechTree(commonTechTree);
    
    gs.setMapId(1);
    UnderWorld::Core::MapSetting ms;
    UnderworldClient::loadCommonMapSetting(1, ms);
    gs.setMapSetting(ms);
    gs.setFactionSetting(ms.getFactionSetting());
    
    int mapIndex = gs.getFactionSetting().getMapIndex(gs.getFactionSetting().getThisFactionIndex());
    gs.getFactionSetting().setContentSetting(contentSetting, mapIndex);
    
    UnderWorld::Core::RandomCreator random;
    UnderWorld::Core::World world;
    
    world.init(gs, &random);
    world.addWorldObserver(this);
    
    int count = 0;
    while (true) {
        ++count;
        world.update(count);
        
        if (count % 71 == 0) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName("疾风弓手_作战");
            UnderWorld::Core::creatureid_t id =  world.createUnit2World(us, random.rand_i() % 2, UnderWorld::Core::Coordinate32(random.rand_i() % 1500, random.rand_i() % 1500));
            if (id != UnderWorld::Core::World::INVALID_ID) {
                std::cout << "create unit <======== " << id << std::endl;
            }
        }
        
        if (count % 73 == 0) {
            int fid = random.rand_i() % 2;
            
            if (world.getFaction(fid)->getUnitCount() > 0) {
                auto unit = world.getFaction(fid)->getUnitByIndex(0);
                std::cout << "eliminate unit <======== " << unit->getUnitId() << std::endl;
                unit->eliminate();
            }
        }
        
        if (count % UnderWorld::Core::GameConstants::FRAME_PER_SEC) {
            world.tick(count / UnderWorld::Core::GameConstants::FRAME_PER_SEC);
        }
        
        
    }
}

void WorldTest::onNotifyWorldEvents(const std::vector<UnderWorld::Core::World::EventLog>& events) {
    for (int i = 0; i < events.size(); ++i) {
        auto& event = events[i];
        if (event._type == UnderWorld::Core::World::EventLogType::UnitCreate) {
            std::cout << "notify create unit <======== " << event._id << std::endl;
        } else if (event._type == UnderWorld::Core::World::EventLogType::UnitEliminate) {
            std::cout << "notify eliminate unit <======== " << event._id << std::endl;
        }
    }
}