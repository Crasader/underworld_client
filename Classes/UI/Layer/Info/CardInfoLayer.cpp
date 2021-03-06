//
//  CardInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardInfoLayer.h"
#include "SeniorCard.h"
#include "CardAttributeNode.h"
#include "Board.h"
#include "XButton.h"
#include "ResourceButton.h"
#include "PureNode.h"
#include "PureScale9Sprite.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"
#include "DeckManager.h"
#include "DataManager.h"
#include "AbstractProperty.h"
#include "AbstractUpgradeProperty.h"
#include "CardProperty.h"
#include "CardData.h"
#include "SkillData.h"
#include "RuneData.h"
#include "CocosUtils.h"

using namespace std;

CardInfoLayer* CardInfoLayer::create(int cardId, const AbstractData* data)
{
    auto ret = new (nothrow) CardInfoLayer();
    if (ret && ret->init(cardId, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoLayer::CardInfoLayer()
:_observer(nullptr)
,_level(nullptr)
,_rarityBar(nullptr)
,_profession(nullptr)
,_runeCircle(nullptr)
,_selectedRune(nullptr) {}

CardInfoLayer::~CardInfoLayer()
{
    removeAllChildren();
}

#pragma mark - RuneCircleObserver
void CardInfoLayer::onRuneCircleClicked(RuneNode* node, int idx)
{
    if (_data) {
        if (_selectedRune) {
            _selectedRune->select(false);
        }
        
        node->select(true);
        _selectedRune = node;
    }
}

#pragma mark - RuneBagLayerObserver
void CardInfoLayer::onRuneBagLayerSelected(Node* pSender, RuneNode* node)
{
    if (pSender) {
        pSender->removeFromParent();
    }
    
    auto data(node->getData());
    DeckManager::getInstance()->imbedRune(_data->getId(), _selectedRune->getIdx(), data->getDbId(), [=]() {
        _selectedRune->update(data);
    });
}

#pragma mark - BaseCardObserver
void CardInfoLayer::onBaseCardInfo(BaseCard* pSender)
{
    auto layer = SpellInfoLayer::create(pSender->getCardId(), pSender->getCardData());
    layer->registerObserver(this);
    addChild(layer);
}

void CardInfoLayer::onBaseCardClickedResourceButton(BaseCard* pSender)
{
    
}

#pragma mark - SpellInfoLayerObserver
void CardInfoLayer::onSpellInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void CardInfoLayer::onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data)
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
        PureScale9Sprite::Type type(PureScale9Sprite::Type::Purple);
        auto bar = PureScale9Sprite::create(type);
        bar->setContentSize(barSize);
        bar->setPosition(size.width / 2, size.height - (secondaryEdge.y + barSize.height / 2));
        node->addChild(bar);
        _rarityBar = bar;
        
        static const float edgeX(5);
        auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        label->setPosition(edgeX, barSize.height / 2);
        bar->addChild(label);
        _level = label;
        
        label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        label->setPosition(barSize.width - edgeX, barSize.height / 2);
        bar->addChild(label);
        _profession = label;
    }
    
    static const Vec2 spaceBarCard(12, 15);
    auto card = JuniorCard::create();
    const auto& cardSize(card->getContentSize());
    card->setPosition(Point(secondaryEdge.x + spaceBarCard.x + cardSize.width / 2, size.height - (secondaryEdge.y + barSize.height + spaceBarCard.y + cardSize.height / 2)));
    node->addChild(card);
    _icon = card;
    
    // description
    {
        static const float spaceBarDesc(5);
        static const float spaceCardDesc(20);
        const Size descSize(barSize.width - (cardSize.width + spaceBarCard.x + spaceCardDesc), spaceBarCard.y + cardSize.height - spaceBarDesc);
        auto descBg = PureScale9Sprite::create(PureScale9Sprite::Type::White);
        descBg->setContentSize(descSize);
        descBg->setPosition(size.width - (secondaryEdge.x + descSize.width / 2), size.height - (secondaryEdge.y + barSize.height + spaceBarDesc + descSize.height / 2));
        node->addChild(descBg);
        
        static const float edge(5);
        auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
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
    
    static const float spaceLineAttribute(20);
    
    // attributes
    {
        static const int columnCount(2);
        static const float spaceY(6);
        float spaceX(0);
        Size attributeSize(Size::ZERO);
        for (int i = 0; i < 10; ++i) {
            const int row(i / columnCount);
            const int column(i % columnCount);
            Color4B color;
            if (row % 2 == 0) {
                color = PURE_GRAY;
            } else {
                color = PURE_WHITE;
            }
            
            auto attribute = CardAttributeNode::create(color);
            node->addChild(attribute);
            _attributes.push_back(attribute);
            
            // calculate space first
            if (0 == i) {
                attributeSize = attribute->getContentSize();
                if (columnCount > 1) {
                    spaceX = (size.width - (secondaryEdge.x * 2 + attributeSize.width * columnCount)) / (columnCount - 1);
                }
            }
            attribute->setPosition(secondaryEdge.x + (attributeSize.width + spaceX) * column + attributeSize.width / 2, line->getPositionY() - (spaceLineAttribute + (attributeSize.height + spaceY) * row + attributeSize.height / 2));
        }
    }
    
    // bottom bar
    if (/* DISABLES CODE */ (false)) {
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
        
        auto button = ResourceButton::create(true, false, ResourceType::Gold, 0, Color4B::BLACK);
        button->addClickEventListener([this](Ref* pSender) {
            auto button(dynamic_cast<ResourceButton*>(pSender));
            if (button) {
                if (button->isResourceEnough()) {
                    DeckManager::getInstance()->upgradeCard(_data->getId(), [this](const CardData* data) {
                        updateData(data);
                    });
                } else {
                    MessageBox("资源不足", nullptr);
                }
            }
        });
        button->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        button->setPosition(Point(barSize.width / 2, 0));
        bar->addChild(button);
        _resourceButton = button;
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
        auto card = SeniorCard::create();
        card->registerObserver(this);
        node->addChild(card);
        _skillCards.push_back(card);
        
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
        _runeCircle = circle;
        
        static const Vec2 circleEdge(40, 18);
        const auto& csize(circle->getContentSize());
        circle->setPosition(circleEdge.x + csize.width / 2, circleEdge.y + csize.height / 2);
        
        static const float spaceY(10);
        for (int i = 0; i < CARD_RUNES_COUNT; ++i) {
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
            
            do {
                auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                label->setTextColor(Color4B::BLACK);
                label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                label->setPosition(10, bgSize.height / 2);
                bg->addChild(label);
                _runeProperties.push_back(label);
            } while (false);
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
        auto button = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        button->setTitleText(opButtonsTitles[i]);
        button->addClickEventListener([this, i](Ref*) { onOpButtonClicked(i); });
        node->addChild(button);
        const auto& bsize(button->getContentSize());
        button->setPosition(Point(size.width - ((edge.x + bsize.width) * (i + 1) - bsize.width / 2), edge.y + bsize.height / 2));
        _opButtons.push_back(button);
    }
}

void CardInfoLayer::onOpButtonClicked(int idx)
{
    if (!_selectedRune) {
        MessageBox("请选择一个符文", nullptr);
    } else if (idx < opButtonsCount) {
        if (0 == idx) {
            DeckManager::getInstance()->unloadRune(_data->getId(), _selectedRune->getIdx(), [this]() {
                _selectedRune->update(nullptr);
            });
        } else if (1 == idx) {
            DeckManager::getInstance()->fetchRunesList([this]() {
                auto property(dynamic_cast<const CardProperty*>(_property));
                auto layer = RuneBagLayer::create(property->getRuneType(_selectedRune->getIdx()), _selectedRune->getData());
                layer->registerObserver(this);
                addChild(layer);
            });
        }
    }
}

void CardInfoLayer::initUI()
{
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
    
    auto button = XButton::createReturnButton(board, Vec2(8.0f, 10.0f));
    button->addClickEventListener([this](Ref*) {
        if (_observer) {
            _observer->onCardInfoLayerReturn(this);
        }
    });
    createLeftNode(board->getSubNode(0));
    createRightNode(board->getSubNode(1));
}

void CardInfoLayer::updateProperty(const DevelopProperty* property)
{
    AbstractInfoLayer::updateProperty(property);
    
    if (_profession) {
        _profession->setString(property ? "" : "");
    }
    
    auto cp(dynamic_cast<const CardProperty*>(property));
    if (cp) {
        if (_rarityBar) {
            auto rarity(cp->getRarity());
            auto type(PureScale9Sprite::Type::Blue);
            if (1 == rarity) {
                type = PureScale9Sprite::Type::Orange;
            } else if (2 == rarity) {
                type = PureScale9Sprite::Type::Purple;
            }
            _rarityBar->setType(type);
        }
        
        do {
            const auto& skills(cp->getSkills());
            int cnt((int)skills.size());
            for (int i = 0; i < cnt; ++i) {
                if (i < _skillCards.size()) {
                    auto card(_skillCards.at(i));
                    card->setVisible(true);
                    card->update(skills.at(i), nullptr);
                }
            }
            
            for (int i = cnt; i < _skillCards.size(); ++i) {
                _skillCards.at(i)->setVisible(false);
            }
        } while (false);
    }
}

void CardInfoLayer::updateData(const AbstractData* data)
{
    AbstractInfoLayer::updateData(data);
    
    // always update these params
    if (_level) {
        _level->setString(data ? StringUtils::format("LV.%d", data->getLevel()) : "");
    }
    
    auto cardData = dynamic_cast<const CardData*>(data);
    
    const bool show(nullptr != data);
    if (show) {
        const auto& skills(cardData->getSkills());
        int cnt((int)skills.size());
        for (int i = 0; i < cnt; ++i) {
            if (i < _skillCards.size()) {
                auto skill(skills.at(i));
                _skillCards.at(i)->update(skill->getId(), skill);
            }
        }
    }
    
    if (_runeCircle) {
        for (int i = 0; i < CARD_RUNES_COUNT; ++i) {
            _runeCircle->setData(i, show ? cardData->getRune(i) : nullptr);
        }
    }
    
    for (auto button : _opButtons) {
        button->setEnabled(show);
    }
}
