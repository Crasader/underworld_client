//
//  LoadingLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/9.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LoadingLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "CheatConfiguration.h"
#include "FrameLoader.h"
#include "DataManager.h"

void LoadingLayer::show(bool pvp, const Callback& callback)
{
    auto scene(Director::getInstance()->getRunningScene());
    if (scene) {
        scene->addChild(LoadingLayer::create(pvp, callback));
    }
}

LoadingLayer* LoadingLayer::create(bool pvp, const Callback& callback)
{
    auto ret = new (std::nothrow) LoadingLayer();
    if (ret && ret->init(pvp, callback)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LoadingLayer::LoadingLayer()
:_progressTimer(nullptr)
,_hint(nullptr)
,_pvp(false)
,_callback(nullptr)
,_isLoading(false)
,_loaded(0) {}

LoadingLayer::~LoadingLayer()
{
    removeAllChildren();
}

bool LoadingLayer::init(bool pvp, const Callback& callback)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        _pvp = pvp;
        _callback = callback;
        
        const auto& winSize(Director::getInstance()->getWinSize());
        auto sprite(Sprite::create(CocosUtils::getResourcePath("loading.png")));
        sprite->setPosition(winSize.width / 2, winSize.height / 2);
        addChild(sprite);
        
        static const float edgeY(50);
        const auto& size(sprite->getContentSize());
        Sprite* bg = Sprite::create(CocosUtils::getResourcePath("ui_tiao_39.png"));
        bg->setPosition(size.width / 2, edgeY);
        sprite->addChild(bg);
        
        const auto& psize(bg->getContentSize());
        auto pt = ProgressTimer::create(Sprite::create(CocosUtils::getResourcePath("ui_tiao_40.png")));
        pt->setType(ProgressTimer::Type::BAR);
        pt->setBarChangeRate(Vec2(1.0f, 0.0f));
        pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
        pt->setPercentage(0);
        pt->setPosition(psize.width / 2, psize.height / 2);
        bg->addChild(pt);
        _progressTimer = pt;
        
        auto hint = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        hint->setPosition(psize.width / 2, psize.height / 2);
        bg->addChild(hint);
        _hint = hint;
        
        loadResources();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(LoadingLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(LoadingLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool LoadingLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return false;
}

void LoadingLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

void LoadingLayer::loadResources()
{
    if (CheatConfiguration::getInstance()->loadPVR && !_isLoading) {
        _isLoading = true;
        _loaded = 0;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        loadResource();
#else
        DataManager::getInstance()->addAllFrames();
        onResourcesLoaded();
#endif
    } else {
        onResourcesLoaded();
    }
}

void LoadingLayer::loadResource()
{
    const auto& resources(DataManager::getInstance()->getPVRFiles());
    auto count(resources.size());
    if (_loaded < count) {
        FrameLoader::getInstance()->addAsync(resources.at(_loaded), [this, count](const string& file) {
            ++ _loaded;
            float p(100.0 * _loaded / count);
            _progressTimer->setPercentage(p);
            _hint->setString(StringUtils::format("%.0f%%", p));
            loadResource();
        });
    } else {
        onResourcesLoaded();
    }
}

void LoadingLayer::onResourcesLoaded()
{
    _isLoading = false;
    _loaded = 0;
    if (_callback) {
        _callback(this);
    } else {
        removeFromParent();
    }
}
