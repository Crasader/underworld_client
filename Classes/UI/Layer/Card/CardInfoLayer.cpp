//
//  CardInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardInfoLayer.h"
#include "DevelopCard.h"
#include "UpgradeCard.h"
#include "CardPropertyNode.h"
#include "DeckManager.h"
#include "Board.h"
#include "UniversalButton.h"
#include "ResourceButton.h"
#include "PureNode.h"
#include "PureScale9Sprite.h"
#include "DevelopCard.h"
#include "CocosGlobal.h"
#include "CardSimpleData.h"
#include "RuneData.h"
#include "CocosUtils.h"

using namespace std;

#pragma mark - CardInfoLayer
CardInfoLayer* CardInfoLayer::create(int cardId)
{
    auto ret = new (nothrow) CardInfoLayer();
    if (ret && ret->init(cardId)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoLayer::CardInfoLayer()
:_observer(nullptr)
,_icon(nullptr)
,_level(nullptr)
,_profession(nullptr)
,_description(nullptr)
,_data(nullptr) {}

CardInfoLayer::~CardInfoLayer()
{
    removeAllChildren();
}

bool CardInfoLayer::init(int cardId)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        _data = DeckManager::getInstance()->getCardData(cardId);
        
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(2);
        board->setTitle(_data->getName());
        board->setExitCallback([this]() {
            if (_observer) {
                _observer->onCardInfoLayerExit(this);
            }
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        UniversalButton::createReturnButton(board, Vec2(8.0f, 10.0f), [this]() {
            if (_observer) {
                _observer->onCardInfoLayerReturn(this);
            }
        });
        
        createLeftNode(board->getSubNode(0));
        createRightNode(board->getSubNode(1));
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(CardInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(CardInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

#pragma mark - LayerColor
bool CardInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    const auto& point(pTouch->getLocation());
    Rect rect(getBoundingBox());
    rect.origin = getParent()->convertToWorldSpace(rect.origin);
    if (rect.containsPoint(point)) {
        return true;
    }
    
    return false;
}

void CardInfoLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - RuneCircleObserver
void CardInfoLayer::onRuneCircleClicked(const RuneData* data)
{
    
}

void CardInfoLayer::registerObserver(CardInfoLayerObserver *observer)
{
    _observer = observer;
}

int CardInfoLayer::getCard() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}

#pragma mark - UI
void CardInfoLayer::createLeftNode(Node* node)
{
    static const Vec2 secondaryEdge(5, 5);
    const auto& size(node->getContentSize());
    const Size barSize(size.width - secondaryEdge.x * 2, 36);
    
    // top bar
    {
        auto bar = PureScale9Sprite::create(PureScale9Sprite::Type::Purple);
        bar->setContentSize(barSize);
        bar->setPosition(size.width / 2, size.height - (secondaryEdge.y + barSize.height / 2));
        node->addChild(bar);
        
        static const float edgeX(5);
        auto label = CocosUtils::createLabel("Lv.1", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        label->setPosition(edgeX, barSize.height / 2);
        bar->addChild(label);
        _level = label;
        
        label = CocosUtils::createLabel("Warrior", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        label->setPosition(barSize.width - edgeX, barSize.height / 2);
        bar->addChild(label);
        _profession = label;
    }
    
    static const float spaceBarCard(15);
    auto card = DevelopCard::create(0);
    const auto& cardSize(card->getContentSize());
    card->setPosition(Point(secondaryEdge.x + cardSize.width / 2, size.height - (secondaryEdge.y + barSize.height + spaceBarCard + cardSize.height / 2)));
    node->addChild(card);
    _icon = card;
    
    // description
    {
        static const float spaceBarDesc(5);
        static const float spaceCardDesc(20);
        const Size descSize(barSize.width - (cardSize.width + spaceCardDesc), spaceBarCard + cardSize.height - spaceBarDesc);
        auto descBg = PureScale9Sprite::create(PureScale9Sprite::Type::White);
        descBg->setContentSize(descSize);
        descBg->setPosition(size.width - (secondaryEdge.x + descSize.width / 2), size.height - (secondaryEdge.y + barSize.height + spaceBarDesc + descSize.height / 2));
        node->addChild(descBg);
        
        static const float edge(5);
        auto label = CocosUtils::createLabel("description", DEFAULT_FONT_SIZE);
        label->setDimensions(descSize.width - edge * 2, descSize.height - edge * 2);
        label->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
        label->setTextColor(Color4B::BLACK);
        label->setPosition(descSize.width / 2, descSize.height / 2);
        descBg->addChild(label);
        _description = label;
    }
    
    static const float spaceCardLine(15);
    auto line = Sprite::create(CocosUtils::getResourcePath("ui_line_2.png"));
    line->setPosition(size.width / 2, card->getPositionY() - (cardSize.height / 2 + spaceCardLine));
    node->addChild(line);
    
    // line
    {
        auto label = CocosUtils::createLabel("properties", DEFAULT_FONT_SIZE);
        label->setTextColor(Color4B::BLACK);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setPosition(line->getContentSize().width / 2, line->getContentSize().height / 2);
        line->addChild(label);
    }
    
    static const float spaceLineProperty(20);
    
    // properties
    {
        static const int columnCount(2);
        static const float spaceY(5);
        float spaceX(0);
        Size propertySize(Size::ZERO);
        for (int i = 0; i < 10; ++i) {
            const int row(i / columnCount);
            const int column(i % columnCount);
            Color4B color;
            if (row % 2 == 0) {
                color = PURE_GRAY;
            } else {
                color = PURE_WHITE;
            }
            
            auto property = CardPropertyNode::create(color);
            node->addChild(property);
            _properties.push_back(property);
            
            // calculate space first
            if (0 == i) {
                propertySize = property->getContentSize();
                if (columnCount > 1) {
                    spaceX = (size.width - (secondaryEdge.x * 2 + propertySize.width * columnCount)) / (columnCount - 1);
                }
            }
            property->setPosition(secondaryEdge.x + (propertySize.width + spaceX) * column + propertySize.width / 2, line->getPositionY() - (spaceLineProperty + (propertySize.height + spaceY) * row + propertySize.height / 2));
        }
    }
    
    // bottom bar
    {
        auto bar = PureScale9Sprite::create(PureScale9Sprite::Type::BlueDeep);
        bar->setContentSize(barSize);
        bar->setPosition(size.width / 2, secondaryEdge.y + barSize.height / 2);
        node->addChild(bar);
        
        static const float edgeX(5);
        auto label = CocosUtils::createLabel("Upgrade", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        label->setPosition(edgeX, barSize.height / 2);
        bar->addChild(label);
        
        auto button = ResourceButton::create(true, false, ResourceType::Gold, 3000, Color4B::BLACK, [this](Ref*) {
            // TODO
        });
        button->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        button->setPosition(barSize.width / 2, 0);
        bar->addChild(button);
    }
}

void CardInfoLayer::createRightNode(Node* node)
{
    static const Vec2 secondaryEdge(10, 5);
    const auto& size(node->getContentSize());
    
    auto skillTitle = CocosUtils::createLabel("Hero Skills", DEFAULT_FONT_SIZE);
    skillTitle->setTextColor(Color4B::BLACK);
    skillTitle->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
    skillTitle->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    node->addChild(skillTitle);
    
    const auto& stsize(skillTitle->getContentSize());
    skillTitle->setPosition(secondaryEdge.x, size.height - (stsize.height / 2 + secondaryEdge.y));
    
    static const float cardHintSpace(5);
    const float cardBasePosY(size.height - (secondaryEdge.y + stsize.height + cardHintSpace));
    static const int skillsCount(4);
    Size cardSize(Size::ZERO);
    float spaceX(0);
    for (int i = 0; i < skillsCount; ++i) {
        auto card = UpgradeCard::create(0);
        node->addChild(card);
        
        if (0 == i) {
            cardSize = card->getContentSize();
            spaceX = (size.width - cardSize.width * skillsCount) / (skillsCount + 1);
        }
        
        card->setPosition(Point((spaceX + cardSize.width) * (i + 1) - cardSize.width / 2, cardBasePosY - cardSize.height / 2));
    }
    
    static const float cardLineSpace(10);
    static const float lineEdgeX(5);
    static const float lineHeight(2);
    auto line = PureNode::createLine(Size(size.width - lineEdgeX * 2, lineHeight));
    line->setPosition(size.width / 2, cardBasePosY - (cardSize.height + cardLineSpace + lineHeight / 2));
    node->addChild(line);
    
    auto runeTitle = CocosUtils::createLabel("Hero Runes", DEFAULT_FONT_SIZE);
    runeTitle->setTextColor(Color4B::BLACK);
    runeTitle->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
    runeTitle->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    node->addChild(runeTitle);
    
    const auto& rtsize(runeTitle->getContentSize());
    runeTitle->setPosition(secondaryEdge.x, line->getPositionY() - ((lineHeight + rtsize.height) / 2 + secondaryEdge.y));
    
    {
        auto circle = RuneCircle::create();
        circle->registerObserver(this);
        node->addChild(circle);
        
        static const Vec2 circleEdge(40, 10);
        const auto& csize(circle->getContentSize());
        circle->setPosition(circleEdge.x + csize.width / 2, circleEdge.y + csize.height / 2);
        
        static const vector<const RuneData*> runeDatas = {nullptr, nullptr, nullptr};
        circle->setData(runeDatas);
        
        static const float spaceY(10);
        for (int i = 0; i < runeDatas.size(); ++i) {
            Color4B color;
            if (i % 2 == 0) {
                color = PURE_WHITE;
            } else {
                color = PURE_GRAY;
            }
            
            static const Size bgSize(308, 41);
            auto bg = PureNode::create(color, bgSize);
            bg->setPosition(size.width - (secondaryEdge.x + bgSize.width / 2), line->getPositionY() - (lineHeight / 2 + (spaceY + bgSize.height) * (i + 1) - bgSize.height / 2));
            node->addChild(bg);
            
            auto data(runeDatas.at(i));
            if (data) {
                auto label = CocosUtils::createLabel(data->getDescription(), DEFAULT_FONT_SIZE);
                label->setTextColor(Color4B::BLACK);
                label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                label->setPosition(10, bgSize.height / 2);
                bg->addChild(label);
            }
        }
    }
    
    {
        static const Vec2 edge(20, 10);
        auto unload = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, "Unload");
        unload->setCallback([this](Ref*) {
            
        });
        node->addChild(unload);
        const auto& usize(unload->getContentSize());
        unload->setPosition(size.width - (edge.x + usize.width / 2), edge.y + usize.height / 2);
        
        auto change = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, "Change");
        change->setCallback([this](Ref*) {
            
        });
        node->addChild(change);
        
        const auto& csize(change->getContentSize());
        change->setPosition(size.width - (edge.x * 2 + usize.width + csize.width / 2), unload->getPositionY());
    }
}
