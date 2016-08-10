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
#include "SoundManager.h"
#include "ResourceNode.h"
#include "UserSimpleNode.h"
#include "FormationLayer.h"
#include "BattleDeckLayer.h"
#include "CardLayer.h"
#include "SettingsLayer.h"
#include "PvpLogLayer.h"
#include "CheatLayer.h"

using namespace std;
using namespace ui;

static const string getResourcePath(const string& file) {
    return "GameImages/main_ui/" + file;
}

enum class MainUILayer::ButtonType {
    Rank,
    Achievement,
    Settings,
    Chat,
    Friend,
    BattleLog,
    Guild,
    Battle,
    Train,
    Quest,
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
                {ButtonType::Friend,        {"ui_kuang_3", "icon_haoyou"}},
                {ButtonType::BattleLog,     {"ui_kuang_3", "icon_zhanji"}},
                {ButtonType::Guild,         {"ui_kuang_3", "icon_lianmeng"}},
                {ButtonType::Battle,        {"ui_kuang_2", "icon_pvp"}},
                {ButtonType::Train,         {"ui_kuang_3", "icon_zhujue"}},
                {ButtonType::Quest,         {"ui_kuang_3", "icon_renwu"}},
                {ButtonType::Shop,          {"ui_kuang_2", "icon_shangdian"}},
            };
            
            if (ButtonInfos.find(type) != end(ButtonInfos)) {
                const auto& pair = ButtonInfos.at(type);
                const auto file = getResourcePath(StringUtils::format("%s.png", pair.first.c_str()));
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
                    const auto iconFile = getResourcePath(StringUtils::format("%s.png", pair.second.c_str()));
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
            auto button = FunctionButton::create(ButtonType::Shop, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
            addChild(button);
            
            const auto& size(button->getContentSize());
            button->setPosition(Point(margin + size.width / 2, margin + size.height / 2));
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
            static const Vec2 space(10, 10);
            float x(winSize.width - margin);
            float y(margin);
            const Point basePoint(x, y);
            
            {
                auto button = FunctionButton::create(ButtonType::Battle, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(x - size.width / 2, y + size.height / 2));
                
                x -= size.width + space.x;
                y += size.height + space.y;
            }
            
            // left
            {
                auto button = FunctionButton::create(ButtonType::Train, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(x - size.width / 2, basePoint.y + size.height / 2));
                
                x -= size.width + space.x;
            }
            
            {
                auto button = FunctionButton::create(ButtonType::Quest, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(x - size.width / 2, basePoint.y + size.height / 2));
                
                x -= size.width + space.x;
            }
            
            {
                auto button = FunctionButton::create(ButtonType::Guild, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(x - size.width / 2, basePoint.y + size.height / 2));
                
                x -= size.width + space.x;
            }
            
            {
                auto button = FunctionButton::create(ButtonType::Friend, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(x - size.width / 2, basePoint.y + size.height / 2));
                
                x -= size.width + space.x;
            }
            
            {
                auto button = FunctionButton::create(ButtonType::BattleLog, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(x - size.width / 2, basePoint.y + size.height / 2));
                
                x -= size.width + space.x;
            }
            
            // top
            {
                auto button = FunctionButton::create(ButtonType::Settings, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(basePoint.x - size.width / 2, y + size.height / 2));
                
                y += size.height + space.y;
            }
            
            {
                auto button = FunctionButton::create(ButtonType::Rank, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(basePoint.x - size.width / 2, y + size.height / 2));
                
                y += size.height + space.y;
            }
            
            {
                auto button = FunctionButton::create(ButtonType::Achievement, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
                addChild(button);
                
                const auto& size(button->getContentSize());
                button->setPosition(Point(basePoint.x - size.width / 2, y + size.height / 2));
                
                y += size.height + space.y;
            }
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

void MainUILayer::onEnterTransitionDidFinish()
{
    LayerColor::onEnterTransitionDidFinish();
    CheatLayer::getInstance()->show();
}

bool MainUILayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return false;
}

void MainUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - ChatLayerObserver
void MainUILayer::onChatLayerClickedButton()
{
    moveChatLayer(!_isChatLayerFolded, true);
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

void MainUILayer::moveChatLayer(bool folded, bool animated)
{
    if (!_chatLayer) {
        _chatLayer = ChatLayer::create();
        _chatLayer->registerObserver(this);
        addChild(_chatLayer, zorder_top);
    }
    
    const auto& size(_chatLayer->getContentSize());
    const Point point(folded ? -size.width : 0, 0);
    if (point != _chatLayer->getPosition()) {
        if (animated) {
            if (!_isChatLayerMoving) {
                _isChatLayerMoving = true;
                static const float duration(0.3f);
                if (folded) {
                    _chatLayer->setFocus(false);
                }
                _chatLayer->runAction(Sequence::create(MoveTo::create(duration, point), CallFunc::create([=]() {
                    onChatLayerMoved(folded, point);
                }), nullptr));
            }
        } else {
            onChatLayerMoved(folded, point);
        }
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
            GameManager::getInstance()->launchPvp();
            break;
            
        case ButtonType::Train:
            runningScene->addChild(FormationLayer::create());
            break;
            
        case ButtonType::Settings:
            runningScene->addChild(SettingsLayer::create());
            break;
        
        case ButtonType::Chat:
            runningScene->addChild(ChatLayer::create());
            break;
            
        case ButtonType::BattleLog:
            runningScene->addChild(PvpLogLayer::create());
            break;
            
        case ButtonType::Shop:
            runningScene->addChild(CardLayer::create());
            break;
            
        case ButtonType::Quest:
            runningScene->addChild(BattleDeckLayer::create());
            break;
            
        default:
            break;
    }
}