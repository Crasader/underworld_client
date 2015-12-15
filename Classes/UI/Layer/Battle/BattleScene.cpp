//
//  BattleScene.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleScene.h"
#include "GameSettings.h"
#include "MapLayer.h"

USING_NS_CC;

BattleScene* BattleScene::create(int mapId)
{
    BattleScene *ret = new (std::nothrow) BattleScene();
    if (ret && ret->init(mapId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleScene::BattleScene()
:_mapId(0)
,_mapLayer(nullptr)
,_render(nullptr)
,_looper(nullptr)
,_sch(nullptr)
{
    
}

BattleScene::~BattleScene()
{
    clear();
}

// on "init" you need to initialize your instance
bool BattleScene::init(int mapId)
{
    if (Scene::init() )
    {
        _mapId = mapId;
        return true;
    }
    
    return false;
}

void BattleScene::onEnter()
{
    Scene::onEnter();
    start(_mapId);
}

#pragma mark - GameRenderObserver
void BattleScene::onGameRenderRestart()
{
    start(_mapId);
}

void BattleScene::start(int mapId)
{
    if (_mapLayer) {
        clear();
    }
    
    // 1. add map layer
    _mapLayer = MapLayer::create(mapId);
    addChild(_mapLayer);
    
    // 2. add map ui layer
    _render = new GameRender(this, mapId, _mapLayer, "对手的名字");
    _render->registerObserver(this);
    
    _sch = new GameScheduler();
    UnderWorld::Core::GameSettings setting;
    
    //1. set map setting
    UnderWorld::Core::MapSetting mapSettting = _mapLayer->mapSetting;
    setting.setMap(mapSettting);
    
    //2. set techTree;
    std::string techTree = "<root> <faction name=\"狼人族\"> <unit name=\"狼人基地\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"120\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"狼人箭塔\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"64\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"狼人步兵\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"income_damage_reduce\" value=\"p:0.2\"/> </unit> <unit name=\"狼人射手\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"20\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"320\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"rebound\" value=\"p:0.5\" range=\"100\" count=\"3\"/> </unit> <unit name=\"吸血鬼蜘蛛\" class=\"warrior\" hp=\"165\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"40\" cost=\"金子:235;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"1.5\" mindamage=\"14\" maxdamage=\"18\" range=\"266\" fields=\"3\"> <bullet speed=\"400\" size=\"20\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/> <feature type=\"rebound\" value=\"p:0.5\" range=\"100\" count=\"3\"/> </unit> <unit name=\"狼人巫师\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> </faction> <faction name=\"吸血鬼族\"> <unit name=\"吸血鬼核心\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"120\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼箭塔\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"64\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼战士\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"drain\" value=\"p:0.2\"/> </unit> <unit name=\"吸血鬼弓兵\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"20\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"300\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> </unit> <unit name=\"吸血鬼巫师\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"1\" maxdamage=\"20\" range=\"300\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> </faction> <resource name=\"金子\" class=\"consumable\"/> <resource name=\"人口\" class=\"holdable\"/> </root>";
    
    setting.setTechTree(techTree);
    
    //3. set faction data
    setting.setFactionCount(2);
    setting.setThisFactionIndex(0);
    setting.setFactionTypeKey(0, "狼人族");
    setting.setFactionControlType(0, UnderWorld::Core::kFactionControlType_Human);
    setting.setTeam(0, 0);
    setting.setMapIndex(0, 0);
    setting.setFactionTypeKey(1, "吸血鬼族");
    setting.setFactionControlType(1, UnderWorld::Core::kFactionControlType_Cpu);
    setting.setTeam(1, 1);
    setting.setMapIndex(1, 1);
    
    //4. set core & tower
    UnderWorld::Core::UnitSetting core0;
    core0.setUnitTypeName("狼人基地");
    setting.setCore(0, core0);
    UnderWorld::Core::UnitSetting core1;
    core1.setUnitTypeName("吸血鬼核心");
    setting.setCore(1, core1);
    UnderWorld::Core::UnitSetting tower0;
    tower0.setUnitTypeName("狼人箭塔");
    setting.setTower(0, tower0);
    UnderWorld::Core::UnitSetting tower1;
    tower1.setUnitTypeName("吸血鬼箭塔");
    setting.setTower(1, tower1);
    
    //5. set camps
    vector<UnderWorld::Core::CampSetting> cs0;
    cs0.resize(4);
    UnderWorld::Core::UnitSetting us00;
    us00.setUnitTypeName("狼人步兵");
    cs0[0].setUnitSetting(us00);
    cs0[0].setMaxProduction(5);
    UnderWorld::Core::UnitSetting us01;
    us01.setUnitTypeName("狼人射手");
    cs0[1].setUnitSetting(us01);
    cs0[1].setMaxProduction(5);
    UnderWorld::Core::UnitSetting us02;
    us02.setUnitTypeName("狼人巫师");
    cs0[2].setUnitSetting(us02);
    cs0[2].setMaxProduction(5);
    UnderWorld::Core::UnitSetting us03;
    us03.setUnitTypeName("吸血鬼蜘蛛");
    cs0[3].setUnitSetting(us03);
    cs0[3].setMaxProduction(5);
    setting.setCamps(0, cs0);
    
    vector<UnderWorld::Core::CampSetting> cs1;
    cs1.resize(3);
    UnderWorld::Core::UnitSetting us10;
    us10.setUnitTypeName("吸血鬼战士");
    cs1[0].setUnitSetting(us10);
    cs1[0].setMaxProduction(5);
    UnderWorld::Core::UnitSetting us11;
    us11.setUnitTypeName("吸血鬼弓兵");
    cs1[1].setUnitSetting(us11);
    cs1[1].setMaxProduction(5);
    UnderWorld::Core::UnitSetting us12;
    us12.setUnitTypeName("吸血鬼巫师");
    cs1[2].setUnitSetting(us12);
    cs1[2].setMaxProduction(5);
    setting.setCamps(1, cs1);
    
    _looper = new UnderWorld::Core::GameLooper(_render, _sch);
    _looper->init(setting);
    _looper->start();
}

void BattleScene::clear()
{
    if (_looper) {
        _looper->end();
    }
    CC_SAFE_DELETE(_looper);
    CC_SAFE_DELETE(_sch);
    CC_SAFE_DELETE(_render);
    removeAllChildren();
    _mapLayer = nullptr;
}