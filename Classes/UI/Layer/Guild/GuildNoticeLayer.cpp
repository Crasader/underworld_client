//
//  GuildNoticeLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildNoticeLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "UniversalButton.h"
#include "MediumBoard.h"

using namespace std;
using namespace ui;

GuildNoticeLayer* GuildNoticeLayer::create()
{
    auto ret = new (nothrow) GuildNoticeLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildNoticeLayer::GuildNoticeLayer()
:_observer(nullptr)
,_noticeTitle(nullptr)
,_notice(nullptr) {}

GuildNoticeLayer::~GuildNoticeLayer()
{
    removeAllChildren();
}

void GuildNoticeLayer::registerObserver(GuildNoticeLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool GuildNoticeLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = MediumBoard::create();
        board->setTitle(LocalHelper::getString("ui_rename_title"));
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto button = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, "Send");
        button->setCallback([this](Ref*) {
            if (_observer && _noticeTitle && _notice) {
                const char* title(_noticeTitle->getText());
                const char* notice(_notice->getText());
                if (title && notice && strlen(title) > 0 && strlen(notice) > 0) {
                    _observer->onGuildNoticeLayerSend(this, title, notice);
                }
            }
        });
        board->addChildToMidBottom(button);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(GuildNoticeLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(GuildNoticeLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool GuildNoticeLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void GuildNoticeLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - EditBoxDelegate
void GuildNoticeLayer::editBoxReturn(EditBox* editBox)
{
    
}
