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
#include <unordered_set>

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class UnitType;
}}

class MapParticleConfigData;

class MapLayerObserver
{
public:
    virtual ~MapLayerObserver() {}
    virtual void onMapLayerTouchBegan(const Point& point) = 0;
    virtual void onMapLayerTouchMoved(const Point& point, bool isValid) = 0;
    virtual void onMapLayerTouchEnded(const Point& point) = 0;
};

class MapLayer : public LayerColor, public cocos2d::extension::ScrollViewDelegate
{
public:
    static MapLayer* create(int mapId, const std::string& mapData);
    void registerObserver(MapLayerObserver *observer);
    
    UnderWorld::Core::Coordinate32 convertPoint(const Point& layerPoint);
    void addUnit(Node* unit, const UnderWorld::Core::Coordinate32& coordinate);
    void repositionUnit(Node* unit, const UnderWorld::Core::Coordinate32& coordinate);
    void addBulletExplosionEffect(const std::string& name, const UnderWorld::Core::Coordinate32& coordinate);
    
    void updateUnitMask(const UnderWorld::Core::UnitType* unitType, const UnderWorld::Core::Coordinate32& coordinate);
    void removeUnitMask();
    
    void updateSpellRing(const std::string& name, const UnderWorld::Core::Coordinate32& coordinate, int range);
    void removeSpellRing();
    void checkUnitInSpellRing(Node* unit);
    
    const UnderWorld::Core::MapSetting& getMapSetting() const;
    
    void addAoeSpell(const Point& startPoint, const std::string& name, float duration);
    void addSpell(const std::string& name, float duration);
    void removeAllSpellEffects();
    
    void addPlaceUnitEffect(const UnderWorld::Core::Coordinate32& coordinate);
    void setPlacedArea(float x1, float x2);
    void clearUnplacedAreas();
    
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
    UnderWorld::Core::Coordinate32 mapCoordinate2coreCoordinate(int x, int y);
    Point coordinate2Point(const UnderWorld::Core::Coordinate32& coordinate);
    inline int calcZOrder(int coreCoordinateY);
    void coordinateConvert(const UnderWorld::Core::Coordinate32& coordinate, Point& mapPosition, int& zOrder);
    
    //-------- effects --------//
    void addParticle(const MapParticleConfigData* data);
    void removeParticle(ParticleSystemQuad* effect);
    void addButterfly();
    Node* addSpellEffect(const std::string& file, bool loop, const Point& position);
    void removeSpellEffect(Node* effect);
    void scrollChecking(float dt);
    
    void clearRingInfo();
    Node* createRing(const std::string& name, const Point& point);
    void removeStaticRing(const Point& point);
    Rect getSpellRingBoundingBox() const;
    
    void addUnplacedArea(float x1, float x2);
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
    std::pair<std::string, Node*> _spellRing;
    std::map<Point, Node*> _staticSpellRings;
    Node* _butterfly;
    Vec2 _scrollViewOffset;
    bool _touchMoved;
    bool _isScrolling;
    std::unordered_set<ParticleSystemQuad*> _particles;
    std::unordered_set<Node*> _nodesInTheRing;
    std::unordered_set<Node*> _spellEffects;
    std::vector<Node*> _unplacedAreas;
    std::string _selectedUnitName;
    Node* _selectedUnitMask;
};

#endif /* MapLayer_h */
