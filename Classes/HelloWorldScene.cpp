#include "HelloWorldScene.h"
#include "GameRender.h"
#include "GameSettings.h"
#include "MapLayer.h"
#include "MapUILayer.h"

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

    _render = new GameRender(this, 1);
    _sch = new GameScheduler();
    UnderWorld::Core::GameSettings setting;
    setting.setTechTree("<root>  <faction name=\"狼人族\">            <unit name=\"狼人基地\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"0\" size=\"16\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> </unit>    <unit name=\"狼人步兵\" class=\"warrior\" hp=\"30\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"100\" size=\"4\">                    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"2\" fields=\"2\"/>                        <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/>              </unit>         <unit name=\"狼人射手\" class=\"warrior\" hp=\"20\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"150\" size=\"2\">                    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\">                               <bullet speed=\"50\" size=\"0\"/>                      </skill>                        <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"15\"/>              </unit> </faction>      <faction name=\"吸血鬼族\">          <unit name=\"吸血鬼核心\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"0\" size=\"16\"> <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/> <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/> </unit>   <unit name=\"吸血鬼战士\" class=\"warrior\" hp=\"30\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"100\" size=\"4\">                  <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" mindamage=\"5\" maxdamage=\"8\" range=\"2\" fields=\"2\"/>                        <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/>              </unit>         <unit name=\"吸血鬼弓兵\" class=\"warrior\" hp=\"20\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" attacksight=\"150\" size=\"2\">                  <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"die\" preperform=\"10.0\" perform=\"0.0\" cd=\"0.0\"/>                   <skill class=\"attack\" preperform=\"0.2\" perform=\"0.0\" cd=\"0.8\" mindamage=\"8\" maxdamage=\"10\" range=\"20\" fields=\"3\">                               <bullet speed=\"50\" size=\"0\"/>                      </skill>                        <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"15\"/>              </unit> </faction></root>");
    setting.setMap(_render->getMapLayer()->mapSetting);
    setting.setFactionCount(2);
    setting.setThisFactionIndex(0);
    setting.setTeam(0, 0);
    setting.setTeam(1, 1);
    setting.setFactionControlType(0, UnderWorld::Core::kFactionControlType_Cpu);
    setting.setFactionControlType(1, UnderWorld::Core::kFactionControlType_Cpu);
    setting.setFactionTypeKey(0, "狼人族");
    setting.setFactionTypeKey(1, "吸血鬼族");
    std::vector<std::pair<std::string, UnderWorld::Core::Coordinate> > units1;
    for (int i = 0; i < 10; ++i) {
        units1.push_back(std::make_pair("狼人步兵", UnderWorld::Core::Coordinate(140, 140)));
    }
    
    units1.push_back(std::make_pair("狼人基地", UnderWorld::Core::Coordinate(140, 140)));
    //setting.setFactionStartUnits(0, units1);
    
    std::vector<std::pair<std::string, UnderWorld::Core::Coordinate> > units2;
    for (int i = 0; i < 10; ++i) {
        units2.push_back(std::make_pair("吸血鬼战士", UnderWorld::Core::Coordinate(900, 140)));
    }
    units2.push_back(std::make_pair("吸血鬼核心", UnderWorld::Core::Coordinate(900, 140)));
    //setting.setFactionStartUnits(1, units2);
    _looper = new UnderWorld::Core::GameLooper(_render, _sch);
    _looper->init(setting);
    _looper->start();
    
    //--------- UI Layer ---------//
    MapUILayer* uiLayer = MapUILayer::create("我的名字", "对手的名字");
    addChild(uiLayer);
    
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