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
#include "Coordinate.h"
#include "MapSetting.h"
#include "Map.h"

static const int TILEDMAP_TAG = 2;
static const int TILEDMAP_ZORDER = 2;
static const string TILEDMAP_LAYER_LOGIC = "logic";
static const string TILEDMAP_LAYER_FOREGROUND = "fg";
static const unsigned int TILEDMAP_MAX_SCALE = 1.0f;

MapLayer::MapLayer()
:mMapId(INVALID_VALUE)
,mTiledMap(nullptr)
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
        mScrollView->setDelegate(this);
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
//        const Size &mapSize = mTiledMap->getMapSize();
        const Size &ls = foreground->getLayerSize();
        for (unsigned int y = 0; y < ls.height; y++)
        {
            int zOrder = 2 * (y + 1);
            for (unsigned int x = 0; x < ls.width; x++)
            {
                Sprite *tile = foreground->getTileAt(Vec2(x, y));
                if (tile) {
//                    tile->setLocalZOrder(zOrder);
                    foreground->reorderChild(tile, zOrder);
                }
            }
        }
        CCLOG("%zd foreground", foreground->getChildren().size());
//
//        //--------- logic ---------//
        cocos2d::experimental::TMXLayer *logicLayer = mTiledMap->getLayer(TILEDMAP_LAYER_LOGIC);
        logicLayer->setVisible(false);
        const Size &logicSize = logicLayer->getLayerSize();
        UnderWorld::Core::MapSetting mapSetting(logicSize.width, logicSize.height);
        for (unsigned int x = 0; x < logicSize.width; x++)
        {
            for (unsigned int y = 0; y < logicSize.height; y++)
            {
                int gid = logicLayer->getTileGIDAt(Vec2(x, y));
//                printf("[%d, %d]%d\n", x, y, gid);
                if (gid == 0) {
                    //can walk
                    mapSetting.add(UnderWorld::Core::Coordinate(x, y));
                } else {
                    //can not walk
                }
            }
        }
        //todo, call gamecore to set Map
        UnderWorld::Core::Map coreMap;
        coreMap.init(mapSetting);
        
        CCLOG("%zd logicLayer", logicLayer->getChildren().size());
        
        //--------- scale ---------//
        const Size &mapPSize = mTiledMap->getContentSize();
        float scale = RESOLUTION_HEIGHT / mapPSize.height;
        mScrollView->setContentSize(mapPSize);
        mScrollView->setMaxScale(TILEDMAP_MAX_SCALE);
        mScrollView->setMinScale(scale);
        mScrollView->setZoomScale(scale);
        
        //--------- event ---------//
        auto listener = EventListenerTouchAllAtOnce::create();
        listener->onTouchesMoved = CC_CALLBACK_2(MapLayer::onTouchesMoved, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        return true;
    }
    return false;
}

void MapLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event)
{
    
}

void MapLayer::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
    
}

void MapLayer::scrollViewDidZoom(cocos2d::extension::ScrollView* view)
{
}