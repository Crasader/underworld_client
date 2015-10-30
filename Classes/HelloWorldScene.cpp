#include "HelloWorldScene.h"
#include "GameRender.h"

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