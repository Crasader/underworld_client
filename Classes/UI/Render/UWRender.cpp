//
//  GameRender.cpp
//  Underworld_Client
//
//  Created by Mac on 16/6/22.
//
//

#include "UWRender.h"
#include "WorldRender.h"
#include "Gui.h"
#include "HMMGui.h"
#include "Game.h"
#include "CocosGlobal.h"
#include "GameModeHMM.h"

namespace UnderWorld{ namespace Core{
    
// =====================================================
// 	class UWRender
// =====================================================
    
// ===================== public ========================

UWRender::~UWRender() {
    M_SAFE_DELETE(_worldRender);
    M_SAFE_DELETE(_gui);
}

// ===================== override AbstractRender ========================
    
void UWRender::init(const Game *game, Commander *commander) {
    clean();
    
    _game = game;
    _commader = commander;
    
    if (!_worldRender) assert(_worldRender = createWorldRender());
    if (_worldRender) _worldRender->init(game->getWorld(), this);

    if (!_gui) assert(_gui = createGui(game->getGameMode()));
    if (_gui) {
        _gui->init(game, commander, _worldRender);
        _gui->setRenderListener(_listener);
    }
    
    if (_scene) {
        const cocos2d::Size& winSize = cocos2d::Director::getInstance()->getWinSize();
        
        // scroll view
        _worldScrollView = cocos2d::extension::ScrollView::create();
        _worldScrollView->setDirection(cocos2d::extension::ScrollView::Direction::BOTH);
        _worldScrollView->setTouchEnabled(true);
        _worldScrollView->setBounceable(false);
        _worldScrollView->setViewSize(winSize);
        _worldScrollView->setPosition(cocos2d::Point(0, MAP_OFFSET_Y));
        _worldScrollView->setDelegate(this);
        
        

        _scene->addChild(_worldScrollView, IN_SCENE_WORLD_RENDER_ZORDER);
        
        //world render
        if (_worldRender && _worldRender->getWorldLayer()) {
            _worldScrollView->addChild(_worldRender->getWorldLayer());
            
            const cocos2d::Size &worldSize = _worldRender->getWorldLayer()->getContentSize();
            float scale = RESOLUTION_HEIGHT / worldSize.height;
            _worldScrollView->setContentSize(worldSize);
            _worldScrollView->setMaxScale(0);
            _worldScrollView->setMinScale(scale);
            _worldScrollView->setZoomScale(scale);
        }
        
        //gui
        if (_gui && _gui->getGuiView()) {
            _scene->addChild(_gui->getGuiView(), IN_SCENE_GUI_ZORDER);
        }
    }
}
    
void UWRender::render(const Game *game) {
    if (!game) return;
        
    if (_worldRender) _worldRender->render(game->getWorld());
    if (_gui) _gui->render(game);
}
    
void UWRender::setRenderListener(RenderListener* listener) {
    _listener = listener;
    if (_gui) _gui->setRenderListener(_listener);
}
    
// ===================== override ScrollViewDelegate ========================

void UWRender::scrollViewDidScroll(cocos2d::extension::ScrollView* view) {
}

void UWRender::scrollViewDidZoom(cocos2d::extension::ScrollView* view) {
}
    
// ===================== private ========================
    
// ===================== creator ========================
    
WorldRender* UWRender::createWorldRender() const {
    return new WorldRender();
}
    
Gui* UWRender::createGui(const AbstractGameMode* gameMode) const {
    if (dynamic_cast<const GameModeHMM*>(gameMode)) {
        return new HMMGui();
    }
    return nullptr;
}

// ===================== misc ========================

void UWRender::clean() {
    _game = nullptr;
    _commader = nullptr;
    
    if (_scene) {
        _scene->removeAllChildren();
    }
    _worldScrollView = nullptr;
}
    

    
    
}}
