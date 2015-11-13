#include "HelloWorldScene.h"
#include "GameSettings.h"
#include "MapLayer.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    FileUtils::getInstance()->addSearchPath("res");
    
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
    
    //1a. set locations
    UnderWorld::Core::LocationSetting locationSetting0;
    locationSetting0.setCoreLocation(UnderWorld::Core::Coordinate(100, 40));
    locationSetting0.addBuildingLocation(UnderWorld::Core::Coordinate(200, 40));
    locationSetting0.addAssembleLocation(0, UnderWorld::Core::Coordinate(120, 40));
    locationSetting0.addAssembleLocation(1, UnderWorld::Core::Coordinate(110, 40));
    mapSettting.setLocationSetting(0, locationSetting0);
    
    UnderWorld::Core::LocationSetting locationSetting1;
    locationSetting1.setCoreLocation(UnderWorld::Core::Coordinate(900, 40));
    locationSetting1.addBuildingLocation(UnderWorld::Core::Coordinate(800, 40));
    locationSetting1.addAssembleLocation(0, UnderWorld::Core::Coordinate(880, 40));
    locationSetting1.addAssembleLocation(1, UnderWorld::Core::Coordinate(890, 40));
    mapSettting.setLocationSetting(1, locationSetting1);
    
    //1b. set resource
    std::vector<UnderWorld::Core::ResourceSetting> rs0;
    rs0.resize(2);
    rs0[0].setResourceTypeName("金子");
    rs0[0].setInitBalance(100);
    rs0[0].setInitSalary(10);
    rs0[0].setSalaryAccelerate(1);
    rs0[1].setResourceTypeName("人口");
    rs0[1].setInitBalance(200);
    rs0[1].setInitSalary(0);
    rs0[1].setSalaryAccelerate(0);
    mapSettting.setStartResource(0, rs0);
    
    std::vector<UnderWorld::Core::ResourceSetting> rs1;
    rs1.resize(2);
    rs1[0].setResourceTypeName("金子");
    rs1[0].setInitBalance(100);
    rs1[0].setInitSalary(10);
    rs1[0].setSalaryAccelerate(1);
    rs1[1].setResourceTypeName("人口");
    rs1[1].setInitBalance(200);
    rs1[1].setInitSalary(0);
    rs1[1].setSalaryAccelerate(0);
    mapSettting.setStartResource(1, rs1);
    
    //1c. set fixed units
    std::vector<std::pair<UnderWorld::Core::UnitSetting, UnderWorld::Core::Coordinate> > fu0;
    fu0.resize(20);
    for (int i = 0; i < 10; ++i) {
        fu0[i].first.setUnitTypeName("狼人步兵");
        fu0[i].second = UnderWorld::Core::Coordinate(120, 40);
    }
    for (int i = 10; i < 20; ++i) {
        fu0[i].first.setUnitTypeName("狼人射手");
        fu0[i].second = UnderWorld::Core::Coordinate(110, 40);
    }
    mapSettting.setFixedUnits(0, fu0);
    
    std::vector<std::pair<UnderWorld::Core::UnitSetting, UnderWorld::Core::Coordinate> > fu1;
    fu1.resize(20);
    for (int i = 0; i < 10; ++i) {
        fu1[i].first.setUnitTypeName("吸血鬼战士");
        fu1[i].second = UnderWorld::Core::Coordinate(880, 40);
    }
    for (int i = 10; i < 20; ++i) {
        fu1[i].first.setUnitTypeName("吸血鬼弓兵");
        fu1[i].second = UnderWorld::Core::Coordinate(890, 40);
    }

    mapSettting.setFixedUnits(1, fu1);
    setting.setMap(mapSettting);
    
    //2. set techTree;
    std::string techTree = "<root> <faction name=\"狼人族\"> <unit name=\"狼人基地\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"20\" size=\"16\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\"> <bullet speed=\"50\" size=\"0\"/> </skill> </unit> <unit name=\"狼人箭塔\" class=\"core\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"20\" size=\"16\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\"> <bullet speed=\"50\" size=\"0\"/> </skill> </unit> <unit name=\"狼人步兵\" class=\"warrior\" hp=\"30\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"20\" size=\"4\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"2\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"10\"/> </unit> <unit name=\"狼人射手\" class=\"warrior\" hp=\"20\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"25\" size=\"2\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\"> <bullet speed=\"50\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"10\"/> </unit> </faction> <faction name=\"吸血鬼族\"> <unit name=\"吸血鬼核心\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"20\" size=\"16\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\"> <bullet speed=\"50\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼箭塔\" class=\"core\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"20\" size=\"16\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\"> <bullet speed=\"50\" size=\"0\"/> </skill> </unit> <unit name=\"吸血鬼战士\" class=\"warrior\" hp=\"30\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"20\" size=\"4\" cost=\"金子:50;人口:1\" priority=\"0\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"2\" fields=\"2\"/> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"10\"/> </unit> <unit name=\"吸血鬼弓兵\" class=\"warrior\" hp=\"20\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"25\" size=\"2\" cost=\"金子:100;人口:1\" priority=\"1\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\"> <bullet speed=\"50\" size=\"0\"/> </skill> <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"10\"/> </unit> </faction> <resource name=\"金子\" class=\"consumable\"/> <resource name=\"人口\" class=\"holdable\"/> </root>";
    setting.setTechTree(techTree);
    
    //3. set faction data
    setting.setFactionCount(2);
    setting.setThisFactionIndex(0);
    setting.setFactionTypeKey(0, "狼人族");
    setting.setFactionControlType(0, UnderWorld::Core::kFactionControlType_Human);
    setting.setTeam(0, 0);
    setting.setMapIndex(0, 0);
    setting.setFactionTypeKey(1, "吸血鬼族");
    setting.setFactionControlType(1, UnderWorld::Core::kFactionControlType_Human);
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
    cs0.resize(2);
    UnderWorld::Core::UnitSetting us00;
    us00.setUnitTypeName("狼人步兵");
    cs0[0].setUnitSetting(us00);
    cs0[0].setMaxProduction(100);
    UnderWorld::Core::UnitSetting us01;
    us01.setUnitTypeName("狼人射手");
    cs0[1].setUnitSetting(us01);
    cs0[1].setMaxProduction(100);
    setting.setCamps(0, cs0);
    
    vector<UnderWorld::Core::CampSetting> cs1;
    cs1.resize(2);
    UnderWorld::Core::UnitSetting us10;
    us10.setUnitTypeName("吸血鬼战士");
    cs1[0].setUnitSetting(us10);
    cs1[0].setMaxProduction(100);
    UnderWorld::Core::UnitSetting us11;
    us11.setUnitTypeName("吸血鬼弓兵");
    cs1[1].setUnitSetting(us11);
    cs1[1].setMaxProduction(100);
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