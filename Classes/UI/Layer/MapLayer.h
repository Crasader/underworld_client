//
//  MapLayer.h
//  Underworld_Client
//
//  Created by burst on 15/10/26.
//
//

#ifndef MapLayer_h
#define MapLayer_h

#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "Coordinate.h"
#include "GameSettings.h"

using namespace std;
USING_NS_CC;

class MapLayer : public LayerColor, public cocos2d::extension::ScrollViewDelegate {
public:
    virtual bool init(int mapId);
    static MapLayer* create(int mapId);
    //-------- scrollviewdelegate --------//
    /**
     * @js NA
     * @lua NA
     */
    virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);
    /**
     * @js NA
     * @lua NA
     */
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view);
    
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    
    void addUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    void repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate);
    
    inline UnderWorld::Core::Coordinate mapCoordinate2coreCoordinate(int x, int y);
    inline void coordinateConvert(const UnderWorld::Core::Coordinate& coreCoordinate, Point& mapPosition, int& zOrder);
    
    inline int calcZOrder(int coreCoordinateY);
    
    UnderWorld::Core::MapSetting mapSetting;
protected:
    virtual ~MapLayer();
    MapLayer();
    
    void addButterfly();
private:
    int _mapId;
    int _width;
    int _height;
    int _tileWidth;
    int _tileHeight;
    cocos2d::experimental::TMXTiledMap* _tiledMap;
    cocos2d::extension::ScrollView *_scrollView;
    cocos2d::experimental::TMXLayer* _mainLayer;
    Node *_butterfly;
};

#endif /* MapLayer_h */
