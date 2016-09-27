//
//  AchievementLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AchievementLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "AchievementManager.h"
#include "AchievementData.h"

using namespace std;
using namespace ui;

AchievementLayer* AchievementLayer::create()
{
    auto ret = new (nothrow) AchievementLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

AchievementLayer::AchievementLayer()
:_observer(nullptr)
,_tableTemplate(nullptr) {}

AchievementLayer::~AchievementLayer()
{
    CC_SAFE_DELETE(_tableTemplate);
    removeAllChildren();
}

void AchievementLayer::registerObserver(AchievementLayerObserver *observer)
{
    _observer = observer;
}

bool AchievementLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        
        // tables
        do {
            static const float edgeY(5.0f);
            Size size(winSize.width, winSize.height - edgeY * 2);
            Point position(0, winSize.height - edgeY);
            _tableTemplate = new (nothrow) TableTemplate(this, position, false, size, 1, TableTemplate::DefaultGap, this);
            _tableTemplate->setContentOffsetType(TableTemplate::ContentOffsetType::BEGIN);
        } while (false);
        
        // buttons
        CocosUtils::createGrayExitButton(this, [this]() { removeFromParent(); });
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [](Touch *pTouch, Event *pEvent) { return true; };
        eventListener->onTouchEnded = [](Touch *pTouch, Event *pEvent) {};
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

#pragma mark - TableTemplateObserver
Node* AchievementLayer::onTableTemplateCreateNodeModel(TableTemplate* tt)
{
    auto node(AchievementNode::create());
    node->registerObserver(this);
    return node;
}

void AchievementLayer::onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node)
{
    do {
        CC_BREAK_IF(idx < 0 || !node);
        const auto& lists(AchievementManager::getInstance()->getData());
        CC_BREAK_IF(idx >= lists.size());
        auto achievementNode(dynamic_cast<AchievementNode*>(node));
        CC_BREAK_IF(!achievementNode);
        achievementNode->update(lists.at(idx));
    } while (false);
}

ssize_t AchievementLayer::numberOfNodesForTableTemplate(const TableTemplate* tt)
{
    return AchievementManager::getInstance()->getData().size();
}
