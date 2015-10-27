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

class MapLayer : public LayerColor {
public:
    virtual bool init(int mapId);
    static MapLayer* create(int mapId);
protected:
    virtual ~MapLayer();
    MapLayer();
private:
    int mMapId;
    Node *mRoot;
    Sprite* mBackground;
    cocos2d::experimental::TMXTiledMap* mTiledMap;
    cocos2d::extension::ScrollView *mScrollView;
};

#endif /* MapLayer_h */
