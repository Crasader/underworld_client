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
#include "UnitType.h"
#include "Map.h"
#include "cocostudio/CocoStudio.h"
#include "tinyxml2/tinyxml2.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "CoreUtils.h"
#include "Constants.h"
#include "DataManager.h"
#include "MapParticleConfigData.h"
#include "GameConstants.h"
#include "URConfigData.h"
#include "CCParabola.h"

using namespace cocostudio;
using namespace std;

static const int TILEDMAP_TAG = 2;
static const int TILEDMAP_ZORDER = 2;
static const string TILEDMAP_LAYER_LOGIC = "logic";
static const string TILEDMAP_LAYER_FOREGROUND = "fg";

static Node* createUnitMask(const UnderWorld::Core::UnitType* ut)
{
    if (ut) {
        string file;
        const string& name = ut->getRenderKey();
        const URConfigData* data = DataManager::getInstance()->getURConfigData(name);
        if (data) {
            static const int resourceId(2);
            UnderWorld::Core::UnitClass uc = ut->getUnitClass();
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
                    
                    {
                        const Point& point = sprite->getPosition() + Point(data->getHpBarPosX(), sprite->getContentSize().height / 2 + data->getHpBarPosY());
                        
                        Node* node = Node::create();
                        actionNode->addChild(node);
                        
                        Label* levelLabel = CocosUtils::createLabel(StringUtils::format(LocalHelper::getString("hint_level").c_str(), 1), BIG_FONT_SIZE);
                        levelLabel->setPosition(point + Point(0, 10));
                        node->addChild(levelLabel);
                        
                        Label* nameLabel = CocosUtils::createLabel(ut->getName(), BIG_FONT_SIZE);
                        nameLabel->setPosition(point + Point(0, 35));
                        node->addChild(nameLabel);
                    }
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
,_butterfly(nullptr)
,_touchMoved(false)
,_isScrolling(false)
,_selectedUnitMask(nullptr)
{
    clearRingInfo();
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
        _scrollView->setPosition(Point(0, MAP_OFFSET_Y));
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
            const vector<MapParticleConfigData*>& particles = DataManager::getInstance()->getMapParticleConfigData(_mapId);
            for (int i = 0; i < particles.size(); ++i) {
                addParticle(particles.at(i));
            }
        }

#if false
        addMapTiles();
        addBuildingPlacedAreas();
#endif
        
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

UnderWorld::Core::Coordinate32 MapLayer::mapCoordinate2coreCoordinate(int x, int y)
{
    //fix:y is [0 - (_height - 1)]
    return UnderWorld::Core::Coordinate32(x, (_height - 1) - y);
}

Point MapLayer::coordinate2Point(const UnderWorld::Core::Coordinate32& coordinate)
{
    const float x = _tileWidth / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coordinate.x;
    const float y = _tileHeight / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coordinate.y;
    return Point(x, y);
}

int MapLayer::calcZOrder(int coreCoordinateY)
{
    return 2 * (_height - coreCoordinateY + 1);
}

void MapLayer::coordinateConvert(const UnderWorld::Core::Coordinate32& coordinate, Point& mapPosition, int& zOrder)
{
    mapPosition = coordinate2Point(coordinate);
    zOrder = calcZOrder(coordinate.y);
}

UnderWorld::Core::Coordinate32 MapLayer::convertPoint(const Point& layerPoint)
{
    Point realPoint = _mainLayer->convertToNodeSpace(convertToWorldSpace(layerPoint));
    UnderWorld::Core::Coordinate32 coordinate;
    coordinate.x = realPoint.x * UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE / _tileWidth;
    coordinate.y = realPoint.y * UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE / _tileHeight;
    return coordinate;
}

void MapLayer::addUnit(Node* unit, const UnderWorld::Core::Coordinate32& coordinate)
{
    Point pos;
    int zOrder;
    coordinateConvert(coordinate, pos, zOrder);
    unit->setPosition(pos);
    _mainLayer->addChild(unit, zOrder);
}

void MapLayer::repositionUnit(Node* unit, const UnderWorld::Core::Coordinate32& coordinate)
{
    Point pos;
    int zOrder;
    coordinateConvert(coordinate, pos, zOrder);
    if (pos == unit->getPosition()) {
        return;
    }
    unit->setPosition(pos);
    reorderChild(unit, zOrder);
}

void MapLayer::addBulletExplosionEffect(const string& name, const UnderWorld::Core::Coordinate32& coordinate)
{
    string file;
    if (name.find(SPELL_NAME_FIREBALL) != string::npos) {
        file = "jinenghuoqiukuosan-1.csb";
    }
    
    const Point& point = coordinate2Point(coordinate);
    addSpellEffect(file, false, point);
    removeStaticRing(point);
}

void MapLayer::updateUnitMask(const UnderWorld::Core::UnitType* unitType, const UnderWorld::Core::Coordinate32& coordinate)
{
    if (unitType) {
        const string& name = unitType->getRenderKey();
        if (_selectedUnitName != name) {
            removeUnitMask();
        }
        
        if (!_selectedUnitMask) {
            _selectedUnitName = name;
            _selectedUnitMask = createUnitMask(unitType);
            _mainLayer->addChild(_selectedUnitMask);
        }
        
        if (_selectedUnitMask) {
            const Point& point = coordinate2Point(coordinate);
            _selectedUnitMask->setPosition(point);
        }
    } else {
        removeUnitMask();
    }
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

void MapLayer::updateSpellRing(const string& name, const UnderWorld::Core::Coordinate32& coordinate, int range)
{
    const Point& point = coordinate2Point(coordinate);
    Node* ring = _spellRing.second;
    if (!ring) {
        ring = createRing(name, point);
    } else {
        if (name != _spellRing.first) {
            removeSpellRing();
            ring = createRing(name, point);
        } else {
            ring->setPosition(point);
        }
    }
    
    if (ring) {
        if (name != _spellRing.first || ring != _spellRing.second) {
            _spellRing.first = name;
            _spellRing.second = ring;
        }
        
        // calculate scale
        static const float defaultRange(218);
        if (range != UnderWorld::Core::SpellType::CAST_DISTANCE_INFINITE) {
            const float scale = range / defaultRange;
            ring->setScale(scale);
        }
    } else {
        clearRingInfo();
    }
}

void MapLayer::removeSpellRing()
{
    Node* ring = _spellRing.second;
    if (ring) {
        ring->stopAllActions();
        ring->removeFromParent();
    }
    
    clearRingInfo();
}

void MapLayer::checkUnitInSpellRing(Node* unit)
{
    static GLubyte selectedOpacity(180);
    static GLubyte normalOpacity(255);
    bool selected(false);
    Node* ring = _spellRing.second;
    if (ring && getSpellRingBoundingBox().containsPoint(unit->getPosition())) {
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

void MapLayer::addAoeSpell(const Point& startPoint, const string& name, float duration)
{
    Node* ring = _spellRing.second;
    if (ring) {
        const Point& targetPos = ring->getPosition();
        if (name.find(SPELL_NAME_FIREBALL) != string::npos) {
#if false
            Node* node = Node::create();
            node->setPosition(startPoint);
            _mainLayer->addChild(node, topZOrder);
            
            // effects
            {
                ParticleSystemQuad *effect = ParticleSystemQuad::create("particle/yan.plist");
                effect->setPosition(-40, 0);
                node->addChild(effect);
            }
            {
                ParticleSystemQuad *effect = ParticleSystemQuad::create("particle/huo.plist");
                node->addChild(effect);
            }
            {
                static const string file("huoqiu.csb");
                Node *effect = CSLoader::createNode(file);
                node->addChild(effect);
                cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                effect->runAction(action);
                action->gotoFrameAndPlay(0, true);
            }
            
            node->runAction(Sequence::create(Parabola::create(3, startPoint, targetPos, 300), CallFunc::create([=] {
                node->removeFromParent();
                static string groundFile("jinenghuoqiukuosan-1.csb");
                addSpellEffect(groundFile, false, targetPos);
                removeStaticRing(targetPos);
            }), nullptr));
#elif false
            const Size& winSize = Director::getInstance()->getWinSize();
            static string skyFile("jinenghuoqiu.csb");
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
                removeStaticRing(targetPos);
            }), nullptr));
#endif
        }
        
        // add a spell ring which will be removed when animation finished
        const float scale(ring->getScale());
        Node* ring = createRing(name, targetPos);
        ring->setScale(scale);
        if (_staticSpellRings.find(targetPos) == _staticSpellRings.end()) {
            _staticSpellRings.insert(make_pair(targetPos, ring));
        } else {
            assert(false);
        }
    }
}

void MapLayer::addSpell(const string& name, float duration)
{
    Node* ring = _spellRing.second;
    if (ring) {
        const Point& targetPos = ring->getPosition();
        static const string key("RemoveSpellRing");
        Scheduler* scheduler = Director::getInstance()->getScheduler();
        scheduler->schedule([=](float) {
            scheduler->unschedule(key, this);
            removeStaticRing(targetPos);
        }, this, duration, false, key);
        
        // add a spell ring which will be removed when animation finished
        const float scale(ring->getScale());
        Node* ring = createRing(name, targetPos);
        ring->setScale(scale);
        if (_staticSpellRings.find(targetPos) == _staticSpellRings.end()) {
            _staticSpellRings.insert(make_pair(targetPos, ring));
        } else {
            assert(false);
        }
    }
}

void MapLayer::removeAllSpellEffects()
{
    for (auto iter = begin(_spellEffects); iter != end(_spellEffects); ++iter)
    {
        removeSpellEffect(*iter);
    }
}

void MapLayer::addPlaceUnitEffect(const UnderWorld::Core::Coordinate32& coordinate)
{
    const Point& pos = coordinate2Point(coordinate);
    static string file("chuchang-fazhen.csb");
    Node *effect = CSLoader::createNode(file);
    effect->setPosition(pos);
    _mainLayer->addChild(effect);
    cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
    effect->runAction(action);
    action->gotoFrameAndPlay(0, false);
    action->setLastFrameCallFunc([effect]() {
        effect->removeFromParent();
    });
}

void MapLayer::setPlacedArea(float x1, float x2)
{
    removeUnplacedAreas();
    
    if (fabs(x1 - x2) > 0) {
        Point point = coordinate2Point(UnderWorld::Core::Coordinate32(x1, 0));
        x1 = point.x;
        
        point = coordinate2Point(UnderWorld::Core::Coordinate32(x2, 0));
        x2 = point.x;
        
        const float width = _scrollView->getContentSize().width;
        const float min = MAX(0, MIN(x1, x2));
        const float max = MIN(width, MAX(x1, x2));
        if (min > 0) {
            addUnplacedArea(0, min);
        }
        
        if (max < width) {
            addUnplacedArea(max, width);
        }
    }
}

#pragma mark - protected
bool MapLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    if (_observer) {
        _observer->onMapLayerTouchBegan(touch->getLocation());
    }
    
    return true;
}

void MapLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    const Point& point = touch->getLocation();
    const Point& startPoint = touch->getStartLocation();
    if (!_touchMoved && startPoint.distance(point) > TOUCH_CANCEL_BY_MOVING_DISTANCE) {
        _touchMoved = true;
    }
    
    if (_observer) {
        _observer->onMapLayerTouchMoved(point, !_touchMoved);
    }
}

void MapLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_touchMoved) {
        _touchMoved = false;
    } else if (_observer) {
        _observer->onMapLayerTouchEnded(touch->getLocation());
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

void MapLayer::clearRingInfo()
{
    _spellRing.first = "";
    _spellRing.second = nullptr;
}

Node* MapLayer::createRing(const string& name, const Point& point)
{
    string fileName;
    if (name.find(SPELL_NAME_FIREBALL) != string::npos) {
        fileName.assign("quan-1.csb");
    } else if (name.find(SPELL_NAME_CURE) != string::npos) {
        fileName.assign("huixue-xin.csb");
    } else if (name.find(SPELL_NAME_SPEEDUP) != string::npos) {
        fileName.assign("jiasu-xin.csb");
    }
    
    if (fileName.length() > 0) {
        Node* ring = CSLoader::createNode(fileName);
        timeline::ActionTimeline *action = CSLoader::createTimeline(fileName);
        ring->runAction(action);
        action->gotoFrameAndPlay(0, true);
        ring->setPosition(point);
        _mainLayer->addChild(ring);
        return ring;
    }
    
    return nullptr;
}

void MapLayer::removeStaticRing(const Point& point)
{
    if (_staticSpellRings.find(point) != _staticSpellRings.end()) {
        Node* ring = _staticSpellRings.at(point);
        if (ring) {
            ring->removeFromParent();
        }
        
        _staticSpellRings.erase(point);
    }
}

Rect MapLayer::getSpellRingBoundingBox() const
{
    Node* ring = _spellRing.second;
    if (ring) {
        const Vector<Node*>& children = ring->getChildren();
        if (children.size() > 0) {
            Sprite* sprite = dynamic_cast<Sprite*>(children.at(0));
            if (sprite) {
                Point origin(ring->getPosition());
                const Size& size = sprite->getContentSize();
                origin.x = origin.x - size.width / 2;
                origin.y = origin.y - size.height / 2;
                return Rect(origin, size);
            }
        }
    }
    
    return Rect::ZERO;
}

void MapLayer::addUnplacedArea(float x1, float x2)
{
    const float width = fabs(x1 - x2);
    if (width > 0) {
        static string file("GameImages/test/ui_hongse.png");
        static const Rect rect(0, 0, 326, 326);
        static const Rect capInsets(160, 160, 6, 6);
        ui::Scale9Sprite* sprite = ui::Scale9Sprite::create(file, rect, capInsets);
        _scrollView->addChild(sprite);
        
        const float height = _scrollView->getContentSize().height;
        sprite->setContentSize(Size(width, height));
        sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        sprite->setPosition(MIN(x1, x2), 0);
        _unplacedAreas.push_back(sprite);
    }
}

void MapLayer::removeUnplacedAreas()
{
    if (_unplacedAreas.size() > 0) {
        for (auto node : _unplacedAreas) {
            node->removeFromParent();
        }
        
        _unplacedAreas.clear();
    }
}

void MapLayer::addBuildingPlacedAreas()
{
    if (0 == _buildingPlacedAreas.size()) {
        static const string file("GameImages/test/ui_quyu.png");
        Sprite* s = Sprite::create(file);
        s->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        _mainLayer->addChild(s);
        _buildingPlacedAreas.push_back(s);
    }
}

void MapLayer::removeBuildingPlacedAreas()
{
    for (auto s : _buildingPlacedAreas) {
        s->removeFromParent();
    }
    
    _buildingPlacedAreas.clear();
}

void MapLayer::addMapTiles()
{
    if (0 == _tilesBatchNodes.size()) {
        static const unsigned int cnt(250);
        static const string bFile("GameImages/test/ui_gezi.png");
        static const string wFile("GameImages/test/ui_gezi_1.png");
        
        auto bbn = SpriteBatchNode::create(bFile, cnt);
        _mainLayer->addChild(bbn);
        _tilesBatchNodes.push_back(bbn);
        
        auto wbn = SpriteBatchNode::create(wFile, cnt);
        _mainLayer->addChild(wbn);
        _tilesBatchNodes.push_back(wbn);
        
        const Size& size = Sprite::create(bFile)->getContentSize();
        const float width = _width * _tileWidth;
        const float height = _height * _tileHeight;
        for (int i = 0; i < ceil(width / size.width); ++i) {
            for (int j = 0; j < ceil(height / size.height); ++j) {
                const float x = (i + 0.5f) * size.width;
                const float y = (j + 0.5f) * size.height;
                if ((i + j) % 2 == 0) {
                    Sprite* s = Sprite::create(wFile);
                    s->setPosition(x, y);
                    wbn->addChild(s);
                } else {
                    Sprite* s = Sprite::create(bFile);
                    s->setPosition(x, y);
                    bbn->addChild(s);
                }
            }
        }
    }
}

void MapLayer::removeMapTiles()
{
    for (auto bn : _tilesBatchNodes) {
        bn->removeAllChildren();
        bn->removeFromParent();
    }
    
    _tilesBatchNodes.clear();
}
