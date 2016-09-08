//
//  GuildSearchLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/6.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildSearchLayer.h"
#include "CocosGlobal.h"
#include "Board.h"
#include "TabButton.h"
#include "GuildUISearchNode.h"
#include "GuildUICreateNode.h"

using namespace std;

GuildSearchLayer* GuildSearchLayer::create()
{
    auto ret = new (nothrow) GuildSearchLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildSearchLayer::GuildSearchLayer()
:_observer(nullptr) {}

GuildSearchLayer::~GuildSearchLayer()
{
    removeAllChildren();
}

void GuildSearchLayer::registerObserver(GuildSearchLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool GuildSearchLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(1);
        board->setTitle("");
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto subNode(board->getSubNode(0));
        const auto& size(subNode->getContentSize());
        static const vector<string> tabTitles {"Search clan", "Create clan"};
        for (int i = 0; i < tabTitles.size(); ++i) {
            static const float startX(100);
            static const float offsetX(40);
            auto button = TabButton::create(tabTitles.at(i), [this, i](Ref*) {
                setTabButtonSelected(i);
                
            });
            const auto& bsize(button->getContentSize());
            button->setPosition(Point(startX + bsize.width / 2 + (bsize.width + offsetX) * i, size.height + bsize.height / 2));
            subNode->addChild(button);
            _tabButtons.push_back(button);
            
            Node* ui(nullptr);
            if (0 == i) {
                ui = GuildUISearchNode::create(size);
            } else if (1 == i) {
                ui = GuildUICreateNode::create(size);
            }
            
            if (ui) {
                ui->setPosition(size.width / 2, size.height / 2);
                subNode->addChild(ui);
                _subNodes.push_back(ui);
            }
        }
        
        setTabButtonSelected(0);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(GuildSearchLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(GuildSearchLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool GuildSearchLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void GuildSearchLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - private
void GuildSearchLayer::setTabButtonSelected(int idx)
{
    for (int i = 0; i < _tabButtons.size(); ++i) {
        _tabButtons.at(i)->setEnabled(i != idx);
        
        if (i < _subNodes.size()) {
            _subNodes.at(i)->setVisible(i == idx);
        }
    }
}
