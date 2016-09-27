//
//  LanguageLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LanguageLayer.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"

using namespace std;

static const vector<LocalType>& languages(LocalHelper::getSupportedLocalTypes());

LanguageLayer* LanguageLayer::create(const Size& size)
{
    auto ret = new (nothrow) LanguageLayer();
    if (ret && ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LanguageLayer::LanguageLayer()
:_observer(nullptr)
,_tableTemplate(nullptr)
,_selectedIdx(CC_INVALID_INDEX)
{
    for (auto i = 0; i < languages.size(); ++i) {
        if (LocalHelper::getLocalType() == languages.at(i)) {
            _selectedIdx = i;
            break;
        }
    }
}

LanguageLayer::~LanguageLayer()
{
    CC_SAFE_DELETE(_tableTemplate);
    removeAllChildren();
}

void LanguageLayer::registerObserver(LanguageLayerObserver *observer)
{
    _observer = observer;
}

bool LanguageLayer::init(const Size& size)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        setContentSize(size);
        
        do {
            Point position(0, size.height);
            _tableTemplate = new (nothrow) TableTemplate(this, position, false, size, 1, TableTemplate::DefaultGap, this);
            _tableTemplate->setContentOffsetType(TableTemplate::ContentOffsetType::BEGIN);
        } while (false);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(LanguageLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(LanguageLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool LanguageLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return false;
}

void LanguageLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - TableTemplateObserver
Node* LanguageLayer::onTableTemplateCreateNodeModel(TableTemplate* tt)
{
    auto node(LanguageNode::create());
    node->registerObserver(this);
    return node;
}

void LanguageLayer::onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node)
{
    do {
        CC_BREAK_IF(idx < 0 || !node);
        CC_BREAK_IF(idx >= languages.size());
        auto languageNode(dynamic_cast<LanguageNode*>(node));
        CC_BREAK_IF(!languageNode);
        auto name(LocalHelper::getLanguageName(languages.at(idx)));
        languageNode->update(name);
        languageNode->setIdx(idx);
        languageNode->tick(idx == _selectedIdx);
    } while (false);
}

ssize_t LanguageLayer::numberOfNodesForTableTemplate(const TableTemplate* tt)
{
    return languages.size();
}

#pragma mark - LanguageNodeObserver
void LanguageLayer::onLanguageNodeSelected(ssize_t idx)
{
    auto confirmLayer = LanguageConfirmationLayer::create(idx);
    confirmLayer->registerObserver(this);
    Director::getInstance()->getRunningScene()->addChild(confirmLayer);
}

#pragma mark - LanguageConfirmationLayerObserver
void LanguageLayer::onLanguageConfirmationLayerConfirm(Node* pSender, ssize_t idx)
{
    if (pSender) {
        pSender->removeFromParent();
    }
    
    do {
        CC_BREAK_IF(idx == _selectedIdx);
        CC_BREAK_IF(!_tableTemplate);
        auto table(_tableTemplate->getTableView());
        CC_BREAK_IF(!table);
        auto prior(_selectedIdx);
        _selectedIdx = idx;
        
        if (CC_INVALID_INDEX != prior) {
            table->updateCellAtIndex(prior);
        }
        
        if (CC_INVALID_INDEX != idx) {
            table->updateCellAtIndex(idx);
        }
        
        if (_observer) {
            _observer->onLanguageLayerSelected(idx);
        }
    } while (false);
}
