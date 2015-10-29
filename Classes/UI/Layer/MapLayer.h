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

using namespace std;
USING_NS_CC;
//using namespace ui;

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
    
    void addUnit(Node* unit, const UnderWorld::Core::Coordinate& pos);
    void repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& pos);
protected:
    virtual ~MapLayer();
    MapLayer();
private:
    int _mapId;
    cocos2d::experimental::TMXTiledMap* _tiledMap;
    cocos2d::extension::ScrollView *_scrollView;
    cocos2d::experimental::TMXLayer* _mainLayer;
};

#endif /* MapLayer_h */
