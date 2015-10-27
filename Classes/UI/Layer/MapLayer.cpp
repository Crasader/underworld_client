//
//  MapLayer.cpp
//  Underworld_Client
//
//  Created by burst on 15/10/26.
//
//

#include "MapLayer.h"
#include "CocosGlobal.h"
#include "2d/CCFastTMXLayer.h"
#include "2d/CCFastTMXTiledMap.h"

static const int TILEDMAP_TAG = 2;
static const int TILEDMAP_ZORDER = 2;
static const string TILEDMAP_LAYER_LOGIC = "logic";
static const string TILEDMAP_LAYER_FOREGROUND = "fg";

MapLayer::MapLayer()
:mMapId(INVALID_VALUE)
,mBackground(nullptr)
,mTiledMap(nullptr)
,mRoot(nullptr)
,mScrollView(nullptr)
{
    
}

MapLayer::~MapLayer()
{
    
}

MapLayer* MapLayer::create(int mapId)
{
    MapLayer* pRet = new (nothrow)MapLayer();
    if(pRet && pRet->init(mapId)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool MapLayer::init(int mapId)
{
    if (LayerColor::init()) {
        mMapId = mapId;
        const Size& winSize = Director::getInstance()->getWinSize();
        //--------- container ----------//
        mScrollView = cocos2d::extension::ScrollView::create();
        mScrollView->setDirection(cocos2d::extension::ScrollView::Direction::BOTH);
        mScrollView->setTouchEnabled(true);
        mScrollView->setBounceable(false);
        mScrollView->setViewSize(winSize);
        mScrollView->setPosition(Point::ZERO);
        addChild(mScrollView);
//        mRoot = Node::create();
//        mScrollView->addChild(mRoot);
        //--------- background ---------//
        int i = 1;
        float offsetX = .0f;
        while (true) {
            string fileName = StringUtils::format("map/bg%d_%d.png", mMapId, i);
            if(FileUtils::getInstance()->isFileExist(fileName)){
                Sprite* bg = Sprite::create(fileName);
                bg->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
                bg->setPosition(Point(offsetX, .0f));
                mScrollView->addChild(bg);
                offsetX += bg->getContentSize().width;
            } else {
                break;
            }
            i++;
        }
        //--------- foreground ---------- //
        mTiledMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("map/%d.tmx", mMapId));
        mScrollView->addChild(mTiledMap, TILEDMAP_ZORDER, TILEDMAP_TAG);
        cocos2d::experimental::TMXLayer *foreground = mTiledMap->getLayer(TILEDMAP_LAYER_FOREGROUND);
        const Size &mapSize = mTiledMap->getMapSize();
        const Size &mapPSize = mTiledMap->getContentSize();
        float scale = RESOLUTION_HEIGHT / mapPSize.height;
        mScrollView->setContentSize(Size(mapPSize.width * scale, RESOLUTION_HEIGHT));
        mScrollView->setMaxScale(1.0f);
        mScrollView->setMinScale(scale);
//        mScrollView->setZoomScale(scale);
        //--------- logic ---------//
        
        return true;
    }
    return false;
}