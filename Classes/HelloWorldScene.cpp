#include "HelloWorldScene.h"
#include "2d/CCFastTMXLayer.h"
#include "2d/CCFastTMXTiledMap.h"
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

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
//    
//    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
//    
//    // position the label on the center of the screen
//    label->setPosition(Vec2(origin.x + visibleSize.width/2,
//                            origin.y + visibleSize.height - label->getContentSize().height));
//
//    // add the label as a child to this layer
//    this->addChild(label, 1);
//
    // add "HelloWorld" splash screen"
//    auto sprite = Sprite::create("map/bg.png");
//
//    // position the sprite on the center of the screen
//    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
//
//    // add the sprite as a child to this layer
//    this->addChild(sprite, 0);
    
    
//    FileUtils::getInstance()->addSearchPath("map");
//    
//    auto *map = cocos2d::experimental::TMXTiledMap::create("test4.tmx");
//    //        map->setPosition(Point(480.0f, 320.0f));
//    addChild(map, 2, 1);

    MapLayer *map = MapLayer::create(1);
    addChild(map, 3);
    
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

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
