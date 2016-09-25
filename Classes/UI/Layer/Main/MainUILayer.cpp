//
//  MainUILayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MainUILayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "GameManager.h"
#include "PvpManager.h"
#include "BillboardManager.h"
#include "ShopManager.h"
#include "SoundManager.h"
#include "ResourceNode.h"
#include "UserSimpleNode.h"
#include "DevelopLayer.h"
#include "BattleDeckLayer.h"
#include "SettingsLayer.h"
#include "PvpLogLayer.h"
#include "GuildLayer.h"
#include "GuildSearchLayer.h"
#include "LoadingLayer.h"
#include "BillboardLayer.h"
#include "BattleResultLayer.h"
#include "CardShopLayer.h"
#include "TestLayer.h"

using namespace std;
using namespace ui;

enum class MainUILayer::ButtonType {
    Rank,
    Achievement,
    Settings,
    Chat,
    Quest,
    BattleLog,
    Guild,
    Battle,
    Train,
    Prepare,
    Shop,
};

#pragma mark - FunctionButton
class MainUILayer::FunctionButton : public Node {
public:
    typedef function<void(ButtonType)> Callback;
    static FunctionButton* create(ButtonType type, const Callback& callback) {
        auto ret = new (nothrow) FunctionButton();
        if (ret && ret->init(type, callback)) {
            ret->autorelease();
            return ret;
        }
        
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    
    virtual ~FunctionButton() { removeAllChildren(); }
    
private:
    FunctionButton():_callback(nullptr) {}
    bool init(ButtonType type, const Callback& callback) {
        if (Node::init()) {
            _type = type;
            _callback = callback;
            
            static const map<ButtonType, pair<string, string>> ButtonInfos = {
                {ButtonType::Rank,          {"ui_kuang_1", "icon_jiangbei"}},
                {ButtonType::Achievement,   {"ui_kuang_1", "icon_xingxing"}},
                {ButtonType::Settings,      {"ui_kuang_1", "icon_shezhi"}},
                {ButtonType::Chat,          {"ui_liaotian", ""}},
                {ButtonType::Quest,         {"ui_kuang_3", "icon_haoyou"}},
                {ButtonType::BattleLog,     {"ui_kuang_3", "icon_zhanji"}},
                {ButtonType::Guild,         {"ui_kuang_3", "icon_lianmeng"}},
                {ButtonType::Battle,        {"ui_kuang_2", "icon_pvp"}},
                {ButtonType::Train,         {"ui_kuang_3", "icon_zhujue"}},
                {ButtonType::Prepare,       {"ui_kuang_3", "icon_renwu"}},
                {ButtonType::Shop,          {"ui_kuang_2", "icon_shangdian"}},
            };
            
            if (ButtonInfos.find(type) != end(ButtonInfos)) {
                const auto& pair = ButtonInfos.at(type);
                const auto file = CocosUtils::getResourcePath(StringUtils::format("%s.png", pair.first.c_str()));
                auto button = Button::create(file, file);
                addChild(button);
                button->setPressedActionEnabled(true);
                button->addClickEventListener([this](Ref*) {
                    if (_callback) {
                        _callback(_type);
                    }
                });
                
                const auto& size(button->getContentSize());
                if (!pair.second.empty()) {
                    const auto iconFile = CocosUtils::getResourcePath(StringUtils::format("%s.png", pair.second.c_str()));
                    auto icon = Sprite::create(iconFile);
                    button->addChild(icon);
                    icon->setPosition(Point(size.width / 2, size.height / 2));
                }
                
                setAnchorPoint(Point::ANCHOR_MIDDLE);
                setContentSize(size);
                button->setPosition(Point(size.width / 2, size.height / 2));
            }
            
            return true;
        }
        
        return false;
    }
    
private:
    ButtonType _type;
    Callback _callback;
};

#pragma mark - MainUILayer
static const int zorder_top(1);

MainUILayer* MainUILayer::create()
{
    auto ret = new (nothrow) MainUILayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MainUILayer::MainUILayer()
:_observer(nullptr)
,_isChatLayerFolded(true)
,_isChatLayerMoving(false)
,_chatLayer(nullptr)
,_userNode(nullptr)
,_staminaResourceNode(nullptr)
,_goldResourceNode(nullptr)
,_gemResourceNode(nullptr)
{
    
}

MainUILayer::~MainUILayer()
{
    removeAllChildren();
}

void MainUILayer::registerObserver(MainUILayerObserver *observer)
{
    _observer = observer;
}

bool MainUILayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const auto& winSize = Director::getInstance()->getWinSize();
        
        static const float margin(10.0f);
        
        // 1. left top
        _userNode = UserSimpleNode::create();
        _userNode->setIsHome(true);
        _userNode->setIsMe(true);
        addChild(_userNode);
        const auto& mainSize(_userNode->getContentSize());
        _userNode->setPosition(Point(mainSize.width / 2 + margin, winSize.height - (mainSize.height / 2 + margin)));
        
        // 2. left
        {
            moveChatLayer(_isChatLayerFolded, false);
        }
        
        // 3. left bottom
        {
            auto button = createButton(ButtonType::Shop, Point::ZERO);
            const auto& size(button->getContentSize());
            button->setPosition(margin + size.width / 2, margin + size.height / 2);
        }
        
        // 4. right top
        {
            _gemResourceNode = ResourceNode::create(::ResourceType::Gem, 100, CC_CALLBACK_1(MainUILayer::onResourceButtonClicked, this));
            static float offsetX(20);
            static float offsetY(20);
            const auto& gemSize(_gemResourceNode->getContentSize());
            const float x = winSize.width - offsetX - gemSize.width / 2;
            const float y = winSize.height - offsetY - gemSize.height / 2;
            
            _gemResourceNode->setPosition(Point(x, y));
            addChild(_gemResourceNode);
            
            _goldResourceNode = ResourceNode::create(::ResourceType::Gold, 100, CC_CALLBACK_1(MainUILayer::onResourceButtonClicked, this));
            const auto& goldSize(_goldResourceNode->getContentSize());
            _goldResourceNode->setPosition(Point(x - (offsetX + (gemSize.width + goldSize.width) / 2), y));
            addChild(_goldResourceNode);
            
            _staminaResourceNode = ResourceNode::create(::ResourceType::Stamina, 100, CC_CALLBACK_1(MainUILayer::onResourceButtonClicked, this));
            const auto& jadeSize(_staminaResourceNode->getContentSize());
            _staminaResourceNode->setPosition(Point(x - (offsetX * 2 + (gemSize.width + jadeSize.width) / 2 + goldSize.width), y));
            addChild(_staminaResourceNode);
        }
        
        // 5. right bottom
        {
            static const float space(10.0f);
            const Point original(winSize.width - margin, margin);
            
            auto button = createButton(ButtonType::Battle, original);
            
            // leftward
            float x(original.x - (button->getContentSize().width + space));
            createButtonLeftward(ButtonType::Train, x, original.y, space);
            createButtonLeftward(ButtonType::Prepare, x, original.y, space);
            createButtonLeftward(ButtonType::Guild, x, original.y, space);
            createButtonLeftward(ButtonType::Quest, x, original.y, space);
            createButtonLeftward(ButtonType::BattleLog, x, original.y, space);
            
            // upward
            float y(original.y + (button->getContentSize().height + space));
            createButtonUpward(ButtonType::Settings, original.x, y, space);
            createButtonUpward(ButtonType::Rank, original.x, y, space);
            createButtonUpward(ButtonType::Achievement, original.x, y, space);
        }
        
        updateAvatar(0);
        updateResources();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MainUILayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MainUILayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void MainUILayer::onEnter()
{
    LayerColor::onEnter();
//    TestLayer::getInstance()->show(this);
}

bool MainUILayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return false;
}

void MainUILayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - ChatLayerObserver
void MainUILayer::onChatLayerTouchedButton(Button* button, Widget::TouchEventType type)
{
    if (Widget::TouchEventType::MOVED == type) {
        const float x(button->getTouchMovePosition().x - button->getTouchBeganPosition().x);
        const auto& basePoint(getChatLayerDefaultPosition(_isChatLayerFolded));
        if (_isChatLayerFolded != (x < 0)) {
            const float offsetX(MAX(MIN(abs(x), _chatLayer->getContentSize().width), 0));
            _chatLayer->setPosition(basePoint + Point((x > 0) ? offsetX : -offsetX, 0));
        } else {
            _chatLayer->setPosition(basePoint);
        }
    } else if (Widget::TouchEventType::ENDED == type ||
               Widget::TouchEventType::CANCELED == type) {
        const auto& endPoint(button->getTouchEndPosition());
        const float x(endPoint.x - button->getTouchBeganPosition().x);
        if (0 == x) {
            moveChatLayer(!_isChatLayerFolded, true);
        } else if (_isChatLayerFolded != (x < 0)) {
            const float width(_chatLayer->getContentSize().width);
            const float offsetX(MAX(MIN(abs(x), width), 0));
            const bool changed(_isChatLayerFolded != (offsetX > width / 2));
            moveChatLayer(changed, true);
        } else {
            moveChatLayer(_isChatLayerFolded, true);
        }
    }
}

#pragma mark - private
void MainUILayer::updateAvatar(int idx)
{
    if (_userNode) {
        _userNode->setAvatar(idx);
    }
}

void MainUILayer::updateExp()
{
    
}

void MainUILayer::updateResources()
{
    
}

Point MainUILayer::getChatLayerDefaultPosition(bool folded) const
{
    if (folded && _chatLayer) {
        return Point(-_chatLayer->getContentSize().width, 0);
    }
    
    return Point::ZERO;
}

void MainUILayer::moveChatLayer(bool folded, bool animated)
{
    if (!_chatLayer) {
        _chatLayer = ChatLayer::create();
        _chatLayer->registerObserver(this);
        addChild(_chatLayer, zorder_top);
    }
    
    const auto& currentPoint(_chatLayer->getPosition());
    const auto& point(getChatLayerDefaultPosition(folded));
    if (point != currentPoint && animated) {
        if (!_isChatLayerMoving) {
            _isChatLayerMoving = true;
            static const float speed(2400.f);
            if (folded) {
                _chatLayer->setFocus(false);
            }
            
            _chatLayer->runAction(Sequence::create(MoveTo::create(currentPoint.distance(point) / speed, point), CallFunc::create([=]() {
                onChatLayerMoved(folded, point);
            }), nullptr));
        }
    } else {
        onChatLayerMoved(folded, point);
    }
}

void MainUILayer::onChatLayerMoved(bool folded, const Point& point)
{
    _chatLayer->setPosition(point);
    _isChatLayerFolded = folded;
    _isChatLayerMoving = false;
    _chatLayer->setButtonStatus(folded);
    _chatLayer->setFocus(!folded);
}

MainUILayer::FunctionButton* MainUILayer::createButton(ButtonType type, const Point& point)
{
    auto button = FunctionButton::create(type, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
    addChild(button);
    const auto& size(button->getContentSize());
    button->setPosition(point.x - size.width / 2, point.y + size.height / 2);
    return button;
}

void MainUILayer::createButtonLeftward(ButtonType type, float& x, float y, float space)
{
    auto button = createButton(type, Point(x, y));
    x -= button->getContentSize().width + space;
}

void MainUILayer::createButtonUpward(ButtonType type, float x, float& y, float space)
{
    auto button = createButton(type, Point(x, y));
    y += button->getContentSize().height + space;
}

void MainUILayer::onResourceButtonClicked(ResourceNode* node)
{
    switch (node->getType()) {
        case ::ResourceType::Gem: {
            
        }
            break;
        case ::ResourceType::Gold: {
            
        }
            break;
        case ::ResourceType::Stamina: {
            
        }
            break;
        default:
            break;
    }
}

void MainUILayer::onFunctionButtonClicked(ButtonType type)
{
    SoundManager::getInstance()->playButtonSound();
    
    auto runningScene(Director::getInstance()->getRunningScene());
    switch (type) {
        case ButtonType::Battle:
            LoadingLayer::show(true, [](Node*) {
                GameManager::getGameClient()->launchPvp();
            });
            break;
            
        case ButtonType::Train:
            DeckManager::getInstance()->fetchCardList([runningScene]() {
                runningScene->addChild(DevelopLayer::create());
            });
            break;
            
        case ButtonType::Prepare:
            DeckManager::getInstance()->fetchCardList([runningScene]() {
                runningScene->addChild(BattleDeckLayer::create());
            });
            break;
            
        case ButtonType::Settings:
            runningScene->addChild(SettingsLayer::create());
            break;
            
        case ButtonType::BattleLog:
            PvpManager::getInstance()->fetchPvpLogs([runningScene] {
                runningScene->addChild(PvpLogLayer::create());
            });
            break;
            
        case ButtonType::Guild: {
            if (/* DISABLES CODE */ (true)) {
                runningScene->addChild(GuildSearchLayer::create());
            } else {
                runningScene->addChild(GuildLayer::create());
            }
        }
            break;
            
        case ButtonType::Rank: {
            BillboardManager::getInstance()->fetchBillboard([runningScene]() {
                runningScene->addChild(BillboardLayer::create());
            });
        }
            break;
            
        case ButtonType::Shop: {
            ShopManager::getInstance()->fetchCardList([runningScene] {
                runningScene->addChild(CardShopLayer::create());
            });
        }
            break;
            
        case ButtonType::Quest: {
            runningScene->addChild(BattleResultLayer::create());
        }
            break;
            
        default:
            break;
    }
}
