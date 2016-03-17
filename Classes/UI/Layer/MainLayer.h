//
//  MainLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MainLayer_h
#define MainLayer_h

#define TCP_CLIENT_TEST_CPP (0)
#define TCP_CLIENT_TEST_OC  (0)

#if TCP_CLIENT_TEST_CPP
#if TCP_CLIENT_TEST_OC
#undef TCP_CLIENT_TEST_OC
#define TCP_CLIENT_TEST_OC  (0)
#endif
#endif

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#if TCP_CLIENT_TEST_CPP
#include "TCPClient.hpp"
#endif

#if TCP_CLIENT_TEST_OC
#include "TCPClient.h"
#endif

USING_NS_CC;
class MainUILayer;

class MainLayerObserver
{
public:
    virtual ~MainLayerObserver() {}
};

class MainLayer : public LayerColor
#if TCP_CLIENT_TEST_OC
, public TCPClientObserver
#endif
{
public:
    static Scene* createScene();
    virtual ~MainLayer();
    void registerObserver(MainLayerObserver *observer);
    
protected:
    static MainLayer* create();
    MainLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual void onEnter() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
#if TCP_CLIENT_TEST_OC
    // TCPClientObserver
    virtual void onConnect(const std::string& url, uint16_t port) override;
    virtual void onWriteData(long tag) override;
    virtual void onReadData(const std::string& data, long tag) override;
    virtual void onDisconnect(TcpErrorCode code) override;
#endif
    
    void addLevelButtons();
    
private:
    MainLayerObserver *_observer;
    MainUILayer *_uiLayer;
    Node *_mainNode;
    ui::ScrollView *_scrollView;
    bool _touchInvalid;
    
#if TCP_CLIENT_TEST_CPP
    int _fd;
#endif
};

#endif /* MainLayer_h */
