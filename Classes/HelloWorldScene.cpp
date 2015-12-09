#include "HelloWorldScene.h"
#include "GameSettings.h"
#include "MapLayer.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{    
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

HelloWorld::HelloWorld()
:_render(nullptr)
{
    
}

HelloWorld::~HelloWorld()
{
    CC_SAFE_DELETE(_render);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    _render = new GameRender(this, 1, "对手的名字");
    
    _sch = new GameScheduler();
    UnderWorld::Core::GameSettings setting;
    
    //1. set map setting
    UnderWorld::Core::MapSetting mapSettting = _render->getMapLayer()->mapSetting;
    
//    //1a. set locations
//    UnderWorld::Core::LocationSetting locationSetting0;
//    locationSetting0.setCoreLocation(UnderWorld::Core::Coordinate(25, 70));
//    locationSetting0.addBuildingLocation(UnderWorld::Core::Coordinate(100, 70));
//    locationSetting0.addAssembleLocation(0, UnderWorld::Core::Coordinate(65, 70));
//    locationSetting0.addAssembleLocation(1, UnderWorld::Core::Coordinate(60, 70));
//    mapSettting.setLocationSetting(0, locationSetting0);
//    
//    UnderWorld::Core::LocationSetting locationSetting1;
//    locationSetting1.setCoreLocation(UnderWorld::Core::Coordinate(275, 70));
//    locationSetting1.addBuildingLocation(UnderWorld::Core::Coordinate(200, 70));
//    locationSetting1.addAssembleLocation(0, UnderWorld::Core::Coordinate(235, 70));
//    locationSetting1.addAssembleLocation(1, UnderWorld::Core::Coordinate(240, 70));
//    mapSettting.setLocationSetting(1, locationSetting1);
//    
//    //1b. set resource
//    std::vector<UnderWorld::Core::ResourceSetting> rs0;
//    rs0.resize(2);
//    rs0[0].setResourceTypeName("金子");
//    rs0[0].setInitBalance(100);
//    rs0[0].setInitSalary(10);
//    rs0[0].setSalaryAccelerate(1);
//    rs0[1].setResourceTypeName("人口");
//    rs0[1].setInitBalance(200);
//    rs0[1].setInitSalary(0);
//    rs0[1].setSalaryAccelerate(0);
//    mapSettting.setStartResource(0, rs0);
//    
//    std::vector<UnderWorld::Core::ResourceSetting> rs1;
//    rs1.resize(2);
//    rs1[0].setResourceTypeName("金子");
//    rs1[0].setInitBalance(100);
//    rs1[0].setInitSalary(10);
//    rs1[0].setSalaryAccelerate(1);
//    rs1[1].setResourceTypeName("人口");
//    rs1[1].setInitBalance(200);
//    rs1[1].setInitSalary(0);
//    rs1[1].setSalaryAccelerate(0);
//    mapSettting.setStartResource(1, rs1);
//    
//    //1c. set fixed units
//    std::vector<std::pair<UnderWorld::Core::UnitSetting, UnderWorld::Core::Coordinate> > fu0;
//    fu0.resize(3);
//    for (int i = 0; i < 1; ++i) {
//        fu0[i].first.setUnitTypeName("狼人步兵");
//        fu0[i].second = UnderWorld::Core::Coordinate(120, 70);
//    }
//    for (int i = 1; i < 2; ++i) {
//        fu0[i].first.setUnitTypeName("狼人射手");
//        fu0[i].second = UnderWorld::Core::Coordinate(110, 70);
//    }
//    for (int i = 2; i < 3; ++i) {
//        fu0[i].first.setUnitTypeName("狼人巫师");
//        fu0[i].second = UnderWorld::Core::Coordinate(100, 70);
//    }
//    mapSettting.setFixedUnits(0, fu0);
//    
//    std::vector<std::pair<UnderWorld::Core::UnitSetting, UnderWorld::Core::Coordinate> > fu1;
//    fu1.resize(3);
//    for (int i = 0; i < 1; ++i) {
//        fu1[i].first.setUnitTypeName("吸血鬼战士");
//        fu1[i].second = UnderWorld::Core::Coordinate(240, 70);
//    }
//    for (int i = 1; i < 2; ++i) {
//        fu1[i].first.setUnitTypeName("吸血鬼弓兵");
//        fu1[i].second = UnderWorld::Core::Coordinate(245, 70);
//    }
//    for (int i = 2; i < 3; ++i) {
//        fu1[i].first.setUnitTypeName("吸血鬼巫师");
//        fu1[i].second = UnderWorld::Core::Coordinate(250, 70);
//    }

//    mapSettting.setFixedUnits(1, fu1);
    setting.setMap(mapSettting);
    
    //2. set techTree;
    std::string techTree = "<root> <faction name=\"狼人族\"> <unit name=\"狼人基地\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"120\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"狼人箭塔\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"64\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"狼人步兵\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"income_damage_reduce\" value=\"p:0.2\"/> </unit> <unit name=\"狼人射手\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"20\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"320\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"rebound\" value=\"p:0.5\" range=\"100\" count=\"3\"/> </unit> <unit name=\"狼人巫师\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"4\" maxdamage=\"5\" range=\"160\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> </faction> <faction name=\"吸血鬼族\"> <unit name=\"吸血鬼核心\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"120\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼箭塔\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"64\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"400\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼战士\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"32\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"8\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"drain\" value=\"p:0.2\"/> </unit> <unit name=\"吸血鬼弓兵\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"400\" size=\"20\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"300\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> </unit> <unit name=\"吸血鬼巫师\" class=\"warrior\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"300\" size=\"20\" cost=\"金子:200;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"1\" maxdamage=\"20\" range=\"300\" fields=\"3\"> <bullet speed=\"200\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/> <feature type=\"splash\" value=\"p:0.4\" range=\"100\"/> </unit> </faction> <resource name=\"金子\" class=\"consumable\"/> <resource name=\"人口\" class=\"holdable\"/> </root>";

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
    cs0.resize(3);
    UnderWorld::Core::UnitSetting us00;
    us00.setUnitTypeName("狼人步兵");
    cs0[0].setUnitSetting(us00);
    cs0[0].setMaxProduction(100);
    UnderWorld::Core::UnitSetting us01;
    us01.setUnitTypeName("狼人射手");
    cs0[1].setUnitSetting(us01);
    cs0[1].setMaxProduction(100);
    UnderWorld::Core::UnitSetting us02;
    us02.setUnitTypeName("狼人巫师");
    cs0[2].setUnitSetting(us02);
    cs0[2].setMaxProduction(100);
    setting.setCamps(0, cs0);
    
    vector<UnderWorld::Core::CampSetting> cs1;
    cs1.resize(3);
    UnderWorld::Core::UnitSetting us10;
    us10.setUnitTypeName("吸血鬼战士");
    cs1[0].setUnitSetting(us10);
    cs1[0].setMaxProduction(100);
    UnderWorld::Core::UnitSetting us11;
    us11.setUnitTypeName("吸血鬼弓兵");
    cs1[1].setUnitSetting(us11);
    cs1[1].setMaxProduction(100);
    UnderWorld::Core::UnitSetting us12;
    us12.setUnitTypeName("吸血鬼巫师");
    cs1[2].setUnitSetting(us12);
    cs1[2].setMaxProduction(100);
    setting.setCamps(1, cs1);

    _looper = new UnderWorld::Core::GameLooper(_render, _sch);
    _looper->init(setting);
    _looper->start();
    
    
//    auto listener = EventListenerTouchAllAtOnce::create();
//    listener->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
//    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
//
    
    return true;
}

void HelloWorld::onTouchesMoved(const std::vector<Touch*>& touches, Event  *event) {
    auto touch = touches[0];
    
    auto diff = touch->getDelta();
    auto node = getChildByTag(1);
    auto currentPos = node->getPosition();
    node->setPosition(currentPos + diff);
}