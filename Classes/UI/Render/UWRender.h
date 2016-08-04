//
//  GameRender.h
//  Underworld_Client
//
//  Created by Mac on 16/6/22.
//
//

#ifndef UWRender_h
#define UWRender_h

#include "cocos2d.h"
#include "cocos-ext.h"

#include "ExternalInterface.h"

namespace UnderWorld{ namespace Core{
    
class WorldRender;
class Gui;
class AbstractGameMode;
    
// ===============================
// 	class UWRender
//
//	UnderWorld Game Render, Hold Game UI & Game Animations 
// ===============================
    
class UWRender : public AbstractRender, public cocos2d::extension::ScrollViewDelegate {
private:
    /** consts */
    static const int IN_SCENE_WORLD_RENDER_ZORDER = 0;
    static const int IN_SCENE_GUI_ZORDER = 1;
    
private:
    /** instance */
    WorldRender* _worldRender;
    Gui* _gui;
    
    /** refs */
    const Game* _game;
    Commander* _commader;
    
    /** cocos */
    cocos2d::Scene* _scene;
    cocos2d::extension::ScrollView* _worldScrollView;
    
public:
    UWRender(cocos2d::Scene* scene) :
        _scene(scene),
        _worldRender(nullptr),
        _gui(nullptr),
        _game(nullptr),
        _commader(nullptr),
        _worldScrollView(nullptr) {}
    virtual ~UWRender();

    /** override AbstractRender */
    virtual void init(const Game* game, Commander* commander) override;
    virtual void render(const Game* game) override;
    
    /** override ScrollViewDelegate */
    virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) override;
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) override;
    
    /** getters */
    const Game* getGame() const        {return _game;}
    
private:
    /** creator */
    virtual WorldRender* createWorldRender() const;
    virtual Gui* createGui(const AbstractGameMode* gameMode) const;
    
    /** misc */
    void clean();
};
    
}}


#endif /* UWRender_h */
