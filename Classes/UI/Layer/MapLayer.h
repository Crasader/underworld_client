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
protected:
    virtual ~MapLayer();
    MapLayer();
private:
    int mMapId;
    cocos2d::experimental::TMXTiledMap* mTiledMap;
    cocos2d::extension::ScrollView *mScrollView;
};

#endif /* MapLayer_h */
