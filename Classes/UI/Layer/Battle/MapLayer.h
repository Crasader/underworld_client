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
    
    UnderWorld::Core::Coordinate convertPoint(const Point& layerPoint);
    void addUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    void repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    
    void updateUnitMask(const UnderWorld::Core::UnitType* unitType, const Point& layerPoint);
    void removeUnitMask();
    
    void updateSpellRing(const std::string& name, const Point& layerPoint, int range);
    void removeSpellRing();
    void checkUnitInSpellRing(Node* unit);
    
    const UnderWorld::Core::MapSetting& getMapSetting() const;
    
    void addAoeSpell(const Point& startPoint, const std::string& name, float duration);
    void addSpell(const std::string& name, float duration);
    void removeAllSpellEffects();
    
    void addPlaceUnitEffect(const UnderWorld::Core::Coordinate& point);
    
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
    Point convertToScrollViewPoint(const Point& layerPoint);
    
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
    std::string _selectedUnitName;
    Node* _selectedUnitMask;
};

#endif /* MapLayer_h */
