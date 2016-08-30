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
#include "Board.h"
#include "UniversalButton.h"
#include "ResourceButton.h"
#include "PureNode.h"
#include "PureScale9Sprite.h"
#include "DevelopCard.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"
#include "CardSimpleData.h"
#include "RuneData.h"
#include "CocosUtils.h"

using namespace std;

#pragma mark - CardInfoLayer
CardInfoLayer* CardInfoLayer::create(const CardSimpleData* data)
{
    auto ret = new (nothrow) CardInfoLayer();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoLayer::CardInfoLayer()
:_observer(nullptr)
,_board(nullptr)
,_icon(nullptr)
,_level(nullptr)
,_profession(nullptr)
,_description(nullptr)
,_selectedRune(nullptr)
,_data(nullptr) {}

CardInfoLayer::~CardInfoLayer()
{
    removeAllChildren();
}

bool CardInfoLayer::init(const CardSimpleData* data)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(2);
        board->setTitle("untitled");
        board->setExitCallback([this]() {
            if (_observer) {
                _observer->onCardInfoLayerExit(this);
            }
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        _board = board;
        
        UniversalButton::createReturnButton(board, Vec2(8.0f, 10.0f), [this]() {
            if (_observer) {
                _observer->onCardInfoLayerReturn(this);
            }
        });
        
        createLeftNode(board->getSubNode(0));
        createRightNode(board->getSubNode(1));
        
        update(data);
        
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
void CardInfoLayer::onRuneCircleClicked(RuneNode* node)
{
    if (_selectedRune) {
        _selectedRune->select(false);
    }
    
    node->select(true);
    _selectedRune = node;
}

#pragma mark - RuneBagLayerObserver
void CardInfoLayer::onRuneBagLayerSelected(Node* pSender, const RuneData* data)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

#pragma mark - UpgradeCardObserver
void CardInfoLayer::onUpgradeCardInfo(UpgradeCard* pSender)
{
    auto layer = SpellInfoLayer::create(pSender->getCardData());
    layer->registerObserver(this);
    addChild(layer);
}

void CardInfoLayer::onUpgradeCardUpgrade(UpgradeCard* pSender)
{
    
}

#pragma mark - SpellInfoLayerObserver
void CardInfoLayer::onSpellInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void CardInfoLayer::onSpellInfoLayerUpgrade(Node* pSender, const CardSimpleData* data)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

#pragma mark - public
void CardInfoLayer::registerObserver(CardInfoLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - UI
void CardInfoLayer::createLeftNode(Node* node)
{
    static const Vec2 secondaryEdge(5, 5);
    const auto& size(node->getContentSize());
    const Size barSize(size.width - secondaryEdge.x * 2, 36);
    
    // top bar
    {
        PureScale9Sprite::Type type;
        // TODO:
        type = PureScale9Sprite::Type::Purple;
        auto bar = PureScale9Sprite::create(type);
        bar->setContentSize(barSize);
        bar->setPosition(size.width / 2, size.height - (secondaryEdge.y + barSize.height / 2));
        node->addChild(bar);
        
        static const float edgeX(5);
        auto label = CocosUtils::createLabel("level", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        label->setPosition(edgeX, barSize.height / 2);
        bar->addChild(label);
        _level = label;
        
        label = CocosUtils::createLabel("profession", DEFAULT_FONT_SIZE);
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
        auto label = CocosUtils::createLabel(LocalHelper::getString("ui_cardInfo_property"), DEFAULT_FONT_SIZE);
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
        static const float spaceY(6);
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
        auto label = CocosUtils::createLabel(LocalHelper::getString("ui_cardInfo_upgrade_x"), DEFAULT_FONT_SIZE);
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
    
    auto skillTitle = CocosUtils::createLabel(LocalHelper::getString("ui_cardInfo_skill"), BIG_FONT_SIZE);
    skillTitle->setTextColor(Color4B::BLACK);
    skillTitle->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
    skillTitle->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    node->addChild(skillTitle);
    
    const auto& stsize(skillTitle->getContentSize());
    skillTitle->setPosition(secondaryEdge.x, size.height - (stsize.height / 2 + secondaryEdge.y));
    
    static const float cardHintSpace(15);
    const float cardBasePosY(size.height - (secondaryEdge.y + stsize.height + cardHintSpace));
    static const int skillsCount(4);
    Size cardSize(Size::ZERO);
    float spaceX(0);
    for (int i = 0; i < skillsCount; ++i) {
        auto card = UpgradeCard::create(0);
        card->registerObserver(this);
        node->addChild(card);
        
        if (0 == i) {
            cardSize = card->getContentSize();
            spaceX = (size.width - cardSize.width * skillsCount) / (skillsCount + 1);
        }
        
        card->setPosition(Point((spaceX + cardSize.width) * (i + 1) - cardSize.width / 2, cardBasePosY - cardSize.height / 2));
    }
    
    static const float cardLineSpace(10);
    static const float lineEdgeX(25);
    static const float lineHeight(2);
    auto line = PureNode::createLine(Size(size.width - lineEdgeX * 2, lineHeight));
    line->setPosition(size.width / 2, cardBasePosY - (cardSize.height + cardLineSpace + lineHeight / 2));
    node->addChild(line);
    
    {
        auto title = CocosUtils::createLabel(LocalHelper::getString("ui_cardInfo_rune"), BIG_FONT_SIZE);
        title->setTextColor(Color4B::BLACK);
        title->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        node->addChild(title);
        
        const auto& tsize(title->getContentSize());
        title->setPosition(secondaryEdge.x, line->getPositionY() - ((lineHeight + tsize.height) / 2 + secondaryEdge.y));
    }
    
    {
        auto circle = RuneCircle::create();
        circle->registerObserver(this);
        node->addChild(circle);
        
        static const Vec2 circleEdge(40, 18);
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
    
    createOpButtons(node);
}

static const int opButtonsCount(2);
void CardInfoLayer::createOpButtons(Node* node)
{
    if (!node) {
        return;
    }
    
    const auto& size(node->getContentSize());
    static const Vec2 edge(20, 10);
    static const string opButtonsTitles[opButtonsCount] = {
        LocalHelper::getString("ui_cardInfo_unload"),
        LocalHelper::getString("ui_cardInfo_change"),
    };
    
    for (int i = 0; i < opButtonsCount; ++i) {
        auto button = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, opButtonsTitles[i]);
        button->setCallback([this, i](Ref*) { onOpButtonClicked(i); });
        node->addChild(button);
        const auto& bsize(button->getContentSize());
        button->setPosition(size.width - ((edge.x + bsize.width) * (i + 1) - bsize.width / 2), edge.y + bsize.height / 2);
    }
}

void CardInfoLayer::onOpButtonClicked(int idx)
{
    if (!_selectedRune) {
        MessageBox("请选择一个符文", nullptr);
    } else if (idx < opButtonsCount) {
        if (0 == idx) {
            
        } else if (1 == idx) {
            auto layer = RuneBagLayer::create(_selectedRune->getData());
            layer->registerObserver(this);
            addChild(layer);
        }
    }
}

void CardInfoLayer::update(const CardSimpleData* data)
{
    if (_data != data) {
        _data = data;
        
        if (_board) {
            _board->setTitle(data ? data->getName() : "");
        }
        
        if (_icon) {
            _icon->update(data);
        }
        
        if (_level) {
            _level->setString(data ? StringUtils::format("LV.%d", data->getLevel()) : "");
        }
        
        if (_profession) {
            _profession->setString(data ? "" : "");
        }
        
        if (_description) {
            _description->setString(data ? data->getDescription() : "");
        }
    }
}
