//
//  RenameLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RenameLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "SettingUI.h"

using namespace std;

static const string getResourcePath(const string& file) {
    return "GameImages/settings_ui/" + file;
}

RenameLayer* RenameLayer::create()
{
    RenameLayer *ret = new (nothrow) RenameLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

RenameLayer::RenameLayer()
:_observer(nullptr) {}

RenameLayer::~RenameLayer()
{
    removeAllChildren();
}

void RenameLayer::registerObserver(RenameLayerObserver *observer)
{
    _observer = observer;
}

bool RenameLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto bg = Sprite::create(getResourcePath("ui_background_3.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        
        auto subNode = SettingUI::createSubBackground(Size(579, 202));
        bg->addChild(subNode);
        
        const auto& size(bg->getContentSize());
        const auto& subBgSize(subNode->getContentSize());
        const float edge((size.height - subBgSize.height) / 2);
        subNode->setPosition(Point(size.width / 2, subBgSize.height / 2 + edge));
        
        // sub node
        static const float edgeY(10.0f);
        {
            auto s = Sprite::create(SettingUI::getResourcePath("ui_tiao_8.png"));
            const auto& size(s->getContentSize());
            s->setPosition(Point(subBgSize.width / 2, subBgSize.height - (size.height / 2 + edgeY)));
            subNode->addChild(s);
            
            auto hint = CocosUtils::createLabel("Only change once", DEFAULT_FONT_SIZE);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE);
            hint->setPosition(Point(size.width / 2, size.height / 2));
            s->addChild(hint);
        }
        
        {
            auto hint = CocosUtils::createLabel("Change your nickname", DEFAULT_FONT_SIZE);
            hint->setTextColor(Color4B::BLACK);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE);
            hint->setPosition(Point(subBgSize.width / 2, subBgSize.height / 2));
            subNode->addChild(hint);
        }
        
        {
            auto s = ui::Scale9Sprite::create(Rect(5, 5, 337, 43), SettingUI::getResourcePath("ui_kuang_6.png"));
            auto eb = ui::EditBox::create(Size(347, 53), s);
            const auto& size(eb->getContentSize());
            eb->setPosition(Point(subBgSize.width / 2, size.height / 2 + edgeY));
            subNode->addChild(eb);
        }
        
        SettingUI::createExitButton(bg, [this]() {
            removeFromParent();
        });
        
        auto title = CocosUtils::createLabel("Rename", BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subBgSize.height + edge) / 2));
        bg->addChild(title);
        
        auto button = SettingUI::createButton(SettingUI::ButtonType::Normal, "Finish");
        button->addClickEventListener([](Ref*) {
            
        });
        button->setPosition(Point(size.width / 2, edge / 2));
        bg->addChild(button);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(RenameLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(RenameLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool RenameLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void RenameLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
