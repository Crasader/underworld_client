//
//  MapLayer.h
//  Underworld_Client
//
//  Created by burst on 15/10/26.
//
//

#ifndef MapLayer_h
#define MapLayer_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "Coordinate.h"
#include "GameSettings.h"

USING_NS_CC;

class MapParticleConfigData;

class MapLayerObserver
{
public:
    virtual ~MapLayerObserver() {}
    virtual void onMapLayerTouchEnded() = 0;
};

class MapLayer : public LayerColor, public cocos2d::extension::ScrollViewDelegate
{
public:
    static MapLayer* create(int mapId, const std::string& mapData);
    void registerObserver(MapLayerObserver *observer);
    
    UnderWorld::Core::Coordinate convertPoint(const Point& layerPoint);
    void addUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    void repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    
    void updateSpellRangeRing(const Point& layerPoint, int range);
    void removeSpellRangeRing();
    void checkUnitInSpellRangeRing(Node* unit);
    
    const UnderWorld::Core::MapSetting& getMapSetting() const;
    
    void addFireballSpellEffect();
    void removeAllSpellEffects();
protected:
    virtual ~MapLayer();
    MapLayer();
    bool init(int mapId, const std::string& mapData);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    //-------- scrollviewdelegate --------//
    virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) override;
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) override;
    
    //-------- coordinates --------//
    inline UnderWorld::Core::Coordinate mapCoordinate2coreCoordinate(int x, int y);
    inline void coordinateConvert(const UnderWorld::Core::Coordinate& coreCoordinate, Point& mapPosition, int& zOrder);
    inline int calcZOrder(int coreCoordinateY);
    
    //-------- effects --------//
    void addParticle(const MapParticleConfigData* data);
    void removeParticle(ParticleSystemQuad* effect);
    void addButterfly();
    Node* addSpellEffect(const std::string& file, bool loop, const Point& position);
    void removeSpellEffect(Node* effect);
    void scrollChecking(float dt);
    
    Rect getSpellRangeRingBoundingBox() const;
private:
    MapLayerObserver *_observer;
    int _mapId;
    int _width;
    int _height;
    int _tileWidth;
    int _tileHeight;
    cocos2d::experimental::TMXTiledMap* _tiledMap;
    cocos2d::extension::ScrollView *_scrollView;
    UnderWorld::Core::MapSetting _mapSetting;
    Node* _mainLayer;
    Node* _spellRing;
    Node* _butterfly;
    Vec2 _scrollViewOffset;
    bool _isScrolling;
    std::set<ParticleSystemQuad*> _particles;
    std::set<Node*> _nodesInTheRing;
    std::set<Node*> _spellEffects;
};

#endif /* MapLayer_h */
