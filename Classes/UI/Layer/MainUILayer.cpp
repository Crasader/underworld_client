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
#include "FormationLayer.h"
#include "BattleDeckLayer.h"
#include "CardLayer.h"

using namespace std;
using namespace ui;

static const string getResourcePath(const string& file) {
    return "GameImages/main_ui/" + file;
}

enum class MainUILayer::ButtonType {
    Rank,
    Achievement,
    Option,
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
                {ButtonType::Option,        {"ui_kuang_1", "icon_shezhi"}},
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
,_iconButton(nullptr)
,_nameLabel(nullptr)
,_levelLabel(nullptr)
,_expProgress(nullptr)
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
        {
            auto background = Sprite::create(getResourcePath("ui_banzi_1.png"));
            addChild(background);
            const auto& mainSize(background->getContentSize());
            background->setPosition(Point(mainSize.width / 2 + margin, winSize.height - (mainSize.height / 2 + margin)));
            
            // avatar
            {
                static const auto avatarFile(getResourcePath("icon_touxiang_1.png"));
                auto avatar = Button::create(avatarFile, avatarFile);
                avatar->addClickEventListener([this](Ref *pSender){
                    SoundManager::getInstance()->playButtonSound();
                    // TODO:
                });
                background->addChild(avatar);
                _iconButton = avatar;
                
                static const Vec2 edge(10.0f, 10.0f);
                const auto& size(avatar->getContentSize());
                avatar->setPosition(Point(size.width / 2 + edge.x, size.height / 2 + edge.y));
            }
            
            {
                auto name = CocosUtils::createLabel("User Name", BIG_FONT_SIZE);
                name->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                background->addChild(name);
                _nameLabel = name;
                static const Vec2 edge(5.0f, 5.0f);
                name->setPosition(Point(_iconButton->getPosition().x + _iconButton->getContentSize().width / 2 + edge.x, mainSize.height - (name->getContentSize().height / 2 + edge.y)));
            }
            
            // trophy
            {
                auto bg = Sprite::create(getResourcePath("ui_tiao_2.png"));
                background->addChild(bg);
                
                auto icon = Sprite::create(getResourcePath("icon_jiangbei.png"));
                icon->setScale(0.5f);
                bg->addChild(icon);
                
                auto trophyCount = CocosUtils::createLabel("123", SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
                trophyCount->setAnchorPoint(Point::ANCHOR_MIDDLE);
                bg->addChild(trophyCount);
                
                const auto& bsize(bg->getContentSize());
                const auto& isize(icon->getContentSize());
                icon->setPosition(Point(0, bsize.height / 2));
                trophyCount->setPosition(Point(bsize.width / 2 + isize.width / 4, bsize.height / 2));
                static const Vec2 edge(8.0f, 5.0f);
                bg->setPosition(Point(mainSize.width - (edge.x + bsize.width / 2), mainSize.height - (edge.y + bsize.height / 2)));
            }
            
            // exp
            {
                auto bg = Sprite::create(getResourcePath("ui_tiao_1.png"));
                background->addChild(bg);
                
                auto s = Sprite::create(getResourcePath("ui_tiao_1_1.png"));
                auto pt = ProgressTimer::create(s);
                pt->setType(ProgressTimer::Type::BAR);
                pt->setBarChangeRate(Vec2(1.0f, 0.0f));
                pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
                pt->setPercentage(80);
                bg->addChild(pt);
                _expProgress = pt;
                
                auto icon = Sprite::create("GameImages/resources/icon_104B.png");
                bg->addChild(icon);
                
                auto level = CocosUtils::createLabel("123", SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
                icon->addChild(level);
                _levelLabel = level;
                
                const auto& bsize(bg->getContentSize());
                const auto& isize(icon->getContentSize());
                icon->setPosition(Point(0, bsize.height / 2));
                pt->setPosition(Point(bsize.width / 2, bsize.height / 2));
                level->setPosition(Point(isize.width / 2, isize.height / 2));
                static const Vec2 edge(8.0f, 10.0f);
                bg->setPosition(Point(mainSize.width - (bsize.width / 2 + edge.x), bsize.height / 2 + edge.y));
            }
        }
        
        // 2. left
        {
            auto button = FunctionButton::create(ButtonType::Chat, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
            addChild(button);
            
            const auto& size(button->getContentSize());
            button->setPosition(Point(size.width / 2, winSize.height / 2));
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
                auto button = FunctionButton::create(ButtonType::Option, CC_CALLBACK_1(MainUILayer::onFunctionButtonClicked, this));
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
        
        updateIcon();
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

bool MainUILayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return false;
}

void MainUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - private
void MainUILayer::updateIcon()
{
    const string file(getResourcePath("icon_touxiang_1.png"));
    _iconButton->loadTextures(file, file);
}

void MainUILayer::updateExp()
{
    
}

void MainUILayer::updateResources()
{
    
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
    
    switch (type) {
        case ButtonType::Battle:
            GameManager::getInstance()->launchPvp();
            break;
            
        case ButtonType::Train:
            Director::getInstance()->getRunningScene()->addChild(FormationLayer::create());
            break;
            
        default:
            break;
    }
}