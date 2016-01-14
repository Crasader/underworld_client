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

class MapLayer : public LayerColor, public cocos2d::extension::ScrollViewDelegate
{
public:
    static MapLayer* create(int mapId, const std::string& mapData);
    
    UnderWorld::Core::Coordinate convertPoint(const Point& layerPoint);
    void addUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    void repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    
    void updateSpellRangeRing(const Point& layerPoint);
    void removeSpellRangeRing();
    
    const UnderWorld::Core::MapSetting& getMapSetting() const;
protected:
    virtual ~MapLayer();
    MapLayer();
    bool init(int mapId, const std::string& mapData);
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    
    //-------- scrollviewdelegate --------//
    virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view);
    
    //-------- coordinates --------//
    inline UnderWorld::Core::Coordinate mapCoordinate2coreCoordinate(int x, int y);
    inline void coordinateConvert(const UnderWorld::Core::Coordinate& coreCoordinate, Point& mapPosition, int& zOrder);
    inline int calcZOrder(int coreCoordinateY);
    
    //-------- effects --------//
    void addParticle(const MapParticleConfigData* data);
    void removeParticle(ParticleSystemQuad* effect);
    void addButterfly();
    void scrollChecking(float dt);
private:
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
};

#endif /* MapLayer_h */
