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
#include "SoundManager.h"

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

void BattleScene::onExit()
{
    SoundManager::getInstance()->stopBackgroundMusic();
    Scene::onExit();
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
    string mapSettingXml = "<root> <location_setting index=\"0\" core_location=\"400,600\" building_location=\"\" assemble_location=\"0:600,600;2:800,600\"/> <resource_settings index=\"0\"> <resource_setting resource_name=\"金子\" init_balance=\"300\" init_salary=\"5\" salary_accelerate=\"0\"/> <resource_setting resource_name=\"人口\" init_balance=\"200\" init_salary=\"0\" salary_accelerate=\"0\"/> </resource_settings> <fixed_unit_setting index=\"0\"> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> </fixed_unit_setting> <location_setting index=\"1\" core_location=\"2000,600\" building_location=\"\" assemble_location=\"0:1600,600;2:1200,600\"/> <resource_settings index=\"1\"> <resource_setting resource_name=\"金子\" init_balance=\"300\" init_salary=\"5\" salary_accelerate=\"0\"/> <resource_setting resource_name=\"人口\" init_balance=\"200\" init_salary=\"0\" salary_accelerate=\"0\"/> </resource_settings> <fixed_unit_setting index=\"1\"> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> </fixed_unit_setting> </root>";
    
    _mapLayer = MapLayer::create(mapId, mapSettingXml);
    addChild(_mapLayer);
    
    // 2. add map ui layer
    _render = new GameRender(this, mapId, _mapLayer, "对手的名字");
    _render->registerObserver(this);
    
    _sch = new GameScheduler();
    
    UnderWorld::Core::GameSettings setting;
    
    //1. set map setting
    setting.setMap(_mapLayer->getMapSetting());
    
    //2. set techTree;
    std::string techTree = "<root> <faction name=\"狼人族\"> <unit name=\"狼人基地\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"120\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"狼人箭塔\" class=\"building\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"64\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"狼人步兵\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"income_damage_reduce\" value=\"p:0.2\"/> </unit> <unit name=\"狼人射手\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"20\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"320\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"rebound\" value=\"p:0.5\" range=\"100\" count=\"3\"/> </unit> <unit name=\"吸血鬼蜘蛛\" class=\"warrior\" hp=\"165\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"40\" cost=\"金子:235;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"1.5\" mindamage=\"14\" maxdamage=\"18\" range=\"266\" fields=\"3\"> <bullet speed=\"400\" size=\"20\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/> <feature type=\"rebound\" value=\"p:0.5\" range=\"100\" count=\"3\"/> </unit> <unit name=\"狼人巫师\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> <unit name=\"狼人独角兽\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"income_damage_reduce\" value=\"p:0.2\"/> </unit> <unit name=\"狼人森林之魂\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> <unit name=\"七恶魔拉玛什图\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"income_damage_reduce\" value=\"p:0.2\"/> </unit> <unit name=\"莉莉丝\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> <unit name=\"时光法师\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> <unit name=\"时光女神\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> <unit name=\"特斯拉\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"income_damage_reduce\" value=\"p:0.2\"/> </unit> <unit name=\"天空女神\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> </faction> <faction name=\"吸血鬼族\"> <unit name=\"吸血鬼核心\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"120\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼箭塔\" class=\"building\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"64\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼战士\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"drain\" value=\"p:0.2\"/> </unit> <unit name=\"吸血鬼弓兵\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"20\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"300\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> </unit> <unit name=\"吸血鬼巫师\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"1\" maxdamage=\"20\" range=\"300\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> </faction> <resource name=\"金子\" class=\"consumable\"/> <resource name=\"人口\" class=\"holdable\"/> </root>";
    
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
    
    // 5. set camps
    // 5.1 werewolf
    {
        vector<UnderWorld::Core::CampSetting> cs;
        static const unsigned int camp_count = 12;
        static const string camp_names[camp_count] = {
            "狼人步兵",
            "狼人射手",
            "狼人巫师",
            "吸血鬼蜘蛛",
            "狼人独角兽",
            "狼人森林之魂",
            "七恶魔拉玛什图",
            "莉莉丝",
            "时光法师",
            "时光女神",
            "特斯拉",
            "天空女神",
        };
        cs.resize(camp_count);
        
        for (int i = 0; i < camp_count; ++i) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName(camp_names[i]);
            cs[i].setUnitSetting(us);
            cs[i].setMaxProduction(5);
        }
        setting.setCamps(0, cs);
    }
    // 5.2 vampire
    {
        vector<UnderWorld::Core::CampSetting> cs;
        static const unsigned int camp_count = 3;
        static const string camp_names[camp_count] = {
            "吸血鬼战士",
            "吸血鬼弓兵",
            "吸血鬼巫师",
        };
        cs.resize(camp_count);
        
        for (int i = 0; i < camp_count; ++i) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName(camp_names[i]);
            cs[i].setUnitSetting(us);
            cs[i].setMaxProduction(5);
        }
        setting.setCamps(1, cs);
    }
    
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