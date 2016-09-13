//
//  TestLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TestLayer.h"
#include "CocosUtils.h"
#include "TestConfiguration.h"
#include "PureNode.h"

using namespace std;

static const Size size(200, 200);

static TestLayer* s_pInstance(nullptr);
TestLayer* TestLayer::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) TestLayer();
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

void TestLayer::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

void TestLayer::show(Node* parent)
{
    removeFromParent();
    
    if (parent) {
        parent->addChild(this);
        const auto& size(parent->getContentSize());
        setPosition(size.width, size.height / 2);
    }
}

TestLayer::TestLayer()
:_bg(nullptr) {}

TestLayer::~TestLayer()
{
    removeAllChildren();
}

bool TestLayer::init(const function<void(Ref*)>& callback)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        setContentSize(size);
        auto bg = PureNode::create(PURE_BLACK, size);
        bg->setPosition(size.width / 2, size.height / 2);
        addChild(bg);
        _bg = bg;
        
        Vector<MenuItem*> itemVec;
        auto item = MenuItemLabel::create(CocosUtils::createLabel("T", BIG_FONT_SIZE), callback);
        item->setPosition(-item->getContentSize().width / 2, size.height / 2);
        itemVec.pushBack(item);
        
        // add custom items
        static const size_t count(2);
        for (int i = 0; i < count; ++i) {
            itemVec.pushBack(createMenuItem(bind(&TestLayer::onMenuItemClicked, this, placeholders::_1, placeholders::_2)));
        }
        
        auto menu = Menu::createWithArray(itemVec);
        menu->setPosition(Point::ZERO);
        bg->addChild(menu);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(TestLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(TestLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool TestLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    const auto& point = convertToNodeSpace(pTouch->getLocation());
    if (_bg->getBoundingBox().containsPoint(point)) {
        return true;
    }
    
    return false;
}

void TestLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

MenuItem* TestLayer::createMenuItem(const Callback& callback)
{
    const auto idx(_labels.size());
    
    auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
    label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _labels.push_back(label);
    
    if (callback) {
        callback(idx, true);
    }
    
    auto item = MenuItemLabel::create(label, [=](Ref*) {
        if (callback) {
            callback(idx, false);
        }
    });
    
    if (item) {
        static const float midX(size.width / 2);
        static const float edge(8);
        static const float height(30);
        item->setPosition(midX, size.height - (edge + (idx + 0.5) * height));
    }
    
    return item;
}

void TestLayer::onMenuItemClicked(size_t idx, bool initialized)
{
    if (_labels.size() > idx) {
        auto label(_labels.at(idx));
        CC_ASSERT(label);
        
        auto tc = TestConfiguration::getInstance();
        switch (idx) {
            case 0: {
                if (!initialized) {
                    tc->setPVR(!tc->isPVREnabled());
                }
                
                label->setString(string("Load PVR: ") + (tc->isPVREnabled() ? "On" : "Off"));
            }
                break;
                
            case 1: {
                if (!initialized) {
                    tc->setOnlineChat(!tc->isOnlineChatEnabled());
                }
                
                label->setString(string("Online Chat: ") + (tc->isOnlineChatEnabled() ? "On" : "Off"));
            }
                break;
                
            default:
                break;
        }
    }
}
