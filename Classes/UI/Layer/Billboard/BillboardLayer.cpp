//
//  BillboardLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BillboardLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "Board.h"
#include "BillboardManager.h"

using namespace std;

BillboardLayer* BillboardLayer::create()
{
    auto ret = new (nothrow) BillboardLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BillboardLayer::BillboardLayer()
:_observer(nullptr)
,_tableTemplate(nullptr)
,_rank(nullptr)
,_trophy(nullptr) {}

BillboardLayer::~BillboardLayer()
{
    CC_SAFE_DELETE(_tableTemplate);
    removeAllChildren();
}

void BillboardLayer::registerObserver(BillboardLayerObserver *observer)
{
    _observer = observer;
}

bool BillboardLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(1);
        board->setTitle(LocalHelper::getString("ui_rank_title"));
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        const auto& size(board->getContentSize());
        static const float edgeTop(45);
        {
            static const float edgeLeft(20);
            auto hint = CocosUtils::createLabel(LocalHelper::getString("ui_rank_myRank"), BIG_FONT_SIZE);
            hint->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            hint->setPosition(edgeLeft, size.height - edgeTop);
            board->addChild(hint);
            
            auto label = CocosUtils::createLabel("", BIG_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(hint->getPosition() + Point(hint->getContentSize().width, 0));
            board->addChild(label);
            _rank = label;
        }
        
        {
            static const float edgeRight(130);
            auto label = CocosUtils::createLabel("", BIG_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(size.width - edgeRight, size.height - edgeTop);
            board->addChild(label);
            _trophy = label;
            
            static const float lsOffset(0);
            const auto& lsize(label->getContentSize());
            auto sprite = Sprite::create(CocosUtils::getResourcePath("icon_jiangbei.png"));
            const auto& ssize(sprite->getContentSize());
            sprite->setPosition(label->getPosition() - Point(ssize.width / 2 + lsOffset, (lsize.height - ssize.height) / 2));
            board->addChild(sprite);
            
            static const float shOffset(0);
            auto hint = CocosUtils::createLabel(LocalHelper::getString("ui_rank_myTrophy"), BIG_FONT_SIZE);
            const auto& hsize(hint->getContentSize());
            hint->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
            hint->setPosition(sprite->getPosition() - Point(ssize.width / 2 + shOffset, (ssize.height - hsize.height) / 2));
            board->addChild(hint);
        }
        
        do {
            auto subNode(board->getSubNode(0));
            CC_BREAK_IF(!subNode);
            static const float edgeY(5.0f);
            const auto& subNodeSize(subNode->getContentSize());
            Size size(subNodeSize.width, subNodeSize.height - edgeY * 2);
            Point position(0, subNodeSize.height - edgeY);
            _tableTemplate = new (nothrow) TableTemplate(subNode, position, false, size, 1, TableTemplate::DefaultGap, this);
            _tableTemplate->setContentOffsetType(TableTemplate::ContentOffsetType::BEGIN);
        } while (false);
        
        updateRank(30);
        updateTrophy(1000);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(BillboardLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(BillboardLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool BillboardLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void BillboardLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - TableTemplateObserver
Node* BillboardLayer::onTableTemplateCreateNodeModel(TableTemplate* tt)
{
    auto node(BillboardCell::create());
    node->registerObserver(this);
    return node;
}

void BillboardLayer::onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node)
{
    do {
        CC_BREAK_IF(idx < 0 || !node);
        const auto& lists(BillboardManager::getInstance()->getBillboard());
        CC_BREAK_IF(idx >= lists.size());
        auto card(dynamic_cast<BillboardCell*>(node));
        CC_BREAK_IF(!card);
        card->update(lists.at(idx));
    } while (false);
}

ssize_t BillboardLayer::numberOfNodesForTableTemplate(const TableTemplate* tt)
{
    return BillboardManager::getInstance()->getBillboard().size();
}

#pragma mark - BillboardCellObserver
void BillboardLayer::onBillboardCellClicked(const BillboardData* data)
{
    
}

void BillboardLayer::updateRank(int rank)
{
    if (_rank) {
        _rank->setString(StringUtils::format("%d", rank));
    }
}

void BillboardLayer::updateTrophy(int trophy)
{
    if (_trophy) {
        _trophy->setString(StringUtils::format("%d", trophy));
    }
}
