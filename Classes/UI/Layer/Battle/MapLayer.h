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
#include <unordered_set>

namespace UnderWorld { namespace Core {
    class UnitType;
}}

class MapParticleConfigData;

USING_NS_CC;
using namespace UnderWorld::Core;

class MapLayerObserver
{
public:
    virtual ~MapLayerObserver() {}
    virtual void onMapLayerTouchBegan(const Point& point) = 0;
    virtual void onMapLayerTouchMoved(const Point& point, bool isValid) = 0;
    virtual void onMapLayerTouchEnded(const Point& point) = 0;
};

class MapLayer : public LayerColor, public extension::ScrollViewDelegate
{
public:
    static MapLayer* create(int mapId);
    virtual ~MapLayer();
    
    void registerObserver(MapLayerObserver *observer);
    Coordinate32 point2Coordinate(const Point& point) const;
    
    // unit
    void addUnit(Node* unit, const Coordinate32& coordinate);
    void repositionUnit(Node* unit, const Coordinate32& coordinate);
    
    // effects
    void addBulletExplosionEffect(const std::string& name, const Coordinate32& coordinate);
    void addUnitPlacementEffect(const Coordinate32& coordinate);
    void setPlacedArea(float x1, float x2);
    
    // spells
    void addSpell(const std::string& name, float duration);
    void addAoeSpell(const Point& startPoint, const std::string& name, float duration);
    void removeAllSpellEffects();
    
    // unit mask
    void updateUnitMask(const UnitType* unitType, const Coordinate32& coordinate);
    void removeUnitMask();
    
    // spell ring
    void updateSpellRing(const std::string& name, const Coordinate32& coordinate, int range);
    void removeSpellRing();
    void checkUnitWithSpellRing(Node* unit);
    
protected:
    MapLayer();
    
    // LayerColor
    bool init(int mapId);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // ScrollViewDelegate
    virtual void scrollViewDidScroll(extension::ScrollView* view) override;
    virtual void scrollViewDidZoom(extension::ScrollView* view) override;
    
    void scrollChecking(float dt);
    
    // Coordinates
    Coordinate32 mapCoordinate2coreCoordinate(int x, int y);
    Point coordinate2Point(const Coordinate32& coordinate);
    int getZOrder(int coordinateY) const;
    void coordinateConversion(const Coordinate32& coordinate, Point& mapPosition, int& zOrder);
    
    // effects
    void addParticle(const MapParticleConfigData* data);
    void removeParticle(ParticleSystemQuad* effect);
    
    // spells
    Node* addSpellEffect(const std::string& file, bool loop, const Point& position);
    void removeSpellEffect(Node* effect);
    void addButterfly();
    
    // unit mask
    Node* createUnitMask(const UnitType* ut) const;
    
    // spell ring
    Node* createRing(const std::string& name, const Point& point);
    void removeStaticRing(const Point& point);
    void clearRingInfo();
    Rect getSpellRingBoundingBox() const;
    
    // areas
    void addUnplacedArea(float x1, float x2);
    void removeUnplacedAreas();
    void addBuildingPlacedAreas();
    void removeBuildingPlacedAreas();
    
    // tiles
    void addMapTiles();
    void removeMapTiles();
    
protected:
    MapLayerObserver *_observer;
    int _mapId;
    int _width;
    int _height;
    int _tileWidth;
    int _tileHeight;
    bool _isScrolling;
    Vec2 _scrollViewOffset;
    bool _touchMoved;
    experimental::TMXTiledMap* _tiledMap;
    Node* _mainLayer;
    extension::ScrollView* _scrollView;
    std::string _selectedUnitName;
    Node* _selectedUnitMask;
    
    // effects
    std::unordered_set<ParticleSystemQuad*> _particles;
    std::unordered_set<Node*> _spellEffects;
    Node* _butterfly;
    
    // spell ring
    std::pair<std::string, Node*> _spellRing;
    std::map<Point, Node*> _staticSpellRings;
    std::unordered_set<Node*> _nodesInTheRing;
    
    // areas
    std::vector<Node*> _unplacedAreas;
    std::vector<Sprite*> _buildingPlacedAreas;
    
    // tiles
    std::vector<SpriteBatchNode*> _tilesBatchNodes;
};

#endif /* MapLayer_h */
