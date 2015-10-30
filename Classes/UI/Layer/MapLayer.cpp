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
:_mapId(INVALID_VALUE)
,_tiledMap(nullptr)
,_scrollView(nullptr)
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
        _mapId = mapId;
        const Size& winSize = Director::getInstance()->getWinSize();
        //--------- container ----------//
        _scrollView = cocos2d::extension::ScrollView::create();
        _scrollView->setDirection(cocos2d::extension::ScrollView::Direction::BOTH);
        _scrollView->setTouchEnabled(true);
        _scrollView->setBounceable(false);
        _scrollView->setViewSize(winSize);
        _scrollView->setPosition(Point::ZERO);
        _scrollView->setDelegate(this);
        addChild(_scrollView);
//        mRoot = Node::create();
//        mScrollView->addChild(mRoot);
        //--------- background ---------//
        int i = 1;
        float offsetX = .0f;
        while (true) {
            string fileName = StringUtils::format("map/bg%d_%d.png", _mapId, i);
            if(FileUtils::getInstance()->isFileExist(fileName)){
                Sprite* bg = Sprite::create(fileName);
                bg->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
                bg->setPosition(Point(offsetX, .0f));
                _scrollView->addChild(bg);
                offsetX += bg->getContentSize().width;
            } else {
                break;
            }
            i++;
        }
        //--------- foreground ---------- //
        _tiledMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("map/%d.tmx", _mapId));
        _scrollView->addChild(_tiledMap, TILEDMAP_ZORDER, TILEDMAP_TAG);
        _mainLayer = _tiledMap->getLayer(TILEDMAP_LAYER_FOREGROUND);
        const Size &mapSize = _tiledMap->getMapSize();
        _width = mapSize.width;
        _height = mapSize.height;
        const Size &ls = _mainLayer->getLayerSize();
        for (unsigned int y = 0; y < ls.height; y++)
        {
            int zOrder = calcZOrder(y);
            for (unsigned int x = 0; x < ls.width; x++)
            {
                Sprite *tile = _mainLayer->getTileAt(Vec2(x, y));
                if (tile) {
//                    tile->setLocalZOrder(zOrder);
                    _mainLayer->reorderChild(tile, zOrder);
                }
            }
        }
        CCLOG("%zd foreground", _mainLayer->getChildren().size());
//
//        //--------- logic ---------//
        cocos2d::experimental::TMXLayer *logicLayer = _tiledMap->getLayer(TILEDMAP_LAYER_LOGIC);
        logicLayer->setVisible(false);
        const Size &logicSize = logicLayer->getLayerSize();
        mapSetting.width = logicSize.width;
        mapSetting.height = logicSize.height;
        for (unsigned int x = 0; x < logicSize.width; x++)
        {
            for (unsigned int y = 0; y < logicSize.height; y++)
            {
                int gid = logicLayer->getTileGIDAt(Vec2(x, y));
                if (gid == 0) {
                    //can walk
                    mapSetting.add(mapCoordinate2coreCoordinate(x, y));
                } else {
                    //can not walk
                }
            }
        }
        CCLOG("%zd logicLayer", mapSetting.walkableArea.size());
        logicLayer->removeFromParent();
        
        //--------- scale ---------//
        const Size &mapPSize = _tiledMap->getContentSize();
        float scale = RESOLUTION_HEIGHT / mapPSize.height;
        _scrollView->setContentSize(mapPSize);
        _scrollView->setMaxScale(TILEDMAP_MAX_SCALE);
        _scrollView->setMinScale(scale);
        _scrollView->setZoomScale(scale);
        
        //--------- event ---------//
        auto listener = EventListenerTouchAllAtOnce::create();
        listener->onTouchesMoved = CC_CALLBACK_2(MapLayer::onTouchesMoved, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        return true;
    }
    return false;
}

int MapLayer::calcZOrder(int mapCoordinateY)
{
    return 2 * (mapCoordinateY + 1);
}


UnderWorld::Core::Coordinate MapLayer::mapCoordinate2coreCoordinate(int x, int y)
{
    return UnderWorld::Core::Coordinate(x, _height - y);
}

Point MapLayer::coreCoordinate2mapCoordinate(int x, int y)
{
    return Point(x, _height - y);
}

void MapLayer::addUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate)
{
    const Point& mapCoordinate = coreCoordinate2mapCoordinate(coreCoordinate.x, coreCoordinate.y);
    unit->setPosition(_mainLayer->getPositionAt(mapCoordinate));
    _mainLayer->addChild(unit, calcZOrder(mapCoordinate.y));
}

void MapLayer::repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate)
{
    const Point& mapCoordinate = coreCoordinate2mapCoordinate(coreCoordinate.x, coreCoordinate.y);
    unit->setPosition(_mainLayer->getPositionAt(mapCoordinate));
    reorderChild(unit, calcZOrder(mapCoordinate.y));
}

void MapLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event)
{
//    Sprite* x = Sprite::create("map/images/tree1.png");
//    addUnit(x, UnderWorld::Core::Coordinate(200,30));
}

void MapLayer::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
    
}

void MapLayer::scrollViewDidZoom(cocos2d::extension::ScrollView* view)
{
}