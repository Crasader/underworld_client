//
//  WorldRender.hpp
//  Underworld_Client
//
//  Created by Mac on 16/6/23.
//
//

#ifndef WorldRender_h
#define WorldRender_h

#include <unordered_map>
#include <string>
#include "cocos2d.h"
#include "cocos-ext.h"

#include "World.h"
#include "Coordinate.h"
#include "Geometry.h"

class MapParticleConfigData;

namespace UnderWorld{ namespace Core{
    
class World;
class UWRender;
class WorldObject;
class AbstractWorldObjectRender;
class SpellPattern;
class HMMCardType;
    
// ===============================
// 	class WorldRender
//
//	World Render, Render World Environment & World Objects
// ===============================

class WorldRender : public WorldObserver {
public:
    /** types */
    enum class RenderLayer {
        Ground = 0,
        Shadow,
        Land,
        Air
    };
    
    /** consts */
    static const int IN_LAYER_WORLD_BACKGROUND_ZORDER = 0;
    static const int IN_LAYER_WORLD_TILEMAP_ZORDER = 1;

    static const std::string TILEDMAP_LAYER_FOREGROUND_NAME;
    static const std::string TILEDMAP_LAYER_LOGIC_NAME;
    
    static const int RENDER_LAYER_AIR_HEIGHT_IN_MAP_SIZE = 65;
    static const int CAMERA_ANGEL_DEGREES = 45;
    static const int EFFECT_RENDER_DELAY_THRESHOLD_IN_SECOND = 1;
    static const int UNIT_PLACE_TIPS_VIEW_OPACITY = 150;

    
private:
    /** instance */
    std::unordered_map<creatureid_t, AbstractWorldObjectRender*> _objectRenders;
    std::vector<AbstractWorldObjectRender*> _recycleBin;
    
    /** data */
    int _mapId;
    float _pixelPerMapsizeX;
    float _pixelPerMapsizeY;
    int _cameraAngelDegrees;
    float _cameraAngelRadians;
    int _worldHeight;
    int _worldWidth;
    
    /** status */
    std::vector<World::EventLog> _events;
    
    /** refs */
    const World* _world;
    const UWRender* _gameRender;
    
    /** cocos */
    cocos2d::Node* _worldLayer;
    cocos2d::Node* _worldContainer;
    cocos2d::experimental::TMXTiledMap* _tiledMap;
    std::unordered_set<cocos2d::ParticleSystemQuad*> _mapParticles;
    cocos2d::Node* _hmmCardRegionTipView;
    cocos2d::Node* _hmmCardPlaceTipView;
    cocos2d::Node* _hmmCardPlaceTipViewShadow;
    const HMMCardType* _tipCardType;
    
    
public:
    /** init & destroy */
    WorldRender();
    virtual ~WorldRender();
    // return true if init success, else this object is not safe
    virtual bool init(const World* world, const UWRender* gameRender);
    
    /** override WorldObserver */
    virtual void onNotifyWorldEvents(const std::vector<World::EventLog>& events) override;
    virtual void onNotifyWorldReleased() override;
    
    /** interface */
    virtual void render(const World* world);
    virtual void deleteWorldObjectRender(const AbstractWorldObjectRender* objectRender);
    cocos2d::Vec2 worldCoordinate2CocosPoint(const Coordinate32& pos,
        RenderLayer renderLayer,
        int32_t height = 0);
    int worldCoordinate2Zorder(const Coordinate32& pos,
        RenderLayer renderLayer,
        int32_t height = 0);
    Coordinate32 cocosPoint2WorldCoordinate(const cocos2d::Vec2& pos);
    cocos2d::Node* addEffect(const std::string& renderKey, bool loop,
        const Coordinate32& pos);
    void showHMMCardRegionTips(const HMMCardType* cardType,
        const Rect32& summonRegion, const Rect32& towerRegion);
    void hideHMMCardRegionTips();
    void showHMMCardPlaceTips(const HMMCardType* cardType, const Coordinate32& pos, bool enable);
    void hideHMMCardPlaceTips();
    
    
    /** getter */
    cocos2d::Node* getWorldLayer()               {return _worldLayer;}
    cocos2d::Node* getWorldContainer()           {return _worldContainer;}
    const World* getWorld() const                {return _world;}
    const UWRender* getGameRender() const        {return _gameRender;}
    int getCameraAngelDegrees() const            {return _cameraAngelDegrees;}
    float getCameraAngelRadians() const          {return _cameraAngelRadians;}

protected:
    /** inner interface */
    virtual void createWorldObjectRender(const WorldObject* object);
    virtual void deleteAllWorldObjectRender();
    virtual bool initWorldEnvironment();
    const cocos2d::Size& getWorldSizeInTile();
    const cocos2d::Size& getTile2PixelScale();
    virtual void renderSpellPattern(const SpellPattern* sp, creatureid_t targetId, const Coordinate32& pos);
    static void createHMMCardPlaceTipsView(const HMMCardType* cardType, const World* world, cocos2d::Node*& outputTipsView, cocos2d::Node*& outputShadowView);
    
    /** resources */
    void getWorldBgFile(int mapId, std::vector<std::string>& output);
    std::string getWorldTMXTiledFile(int mapId);
    
    /** cocos */
    void addMapParticle(const MapParticleConfigData* particleData);
    
    /** misc */
    void handleEvents();
    void recycle();
};
    
}}

#endif /* WorldRender_h */
