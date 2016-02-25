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
#include "Camp.h"
#include "Map.h"
#include "cocostudio/CocoStudio.h"
#include "tinyxml2/tinyxml2.h"
#include "CoreUtils.h"
#include "Constants.h"
#include "DataManager.h"
#include "MapParticleConfigData.h"
#include "GameConstants.h"
#include "URConfigData.h"

using namespace cocostudio;
using namespace std;

static const int TILEDMAP_TAG = 2;
static const int TILEDMAP_ZORDER = 2;
static const string TILEDMAP_LAYER_LOGIC = "logic";
static const string TILEDMAP_LAYER_FOREGROUND = "fg";

static Node* createUnitMask(const UnderWorld::Core::Camp* camp)
{
    if (camp) {
        string file;
        const string& name = camp->getBaseUnitType()->getRenderKey();
        const URConfigData* data = DataManager::getInstance()->getURConfigData(name);
        if (data) {
            static const int resourceId(2);
            UnderWorld::Core::UnitClass uc = camp->getBaseUnitType()->getUnitClass();
            if (UnderWorld::Core::kUnitClass_Core == uc || UnderWorld::Core::kUnitClass_Building == uc) {
                file = StringUtils::format(data->getBNormal().c_str(), resourceId);
            } else {
                const string& prefix = data->getPrefix();
                file = prefix + StringUtils::format("-standby-%d.csb", resourceId);
            }
            
            if (FileUtils::getInstance()->isFileExist(file)) {
                Node* actionNode = CSLoader::createNode(file);
                Sprite* sprite = dynamic_cast<Sprite*>(actionNode->getChildren().front());
                if (sprite) {
                    sprite->setOpacity(180);
                }
                return actionNode;
            }
        }
    }
    
    return nullptr;
}

MapLayer::MapLayer()
:_observer(nullptr)
,_mapId(INVALID_VALUE)
,_width(0)
,_height(0)
,_tileWidth(0)
,_tileHeight(0)
,_tiledMap(nullptr)
,_scrollView(nullptr)
,_mainLayer(nullptr)
,_spellRing(nullptr)
,_butterfly(nullptr)
,_isScrolling(false)
,_selectedUnitMask(nullptr)
{
    
}

MapLayer::~MapLayer()
{
    
}

MapLayer* MapLayer::create(int mapId, const string& mapData)
{
    MapLayer* pRet = new (nothrow)MapLayer();
    if(pRet && pRet->init(mapId, mapData)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool MapLayer::init(int mapId, const string& mapData)
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
        // TODO: change "_mapId"
        _tiledMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("map/%d.tmx", (2 - _mapId % 2)));
        _scrollView->addChild(_tiledMap, TILEDMAP_ZORDER, TILEDMAP_TAG);
        _mainLayer = _tiledMap->getLayer(TILEDMAP_LAYER_FOREGROUND);
        const Size &mapSize = _tiledMap->getMapSize();
        _width = mapSize.width;
        _height = mapSize.height;
        const Size &tileSize = _tiledMap->getTileSize();
        _tileWidth = tileSize.width;
        _tileHeight = tileSize.height;
        if (_mainLayer) {
            
            cocos2d::experimental::TMXLayer* tmxLayer = dynamic_cast<cocos2d::experimental::TMXLayer*>(_mainLayer);
            
            const Size &ls = tmxLayer->getLayerSize();
            for (unsigned int y = 0; y < ls.height; y++)
            {
                int zOrder = calcZOrder(ls.height - y);
                for (unsigned int x = 0; x < ls.width; x++)
                {
                    Sprite *tile = tmxLayer->getTileAt(Vec2(x, y));
                    if (tile) {
                        //                    tile->setLocalZOrder(zOrder);
                        tile->setVisible(false);
                        _mainLayer->reorderChild(tile, zOrder);
                    }
                }
            }
            CCLOG("%zd foreground", _mainLayer->getChildren().size());
        } else {
            _mainLayer = Node::create();
            _tiledMap->addChild(_mainLayer);
        }
//
//        //--------- logic ---------//
        cocos2d::experimental::TMXLayer *logicLayer = _tiledMap->getLayer(TILEDMAP_LAYER_LOGIC);
        logicLayer->setVisible(false);
        const Size &logicSize = logicLayer->getLayerSize();
        _mapSetting.setWidth(logicSize.width);
        _mapSetting.setHeight(logicSize.height);
        for (unsigned int x = 0; x < logicSize.width; x++)
        {
            for (unsigned int y = 0; y < logicSize.height; y++)
            {
                int gid = logicLayer->getTileGIDAt(Vec2(x, y));
                if (gid == 0) {
                    //can walk
                } else {
                    //can not walk
                    //TODO: land can't walk & air can't walk
                    _mapSetting.addUnWalkableArea(mapCoordinate2coreCoordinate(x, y), UnderWorld::Core::kFieldType_Land);
                }
            }
        }
        _mapSetting.init(mapData);
        CCLOG("%zd logicLayer", _mapSetting.getUnWalkableArea().size());
        logicLayer->removeFromParent();
        
        //--------- effect ---------//
        if (0) {
            // 1. butterfly
            addButterfly();
            
            // 2. campfire
            {
                static const string file("effect-place-1.csb");
                Node *effect = CSLoader::createNode(file);
                effect->setPosition(Point(875, 80));
                _tiledMap->addChild(effect);
                timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                effect->runAction(action);
                action->gotoFrameAndPlay(0, true);
            }
            
            // 3. smoke
            {
                static const string file("particle/guohuo.plist");
                ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
                effect->setPosition(Point(1455, 1020));
                effect->setScale(2.5f);
                _tiledMap->addChild(effect);
            }
            
            // 4. spark
            {
                static const string file("particle/huoxing.plist");
                ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
                effect->setPosition(Point(278, 232));
                _tiledMap->addChild(effect);
            }
            
            // 5. fire
            {
                static const string file("effect-fire-1.csb");
                Node *effect = CSLoader::createNode(file);
                effect->setPosition(Point(285, 245));
                _tiledMap->addChild(effect);
                timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                effect->setScaleX(0.6f);
                effect->setScaleY(1.3f);
                effect->runAction(action);
                action->gotoFrameAndPlay(0, true);
            }
        } else {
            const vector<MapParticleConfigData*>& particles = DataManager::getInstance()->getMapParticleConfigData(mapId);
            for (int i = 0; i < particles.size(); ++i) {
                addParticle(particles.at(i));
            }
        }
        
        //--------- scale ---------//
        const Size &mapPSize = _tiledMap->getContentSize();
        float scale = RESOLUTION_HEIGHT / mapPSize.height;
        _scrollView->setContentSize(mapPSize);
        _scrollView->setMaxScale(Constants::TILEDMAP_MAX_SCALE);
        _scrollView->setMinScale(scale);
        _scrollView->setZoomScale(scale);
        _scrollViewOffset = _scrollView->getContentOffset();
        
        //--------- event ---------//
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MapLayer::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(MapLayer::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MapLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        return true;
    }
    return false;
}

void MapLayer::registerObserver(MapLayerObserver *observer)
{
    _observer = observer;
}

const UnderWorld::Core::MapSetting& MapLayer::getMapSetting() const
{
    return _mapSetting;
}

int MapLayer::calcZOrder(int coreCoordinateY)
{
    return 2 * (_height - coreCoordinateY + 1);
}


UnderWorld::Core::Coordinate MapLayer::mapCoordinate2coreCoordinate(int x, int y)
{
    //fix:y is [0 - (_height - 1)]
    return UnderWorld::Core::Coordinate(x, (_height - 1) - y);
}

void MapLayer::coordinateConvert(const UnderWorld::Core::Coordinate& coreCoordinate, Point& mapPosition, int& zOrder)
{
    
    mapPosition.x = _tileWidth / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coreCoordinate.x;
    mapPosition.y = _tileHeight / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coreCoordinate.y;
    zOrder = 2 * (_height - coreCoordinate.y + 1);
}

Point MapLayer::convertToScrollViewPoint(const Point& layerPoint)
{
    Node* container = _scrollView->getContainer();
    const Point pos = _scrollView->convertToNodeSpace(convertToWorldSpace(layerPoint)) - container->getPosition();
    const Point realPos = Point(pos.x / container->getScaleX(), pos.y / container->getScaleY());
    return realPos;
}

UnderWorld::Core::Coordinate MapLayer::convertPoint(const Point& layerPoint)
{
    Point realPoint = _mainLayer->convertToNodeSpace(convertToWorldSpace(layerPoint));
    UnderWorld::Core::Coordinate coordinate;
    coordinate.x = realPoint.x * UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE / _tileWidth;
    coordinate.y = realPoint.y * UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE / _tileHeight;
    return coordinate;
}

void MapLayer::addUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate)
{
    Point pos;
    int zOrder;
    coordinateConvert(coreCoordinate, pos, zOrder);
    unit->setPosition(pos);
    _mainLayer->addChild(unit, zOrder);
}

void MapLayer::repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate)
{
    Point pos;
    int zOrder;
    coordinateConvert(coreCoordinate, pos, zOrder);
    if (pos == unit->getPosition()) {
        return;
    }
    unit->setPosition(pos);
    reorderChild(unit, zOrder);
}

void MapLayer::updateUnitMask(const UnderWorld::Core::Camp* camp, const Point& layerPoint)
{
    const string& name = camp->getBaseUnitType()->getRenderKey();
    if (_selectedUnitName != name) {
        removeUnitMask();
    }
    
    if (!_selectedUnitMask) {
        _selectedUnitName = name;
        _selectedUnitMask = createUnitMask(camp);
        _scrollView->addChild(_selectedUnitMask);
    }
    
    _selectedUnitMask->setPosition(convertToScrollViewPoint(layerPoint));
}

void MapLayer::removeUnitMask()
{
    if (_selectedUnitMask) {
        _selectedUnitMask->stopAllActions();
        _selectedUnitMask->removeFromParent();
        _selectedUnitMask = nullptr;
        _selectedUnitName = "";
    }
}

void MapLayer::updateSpellRangeRing(const Point& layerPoint, int range)
{
    if (!_spellRing) {
        static const string name("quan-1.csb");
        _spellRing = CSLoader::createNode(name);
        timeline::ActionTimeline *action = CSLoader::createTimeline(name);
        _spellRing->runAction(action);
        action->gotoFrameAndPlay(0, true);
        _scrollView->addChild(_spellRing);
    }
    
    // calculate scale
    static const float defaultRange(218);
    if (range != UnderWorld::Core::SpellType::CAST_DISTANCE_INFINITE) {
        const float scale = range / defaultRange;
        _spellRing->setScale(scale);
    }
    
    _spellRing->setPosition(convertToScrollViewPoint(layerPoint));
}

void MapLayer::removeSpellRangeRing()
{
    if (_spellRing) {
        _spellRing->stopAllActions();
        _spellRing->removeFromParent();
        _spellRing = nullptr;
    }
}

void MapLayer::checkUnitInSpellRangeRing(Node* unit)
{
    static GLubyte selectedOpacity(180);
    static GLubyte normalOpacity(255);
    bool selected(false);
    if (_spellRing && getSpellRangeRingBoundingBox().containsPoint(unit->getPosition())) {
        selected = true;
    }
    
    const GLubyte opacity(unit->getOpacity());
    if (selected) {
        if (opacity != selectedOpacity) {
            unit->setOpacity(selectedOpacity);
        }
    } else {
        if (opacity != normalOpacity) {
            unit->setOpacity(normalOpacity);
        }
    }
}

void MapLayer::addFireballSpellEffect()
{
    if (_spellRing) {
        static string skyFile("jinenghuoqiu.csb");
        const Point& targetPos = _spellRing->getPosition();
        const Size& winSize = Director::getInstance()->getWinSize();
        Node* skyEffect = addSpellEffect(skyFile, true, Point::ZERO);
        float offsetY(0.0f);
        if (skyEffect->getChildren().size() > 0) {
            Node* sprite = skyEffect->getChildren().at(0);
            offsetY = sprite->getContentSize().height / 2;
        }
        const Point startPos = targetPos + Point(0, _scrollView->convertToNodeSpace(Point(0, winSize.height + offsetY)).y);
        skyEffect->setPosition(startPos);
        skyEffect->runAction(Sequence::create(MoveTo::create(1.0f, targetPos + Point(0, offsetY)), CallFunc::create([=] {
            removeSpellEffect(skyEffect);
            static string groundFile("jinenghuoqiukuosan-1.csb");
            addSpellEffect(groundFile, false, targetPos);
        }), nullptr));
    }
}

void MapLayer::removeAllSpellEffects()
{
    for (auto iter = begin(_spellEffects); iter != end(_spellEffects); ++iter)
    {
        removeSpellEffect(*iter);
    }
}

void MapLayer::addPlaceUnitEffect(const Point& point)
{
    static string file("chuchang-fazhen.csb");
    Node *effect = CSLoader::createNode(file);
    effect->setPosition(convertToScrollViewPoint(point));
    _scrollView->addChild(effect);
    cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
    effect->runAction(action);
    action->gotoFrameAndPlay(0, false);
    action->setLastFrameCallFunc([effect]() {
        effect->removeFromParent();
    });
}

bool MapLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    return true;
}

void MapLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    if (_observer) {
        const Point& point = touch->getLocation();
        _observer->onMapLayerTouchMoved(point);
    }
}

void MapLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_observer) {
        const Point& point = touch->getLocation();
        const Point& startPoint = touch->getStartLocation();
        if (startPoint.distance(point) < TOUCH_CANCEL_BY_MOVING_DISTANCE) {
            _observer->onMapLayerTouchEnded(point);
        }
    }
}

void MapLayer::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
    if (!_isScrolling) {
        _isScrolling = true;
        scrollChecking(0);
    }
}

void MapLayer::scrollViewDidZoom(cocos2d::extension::ScrollView* view)
{
}

#pragma mark - effects
void MapLayer::addParticle(const MapParticleConfigData* data)
{
    const string& name = data->getName();
    Node* effect = nullptr;
    if (name.find(".csb") != string::npos) {
        effect = CSLoader::createNode(name);
        timeline::ActionTimeline *action = CSLoader::createTimeline(name);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, true);
    } else {
        ParticleSystemQuad *particle = ParticleSystemQuad::create(data->getName());
        _particles.insert(particle);
        
        effect = particle;
    }
    
    Point pos = Point(_width * _tileWidth / 2, _height * _tileHeight / 2) + Point(data->getPosX(), data->getPosY());
    effect->setPosition(pos);
    effect->setScaleX(data->getScaleX());
    effect->setScaleY(data->getScaleY());
    _tiledMap->addChild(effect);
}

void MapLayer::removeParticle(ParticleSystemQuad* effect)
{
    if (effect) {
        effect->removeFromParent();
        if (_particles.find(effect) != _particles.end()) {
            _particles.erase(effect);
        }
    }
}

void MapLayer::addButterfly()
{
    if (_butterfly) {
        _butterfly->stopAllActions();
        _butterfly->removeFromParent();
        _butterfly = nullptr;
    }
    
    static const string file("effect-place-2.csb");
    _butterfly = CSLoader::createNode(file);
    _tiledMap->addChild(_butterfly);
    timeline::ActionTimeline *action = CSLoader::createTimeline(file);
    _butterfly->runAction(action);
    action->gotoFrameAndPlay(0, false);
    action->setLastFrameCallFunc([this]() {
        addButterfly();
    });
    
    // generate a random position
    static const float max_x = _width * _tileWidth;
    static const float valid_y[2] = {140.0f, 1070.0f};
    static const float invalid_x[2][2] = {{625, 1155}, {1220, 1665}};
    
    const int random_y_2 = cocos2d::random() % 2;
    const float y = valid_y[random_y_2];
    
    const int random_x_2 = cocos2d::random() % 2;
    const float random_0_1 = CCRANDOM_0_1();
    float x = invalid_x[random_x_2][random_y_2];
    if (random_x_2 == 0) {
        x = random_0_1 * x;
    } else {
        x = x + (max_x - x) * random_0_1;
    }
    
    _butterfly->setPosition(Point(x, y));
}

Node* MapLayer::addSpellEffect(const string& file, bool loop, const Point& position)
{
    if (file.length() > 0) {
        Node *effect = CSLoader::createNode(file);
        effect->setPosition(position);
        _scrollView->addChild(effect);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, loop);
        if (!loop) {
            action->setLastFrameCallFunc([=]() {
                effect->removeFromParent();
                removeSpellEffect(effect);
            });
        }
        
        _spellEffects.insert(effect);
        
        return effect;
    }
    
    return nullptr;
}

void MapLayer::removeSpellEffect(Node* effect)
{
    if (effect) {
        effect->stopAllActions();
        effect->removeFromParent();
        _spellEffects.erase(effect);
    }
}

void MapLayer::scrollChecking(float dt)
{
    const Vec2& offset = _scrollView->getContentOffset();
    const bool show(offset == _scrollViewOffset);
    for (auto iter = begin(_particles); iter != end(_particles); ++iter) {
        (*iter)->setVisible(show);
    }
    
    SEL_SCHEDULE selector = CC_SCHEDULE_SELECTOR(MapLayer::scrollChecking);
    if (show) {
        _isScrolling = false;
        _scheduler->unschedule(selector, this);
    } else {
        _scrollViewOffset = offset;
        if (false == _scheduler->isScheduled(selector, this)) {
            _scheduler->schedule(selector, this, 0.1f, false);
        }
    }
}

Rect MapLayer::getSpellRangeRingBoundingBox() const
{
    if (_spellRing) {
        const Vector<Node*>& children = _spellRing->getChildren();
        if (children.size() > 0) {
            Sprite* sprite = dynamic_cast<Sprite*>(children.at(0));
            if (sprite) {
                Point origin(_spellRing->getPosition());
                const Size& size = sprite->getContentSize();
                origin.x = origin.x - size.width / 2;
                origin.y = origin.y - size.height / 2;
                return Rect(origin, size);
            }
        }
    }
    
    return Rect::ZERO;
}
