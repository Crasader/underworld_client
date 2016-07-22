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

static const int buttonIconTag(1000);
static const string getResourcePath(const string& file) {
    return "GameImages/main_ui/" + file;
}

enum class MainUILayer::ButtonType {
    Rank,
    Achievement,
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
    typedef function<void()> Callback;
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
            _callback = callback;
            
            static const map<ButtonType, pair<string, string>> ButtonInfos = {
                {ButtonType::Rank,          {"ui_kuang_1", "icon_jiangbei"}},
                {ButtonType::Achievement,   {"ui_kuang_1", "icon_xingxing"}},
                {ButtonType::Chat,          {"ui_kuang_1", "icon_xingxing"}},
                {ButtonType::Friend,        {"ui_kuang_3", "icon_haoyou"}},
                {ButtonType::BattleLog,     {"ui_kuang_3", "icon_zhanji"}},
                {ButtonType::Guild,         {"ui_kuang_3", "icon_lianmeng"}},
                {ButtonType::Battle,        {"ui_kuang_2", "icon_pvp"}},
                {ButtonType::Train,         {"ui_kuang_3", "icon_zhujue"}},
                {ButtonType::Quest,         {"ui_kuang_3", ""}},
                {ButtonType::Shop,          {"ui_kuang_2", ""}},
            };
            
            if (ButtonInfos.find(type) != end(ButtonInfos)) {
                const auto& pair = ButtonInfos.at(type);
                const auto file = getResourcePath(StringUtils::format("%s.png", pair.first.c_str()));
                auto button = Button::create(file, file);
                addChild(button);
                button->setPressedActionEnabled(true);
                button->addClickEventListener([this](Ref*) {
                    if (_callback) {
                        _callback();
                    }
                });
                
                const auto iconFile = getResourcePath(StringUtils::format("%s.png", pair.second.c_str()));
                auto icon = Sprite::create(iconFile);
                button->addChild(icon);
                const auto& size(button->getContentSize());
                icon->setPosition(Point(size.width / 2, size.height / 2));
            }
            
            return true;
        }
        
        return false;
    }
    
private:
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
,_expProgressBar(nullptr)
,_pvpButton(nullptr)
,_bagButton(nullptr)
,_questButton(nullptr)
,_optionButton(nullptr)
,_guildButton(nullptr)
,_armyButton(nullptr)
,_jadeResourceNode(nullptr)
,_goldResourceNode(nullptr)
,_gemResourceNode(nullptr)
{
    
}

MainUILayer::~MainUILayer()
{
    removeButtonIcons();
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
#if false
            auto background = Sprite::create(getResourcePath("ui_banzi_1.png"));
            addChild(background);
            background->setPosition(Point(margin, winSize.height - margin));
            const auto& backgroundSize(background->getContentSize());
            
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
                
                static const Vec2 edge(5.0f, 5.0f);
                const auto& size(avatar->getContentSize());
                avatar->setPosition(Point(size.width + edge.x, size.height + edge.y));
            }
            
            {
                auto name = CocosUtils::createLabel("User Name", BIG_FONT_SIZE);
                name->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                background->addChild(name);
                _nameLabel = name;
                static const Vec2 edge(5.0f, 5.0f);
                name->setPosition(Point(_iconButton->getPosition().x + _iconButton->getContentSize().width / 2 + edge.x, backgroundSize.height - (name->getContentSize().height / 2 + edge.y)));
            }
            
            // trophy
            {
                auto bg = Sprite::create(getResourcePath("ui_tiao_2.png"));
                background->addChild(bg);
                
                auto icon = Sprite::create(getResourcePath("icon_jiangbei.png"));
                bg->addChild(icon);
                
                auto trophyCount = CocosUtils::createLabel("123", SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
                trophyCount->setAnchorPoint(Point::ANCHOR_MIDDLE);
                bg->addChild(trophyCount);
                
                const auto& bsize(bg->getContentSize());
                const auto& isize(icon->getContentSize());
                icon->setPosition(Point(isize.width / 2, bsize.height / 2));
                trophyCount->setPosition(Point(bsize.width / 2 + isize.width / 4, bsize.height / 2));
            }
            
            // exp
            {
                auto bg = Sprite::create(getResourcePath("ui_tiao_1.png"));
                background->addChild(bg);
                
                auto bar = LoadingBar::create(getResourcePath("ui_tiao_1_1.png"));
                bar->setScale9Enabled(true);
                bar->setCapInsets(Rect(1, 1, 176, 2));
                bg->addChild(bar);
                
                auto icon = Sprite::create("GameImages/resources/icon_104B.png");
                bg->addChild(icon);
                
                auto level = CocosUtils::createLabel("123", SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
                icon->addChild(level);
                
                const auto& bsize(bg->getContentSize());
                const auto& isize(icon->getContentSize());
                icon->setPosition(Point(isize.width / 2, bsize.height / 2));
                bar->setPosition(Point(bsize.width / 2, bsize.height / 2));
                level->setPosition(Point(isize.width / 2, isize.height / 2));
            }
#endif
            
            static const string csbFile("UI_CharInfo.csb");
            Node *mainNode = CocosUtils::playAnimation(csbFile, 0, false);
            mainNode->setPosition(Point(margin, winSize.height - margin));
            addChild(mainNode);
            
            Node* root = mainNode->getChildByTag(7);
            const auto& children = root->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                Node* child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 15:
                            {
                                auto button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                                _iconButton = button;
                            }
                                break;
                            case 8:
                            {
                                Label* label = CocosUtils::createLabel("User Name", BIG_FONT_SIZE);
                                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                                child->addChild(label);
                                _nameLabel = label;
                            }
                                break;
                            case 11:
                            {
                                Label* label = CocosUtils::createLabel("LV.10", DEFAULT_FONT_SIZE);
                                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                                child->addChild(label);
                                _levelLabel = label;
                            }
                                break;
                            case 10:
                            {
                                LoadingBar* bar = dynamic_cast<LoadingBar*>(child);
                                bar->setScale9Enabled(true);
                                bar->setCapInsets(Rect(1, 1, 176, 2));
                                _expProgressBar = bar;
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        // 2. left
        {
            static const string csbFile("UI_ChatIcon.csb");
            auto mainNode = CocosUtils::playAnimation(csbFile, 0, false);
            mainNode->setPosition(Point(0, winSize.height / 2));
            addChild(mainNode);
            
            auto root = mainNode;
            const auto& children = root->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                auto child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 20:
                            {
                                auto button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->setPressedActionEnabled(true);
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        // 3. left bottom
        {
            static const float margin(5.0f);
            static const string csbFile("UI_PVPICON.csb");
            auto mainNode = CocosUtils::playAnimation(csbFile, 0, false);
            mainNode->setPosition(Point(margin, margin));
            addChild(mainNode);
            
            auto root = mainNode->getChildByTag(23);
            const auto& children = root->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                auto child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 24:
                            {
                                auto button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->loadTextureDisabled("GameImages/public/button_4.png");
                                    setButtonIcons(button, 25, "icon_pvp_1", "icon_pvp_2");
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        GameManager::getInstance()->launchPvp();
                                    });
                                }
                                
                                _pvpButton = button;
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
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
            
            _jadeResourceNode = ResourceNode::create(::ResourceType::Stamina, 100, CC_CALLBACK_1(MainUILayer::onResourceButtonClicked, this));
            const auto& jadeSize(_jadeResourceNode->getContentSize());
            _jadeResourceNode->setPosition(Point(x - (offsetX * 2 + (gemSize.width + jadeSize.width) / 2 + goldSize.width), y));
            addChild(_jadeResourceNode);
        }
        
        // 5. right bottom
        {
            static const float margin(-5.0f);
            static const Point basePosition(winSize.width - margin, margin);
            _bagButton = addFunctionButton("icon_beibao_1", "icon_beibao_2", "button_2", basePosition);
            _bagButton->addClickEventListener([this](Ref *pSender){
                SoundManager::getInstance()->playButtonSound();
                // TODO:
            });
            
            const auto& size = _bagButton->getContentSize();
            static const float offset(-20.0f);
            
            _questButton = addFunctionButton("icon_renwu_1", "icon_renwu_2", "button_2", basePosition + Point(0, size.height + offset));
            _questButton->addClickEventListener([this](Ref *pSender){
                SoundManager::getInstance()->playButtonSound();
                // TODO:
            });
            
            _optionButton = addFunctionButton("icon_shezhi_1", "icon_shezhi_2", "button_2", basePosition + Point(0, (size.height + offset) * 2));
            _optionButton->addClickEventListener([this](Ref *pSender){
                SoundManager::getInstance()->playButtonSound();
                // TODO:
                Director::getInstance()->getRunningScene()->addChild(CardLayer::create());
            });
            
            _guildButton = addFunctionButton("icon_gonghui_1", "icon_gonghui_2", "button_2", basePosition - Point(size.width + offset, 0));
            _guildButton->addClickEventListener([this](Ref *pSender){
                SoundManager::getInstance()->playButtonSound();
                // TODO:
                Director::getInstance()->getRunningScene()->addChild(BattleDeckLayer::create());
            });
            
            _armyButton = addFunctionButton("icon_jundui_1", "icon_jundui_2", "button_2", basePosition - Point((size.width + offset) * 2, 0));
            _armyButton->addClickEventListener([this](Ref *pSender){
                SoundManager::getInstance()->playButtonSound();
                Director::getInstance()->getRunningScene()->addChild(FormationLayer::create());
            });
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

Button* MainUILayer::addFunctionButton(const string& normal, const string& touched, const string& disabled, const Point& position)
{
    static const string csbFile("UI_FunctionIcon.csb");
    auto mainNode = CocosUtils::playAnimation(csbFile, 0, false);
    mainNode->setPosition(position);
    addChild(mainNode);
    
    auto root = mainNode->getChildByTag(21);
    auto button = dynamic_cast<Button*>(root->getChildByTag(22));
    if (button) {
        button->loadTextureDisabled("GameImages/public/" + disabled + ".png");
        setButtonIcons(button, 23, normal, touched);
        return button;
    }
    
    return nullptr;
}

#pragma mark - button icons
struct MainUILayer::ButtonIconInfo {
    int iconParentTag;
    std::string iconNormal;
    std::string iconTouched;
};

void MainUILayer::setButtonIcons(Button* button, int childTag, const string& normal, const string& touched)
{
    auto node = button->getChildByTag(childTag);
    if (node) {
        static const string prefix("GameImages/icons/");
        static const string suffix(".png");
        
        auto normalFile = prefix + normal + suffix;
        auto touchedFile = prefix + touched + suffix;
        
        addButtonIcon(node, normalFile);
        
        button->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                addButtonIcon(node, touchedFile);
            } else if (type == Widget::TouchEventType::CANCELED || type == Widget::TouchEventType::ENDED) {
                addButtonIcon(node, normalFile);
            }
        });
        
        if (_buttonIconInfos.find(button) == _buttonIconInfos.end()) {
            auto info = new (nothrow) ButtonIconInfo {childTag, normalFile, touchedFile};
            _buttonIconInfos.insert(make_pair(button, info));
        }
    }
}

void MainUILayer::setButtonEnabled(Button* button, bool enabled)
{
    if (button && button->isEnabled() != enabled) {
        button->setEnabled(enabled);
        
        if (_buttonIconInfos.find(button) != _buttonIconInfos.end()) {
            auto info = _buttonIconInfos.at(button);
            auto node = button->getChildByTag(info->iconParentTag);
            if (node) {
                node->removeChildByTag(buttonIconTag);
                auto file = enabled ? info->iconNormal : info->iconTouched;
                auto s = Sprite::create(file);
                s->setTag(buttonIconTag);
                node->addChild(s);
            }
        }
    }
}

void MainUILayer::addButtonIcon(Node* node, const string& file)
{
    if (node) {
        node->removeChildByTag(buttonIconTag);
        auto s = Sprite::create(file);
        s->setTag(buttonIconTag);
        node->addChild(s);
    }
}

void MainUILayer::removeButtonIcons()
{
    for (auto iter = begin(_buttonIconInfos); iter != end(_buttonIconInfos); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
}

#pragma mark - private
void MainUILayer::updateIcon()
{
    const string file("GameImages/avatars/icon_user.png");
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
    
}