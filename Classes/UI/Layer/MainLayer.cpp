//
//  MainLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MainLayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "SoundManager.h"
#include "MainUILayer.h"
#include "BattleScene.h"

using namespace std;
using namespace ui;

Scene* MainLayer::createScene()
{
    Scene* scene = Scene::create();
    MainLayer* layer = MainLayer::create();
    scene->addChild(layer);
    return scene;
}

MainLayer* MainLayer::create()
{
    MainLayer *ret = new (nothrow) MainLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MainLayer::MainLayer()
:_observer(nullptr)
,_uiLayer(nullptr)
,_mainNode(nullptr)
,_scrollView(nullptr)
,_touchInvalid(false)
#if TCP_CLIENT_TEST_CPP
,_fd(-1)
#endif
{
    
}

MainLayer::~MainLayer()
{
    removeAllChildren();
}

void MainLayer::registerObserver(MainLayerObserver *observer)
{
    _observer = observer;
}

bool MainLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
#if true
        _mainNode = Sprite::create("GameImages/world_bg/world_1.png");
        _mainNode->setAnchorPoint(Point::ZERO);
#else
        static const string CsbFile("zhuchangjing.csb");
        _mainNode = CocosUtils::playCSBAnimation(CsbFile, false, 0, nullptr);
#endif
        
        const Size& nodeSize = _mainNode->getContentSize();
        _touchEnabled = (nodeSize.height > winSize.height) ? true : false;
        _scrollView = ui::ScrollView::create();
        _scrollView->setDirection(cocos2d::ui::ScrollView::Direction::BOTH);
        _scrollView->setTouchEnabled(_touchEnabled);
        _scrollView->setBounceEnabled(false);
        _scrollView->setContentSize(winSize);
        _scrollView->setPosition(Point::ZERO);
        _scrollView->setInnerContainerSize(_touchEnabled ? nodeSize : winSize);
        _scrollView->setSwallowTouches(!_touchEnabled);
        _scrollView->setScrollBarEnabled(false);
        _scrollView->addChild(_mainNode);
        addChild(_scrollView);
        
        // UI layer
        _uiLayer = MainUILayer::create();
        addChild(_uiLayer);
        
        addLevelButtons();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MainLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MainLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
#if TCP_CLIENT_TEST_CPP || TCP_CLIENT_TEST_OC
        static const string host("192.168.31.139");
        static const uint16_t port(8080);
#endif
        
#if TCP_CLIENT_TEST_CPP
        _fd = TCPClient::connect(host, port, 1000);
        TCPClient::write(_fd, "Hello World\n");
        string data;
        TCPClient::read(_fd, data, 20);
        CCLOG("");
#endif
        
#if TCP_CLIENT_TEST_OC
        TCPClient::init(this);
        TCPClient::connect(host, port, -1);
#endif
        
        return true;
    }
    
    return false;
}

void MainLayer::onEnter()
{
    LayerColor::onEnter();
    
    if (_scrollView)
    {
//        _scrollView->jumpToBottom();
        _scrollView->setInnerContainerPosition(Point(-1180, -720));
    }
}

bool MainLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void MainLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#if TCP_CLIENT_TEST_OC
#pragma mark - TCPClientObserver
void MainLayer::onConnect(const std::string& url, uint16_t port)
{
    CCLOG("onConnect");
    static const long tag(0);
    TCPClient::writeData("{\"code\":2,\"uid\":1,\"name\":\"p1\",\"cards\":\"骑士|弓箭手|炸弹人\"}", -1, tag);
    TCPClient::readData(-1, tag);
}

void MainLayer::onWriteData(long tag)
{
    CCLOG("onWriteData : %ld", tag);
//    TCPClient::writeData("Hello World", tag);
}

void MainLayer::onReadData(const char* data, unsigned long len, long tag)
{
    CCLOG("onReadData : %s, %ld", data, tag);
    TCPClient::disconnect();
}

void MainLayer::onDisconnect(TcpErrorCode code)
{
    CCLOG("onClose");
}
#endif

void MainLayer::addLevelButtons()
{
    static const size_t count(4);
    static const Vec2 positions[count] = {Vec2(0.5, 0.5), Vec2(0.47, 0.8), Vec2(0.75, 0.5), Vec2(0.8, 0.75)};
    const Size& size = _mainNode->getContentSize();
    for (int i = 0; i < count; ++i) {
        string file = StringUtils::format("GameImages/icons/guanqia/icon_guanka_%d.png", i + 1);
        Button* button = Button::create(file, file);
        button->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
            } else if (type == Widget::TouchEventType::MOVED) {
                if (!_touchInvalid && button->getTouchMovePosition().distance(button->getTouchBeganPosition()) > TOUCH_CANCEL_BY_MOVING_DISTANCE) {
                    _touchInvalid = true;
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                if (!_touchInvalid) {
                    // TODO:
                    SoundManager::getInstance()->playButtonSound();
                    CocosUtils::replaceScene(BattleScene::create(button->getTag() + 1), true);
                }
            }
        });
        button->setPosition(Point(size.width * positions[i].x, size.height * positions[i].y));
        button->setSwallowTouches(false);
        _mainNode->addChild(button, 0, i);
    }
}
