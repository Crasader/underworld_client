//
//  CheatLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CheatLayer.h"
#include "CocosUtils.h"
#include "CheatConfiguration.h"

using namespace std;

static const Size size(200, 200);

static CheatLayer* s_pInstance(nullptr);
CheatLayer* CheatLayer::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) CheatLayer();
        s_pInstance->init([=](Ref*) {
            const auto& winSize(Director::getInstance()->getWinSize());
            if (s_pInstance->getPositionX() == winSize.width) {
                s_pInstance->setPositionX(winSize.width - size.width);
            } else {
                s_pInstance->setPositionX(winSize.width);
            }
        });
    }
    
    return s_pInstance;
}

void CheatLayer::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

void CheatLayer::show()
{
    auto director(Director::getInstance());
    director->getRunningScene()->addChild(s_pInstance);
    const auto& winSize(director->getWinSize());
    s_pInstance->setPosition(winSize.width, winSize.height / 2);
}

CheatLayer::CheatLayer()
:_loadPVRLabel(nullptr) {}

CheatLayer::~CheatLayer()
{
    removeAllChildren();
}

bool CheatLayer::init(const function<void(Ref*)>& callback)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        setContentSize(size);
        auto bg = CocosUtils::createBackground(size);
        bg->setPosition(size.width / 2, size.height / 2);
        addChild(bg);
        _bg = bg;
        
        _loadPVRLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _loadPVRLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        _loadPVRLabel->setAnchorPoint(Point::ANCHOR_MIDDLE);
        setLoadPVR();
        
        Vector<MenuItem*> itemVec;
        auto item = MenuItemLabel::create(CocosUtils::createLabel("C", BIG_FONT_SIZE), callback);
        item->setPosition(-item->getContentSize().width / 2, size.height / 2);
        itemVec.pushBack(item);
        
        auto cc = CheatConfiguration::getInstance();
        auto itemLoadPVR = MenuItemLabel::create(_loadPVRLabel, [=](Ref*) {
            cc->loadPVR = !cc->loadPVR;
            setLoadPVR();
        });
        itemVec.pushBack(itemLoadPVR);
        
        auto menu = Menu::createWithArray(itemVec);
        menu->setPosition(Point::ZERO);
        bg->addChild(menu);
        
        // positions
        setNodePosition(0, itemLoadPVR);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(CheatLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(CheatLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool CheatLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    const auto& point = convertToNodeSpace(pTouch->getLocation());
    if (_bg->getBoundingBox().containsPoint(point)) {
        return true;
    }
    
    return false;
}

void CheatLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

void CheatLayer::setNodePosition(int idx, Node* node)
{
    if (node) {
        static const float midX(size.width / 2);
        static const float edge(8);
        static const float height(30);
        node->setPosition(midX, size.height - (edge + (idx + 0.5) * height));
    }
}

void CheatLayer::setLoadPVR()
{
    _loadPVRLabel->setString(string("Load PVR: ") + (CheatConfiguration::getInstance()->loadPVR ? "On" : "Off"));
}
