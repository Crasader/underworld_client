//
//  MapLayer.cpp
//  Underworld_Client
//
//  Created by burst on 15/10/26.
//
//

#include "MapLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "Constants.h"
#include "LocalHelper.h"
#include "DataManager.h"
#include "MapParticleConfigData.h"
#include "URConfigData.h"
#include "SpellRingNode.h"

using namespace std;

static const int TILEDMAP_TAG(2);
static const int TILEDMAP_ZORDER(2);
static const string TILEDMAP_LAYER_LOGIC("logic");
static const string TILEDMAP_LAYER_FOREGROUND("fg");

MapLayer* MapLayer::create(int mapId)
{
    MapLayer* pRet = new (nothrow) MapLayer();
    if(pRet && pRet->init(mapId)) {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
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
,_mainLayer(nullptr)
,_scrollView(nullptr)
,_butterfly(nullptr)
,_scrollViewOffset(Vec2::ZERO)
,_touchMoved(false)
,_isScrolling(false)
,_selectedUnitMask(nullptr)
{
    clearRingInfo();
}

MapLayer::~MapLayer()
{
    
}

bool MapLayer::init(int mapId)
{
    if (LayerColor::init()) {
        _mapId = mapId;
        const Size& winSize = Director::getInstance()->getWinSize();
        
        //--------- container ----------//
        _scrollView = extension::ScrollView::create();
        _scrollView->setDirection(extension::ScrollView::Direction::BOTH);
        _scrollView->setTouchEnabled(true);
        _scrollView->setBounceable(false);
        _scrollView->setViewSize(winSize);
        _scrollView->setPosition(Point(0, MAP_OFFSET_Y));
        _scrollView->setDelegate(this);
        addChild(_scrollView);
        
        //--------- background ---------//
        float offsetX(0.0f);
        for (int i = 1; ; ++i) {
            string fileName = StringUtils::format("map/bg%d_%d.png", _mapId, i);
            if(FileUtils::getInstance()->isFileExist(fileName)){
                auto bg = Sprite::create(fileName);
                bg->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
                bg->setPosition(Point(offsetX, .0f));
                _scrollView->addChild(bg);
                offsetX += bg->getContentSize().width;
            } else {
                break;
            }
            
            ++i;
        }
        
        //--------- foreground ---------- //
        _tiledMap = experimental::TMXTiledMap::create(StringUtils::format("map/%d.tmx", _mapId));
        _scrollView->addChild(_tiledMap, TILEDMAP_ZORDER, TILEDMAP_TAG);
        _mainLayer = _tiledMap->getLayer(TILEDMAP_LAYER_FOREGROUND);
        
        const Size &mapSize(getMapSize());
        _width = mapSize.width;
        _height = mapSize.height;
        
        const Size &tileSize(_tiledMap->getTileSize());
        _tileWidth = tileSize.width;
        _tileHeight = tileSize.height;
        
        if (_mainLayer) {
            experimental::TMXLayer* layer = dynamic_cast<experimental::TMXLayer*>(_mainLayer);
            if (layer) {
                const Size &ls = layer->getLayerSize();
                for (unsigned int y = 0; y < ls.height; ++y) {
                    int zOrder = getZOrder(ls.height - y);
                    for (unsigned int x = 0; x < ls.width; ++x) {
                        auto tile = layer->getTileAt(Point(x, y));
                        if (tile) {
                            tile->setVisible(false);
                            layer->reorderChild(tile, zOrder);
                        }
                    }
                }
            }
            
            CCLOG("%zd foreground", _mainLayer->getChildren().size());
        } else {
            _mainLayer = Node::create();
            _tiledMap->addChild(_mainLayer);
        }
        
        //--------- logic ---------//
        auto logicLayer = _tiledMap->getLayer(TILEDMAP_LAYER_LOGIC);
        logicLayer->setVisible(false);
        logicLayer->removeFromParent();
        
        //--------- effect ---------//
        if (false) {
            // 1. butterfly
            addButterfly();
            
            // 2. campfire
            {
                static const string file("effect-place-1.csb");
                auto effect = CocosUtils::playAnimation(file, 0, true);
                effect->setPosition(Point(875, 80));
                _tiledMap->addChild(effect);
            }
            
            // 3. smoke
            {
                static const string file("particle/guohuo.plist");
                auto effect = CocosUtils::playAnimation(file, 0, true);
                effect->setPosition(Point(1455, 1020));
                effect->setScale(2.5f);
                _tiledMap->addChild(effect);
            }
            
            // 4. spark
            {
                static const string file("particle/huoxing.plist");
                auto effect = CocosUtils::playAnimation(file, 0, true);
                effect->setPosition(Point(278, 232));
                _tiledMap->addChild(effect);
            }
            
            // 5. fire
            {
                static const string file("effect-fire-1.csb");
                auto effect = CocosUtils::playAnimation(file, 0, true);
                effect->setPosition(Point(285, 245));
                _tiledMap->addChild(effect);
                effect->setScaleX(0.6f);
                effect->setScaleY(1.3f);
            }
        } else if (false) {
            const auto& particles = DataManager::getInstance()->getMapParticleConfigData(_mapId);
            for (int i = 0; i < particles.size(); ++i) {
                addParticle(particles.at(i));
            }
        }
        
        // TODO: remove test code
        if (mapId < 4) {
            const Size &mapPSize = _tiledMap->getContentSize();
            float scale = RESOLUTION_HEIGHT / mapPSize.height;
            _scrollView->setContentSize(mapPSize);
            _scrollView->setMaxScale(Constants::TILEDMAP_MAX_SCALE);
            _scrollView->setMinScale(scale);
            _scrollView->setZoomScale(scale);
            _scrollViewOffset = _scrollView->getContentOffset();
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

#pragma mark - public

void MapLayer::registerObserver(MapLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark getters
const Size& MapLayer::getMapSize() const
{
    if (_tiledMap) {
        return _tiledMap->getMapSize();
    }
    
    return Size::ZERO;
}

Coordinate32 MapLayer::point2Coordinate(const Point& point) const
{
    Point realPoint = _mainLayer->convertToNodeSpace(convertToWorldSpace(point));
    Coordinate32 coordinate;
    coordinate.x = realPoint.x * UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE / _tileWidth;
    coordinate.y = realPoint.y * UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE / _tileHeight;
    return coordinate;
}

#pragma mark unit
void MapLayer::addUnit(Node* unit, const Coordinate32& coordinate)
{
    Point pos;
    int zOrder;
    coordinateConversion(coordinate, pos, zOrder);
    unit->setPosition(pos);
    _mainLayer->addChild(unit, zOrder);
}

void MapLayer::repositionUnit(Node* unit, const Coordinate32& coordinate)
{
    Point pos;
    int zOrder;
    coordinateConversion(coordinate, pos, zOrder);
    if (pos != unit->getPosition()) {
        unit->setPosition(pos);
        reorderChild(unit, zOrder);
    }
}

#pragma mark effects
void MapLayer::addBulletExplosionEffect(const string& name, const Coordinate32& coordinate)
{
    string file;
    if (name.find(SPELL_NAME_FIREBALL) != string::npos) {
        file = "jinenghuoqiukuosan-1.csb";
    }
    
    if (file.length() > 0) {
        const Point& point = coordinate2Point(coordinate);
        addSpellEffect(file, false, point);
        removeStaticRing(point);
    }
}

void MapLayer::addUnitPlacementEffect(const Coordinate32& coordinate)
{
    const Point& pos = coordinate2Point(coordinate);
    static string file("chuchang-fazhen.csb");
    Node* effect = CocosUtils::playAnimation(file, 0, false, 0, -1, [](Node* sender) {
        sender->removeFromParent();
    });
    effect->setPosition(pos);
    _mainLayer->addChild(effect);
}

#pragma mark spells
void MapLayer::addSpell(int card, float duration)
{
    auto ring(_spellRing.second);
    if (ring) {
        auto staticRing = createRing(card, duration);
        if (staticRing) {
            staticRing->setPosition(ring->getPosition());
            staticRing->setScale(ring->getScale());
        }
    }
}

void MapLayer::addAoeSpell(const Point& startPoint, int card, float duration)
{
    auto ring = _spellRing.second;
    if (ring) {
        const Point& targetPos = ring->getPosition();
        if (card == SPELL_NAME_FIREBALL) {
#if false
            Node* node = Node::create();
            node->setPosition(startPoint);
            _mainLayer->addChild(node, topZOrder);
            
            // effects
            {
                static const string file("particle/yan.plist");
                auto effect = CocosUtils::playAnimation(file, 0, true);
                effect->setPosition(-40, 0);
                node->addChild(effect);
            }
            {
                static const string file("particle/huo.plist");
                auto effect = CocosUtils::playAnimation(file, 0, true);
                node->addChild(effect);
            }
            {
                static const string file("huoqiu.csb");
                auto effect = CocosUtils::playAnimation(file, 0, true);
                node->addChild(effect);
            }
            
            node->runAction(Sequence::create(Parabola::create(3, startPoint, targetPos, 300), CallFunc::create([=] {
                node->removeFromParent();
                static string groundFile("jinenghuoqiukuosan-1.csb");
                addSpellEffect(groundFile, false, targetPos);
                removeStaticRing(targetPos);
            }), nullptr));
#elif true
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
        auto staticRing = createRing(card, 0);
        staticRing->setPosition(targetPos);
        staticRing->setScale(ring->getScale());
        if (_staticSpellRings.find(targetPos) == _staticSpellRings.end()) {
            _staticSpellRings.insert(make_pair(targetPos, ring));
        } else {
            assert(false);
        }
    }
}

void MapLayer::removeAllSpellEffects()
{
    for (auto effect: _spellEffects) {
        removeSpellEffect(effect);
    }
}

#pragma mark unit mask
void MapLayer::updateUnitMask(const UnitType* unitType, const Coordinate32& coordinate)
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
        _selectedUnitMask->removeFromParent();
        _selectedUnitMask = nullptr;
        _selectedUnitName = "";
    }
}

#pragma mark spell ring
void MapLayer::updateSpellRing(int cardId, const Coordinate32& coordinate, int range)
{
    auto ring = _spellRing.second;
    if (!ring) {
        ring = createRing(cardId, 0);
    } else if (cardId != _spellRing.first) {
        removeSpellRing();
        ring = createRing(cardId, 0);
    }
    
    if (ring) {
        const Point& point = coordinate2Point(coordinate);
        ring->setPosition(point);
        
        if (cardId != _spellRing.first || ring != _spellRing.second) {
            _spellRing.first = cardId;
            _spellRing.second = ring;
        }
        
        // calculate scale
        static const float defaultRange(218);
        if (range != SpellType::CAST_DISTANCE_INFINITE) {
            const float scale = range / defaultRange;
            ring->setScale(scale);
        }
    } else {
        clearRingInfo();
    }
}

void MapLayer::removeSpellRing()
{
    auto ring = _spellRing.second;
    if (ring) {
        ring->removeFromParent();
    }
    
    clearRingInfo();
}

void MapLayer::checkUnitWithSpellRing(Node* unit)
{
    static GLubyte selectedOpacity(180);
    static GLubyte normalOpacity(255);
    bool selected(false);
    auto ring = _spellRing.second;
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

#pragma mark areas
void MapLayer::setPlacedArea(float x1, float x2)
{
    removeUnplacedAreas();
    
    if (fabs(x1 - x2) > 0) {
        Point point = coordinate2Point(Coordinate32(x1, 0));
        x1 = point.x;
        
        point = coordinate2Point(Coordinate32(x2, 0));
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

#pragma mark LayerColor
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

#pragma mark ScrollViewDelegate
void MapLayer::scrollViewDidScroll(extension::ScrollView* view)
{
    
}

void MapLayer::scrollViewDidZoom(extension::ScrollView* view)
{
    
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
        if (!_scheduler->isScheduled(selector, this)) {
            _scheduler->schedule(selector, this, 0.1f, false);
        }
    }
}

#pragma mark Coordinates
Coordinate32 MapLayer::mapCoordinate2coreCoordinate(int x, int y)
{
    return Coordinate32(x, (_height - 1) - y);
}

Point MapLayer::coordinate2Point(const Coordinate32& coordinate)
{
    const float x = _tileWidth / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coordinate.x;
    const float y = _tileHeight / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coordinate.y;
    return Point(x, y);
}

int MapLayer::getZOrder(int coordinateY) const
{
    return 2 * (_height - coordinateY + 1);
}

void MapLayer::coordinateConversion(const Coordinate32& coordinate, Point& mapPosition, int& zOrder)
{
    mapPosition = coordinate2Point(coordinate);
    zOrder = getZOrder(coordinate.y);
}

#pragma mark effects
void MapLayer::addParticle(const MapParticleConfigData* data)
{
    const string& name = data->getName();
    Node* effect = CocosUtils::playAnimation(name, 0, true);
    if (".plist" == FileUtils::getInstance()->getFileExtension(name)) {
        auto particle = dynamic_cast<ParticleSystemQuad*>(effect);
        _particles.insert(particle);
    }
    
    auto pos = Point(_width * _tileWidth / 2, _height * _tileHeight / 2) + Point(data->getPosX(), data->getPosY());
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

#pragma mark spells
Node* MapLayer::addSpellEffect(const string& file, bool loop, const Point& position)
{
    if (file.length() > 0) {
        Node* effect(nullptr);
        if (loop) {
            effect = CocosUtils::playAnimation(file, 0, loop);
        } else {
            effect = CocosUtils::playAnimation(file, 0, loop, 0, -1, [this](Node* sender) {
                sender->removeFromParent();
                removeSpellEffect(sender);
            });
        }
        effect->setPosition(position);
        _scrollView->addChild(effect);
        _spellEffects.insert(effect);
        
        return effect;
    }
    
    return nullptr;
}

void MapLayer::removeSpellEffect(Node* effect)
{
    if (effect) {
        effect->removeFromParent();
        _spellEffects.erase(effect);
    }
}

void MapLayer::addButterfly()
{
    if (_butterfly) {
        _butterfly->removeFromParent();
        _butterfly = nullptr;
    }
    
    static const string file("effect-place-2.csb");
    _butterfly = CocosUtils::playAnimation(file, 0, false, 0, -1, [this](Node* sender) {
        addButterfly();
    });
    _tiledMap->addChild(_butterfly);
    
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

#pragma mark unit mask
Node* MapLayer::createUnitMask(const UnitType* ut) const
{
    if (ut) {
        string file;
        const auto& name = ut->getRenderKey();
        auto data = DataManager::getInstance()->getURConfigData(name);
        if (data) {
            static const int resourceId(2);
            auto uc = ut->getUnitClass();
            if (data->getAnimationType() == UnitAnimationType::PVR) {
                file = data->getNormalPrefix() + StringUtils::format("/stand/body/%d", resourceId);
            } else {
                if (kUnitClass_Core == uc || kUnitClass_Building == uc) {
                    file = StringUtils::format(data->getNormalPrefix().c_str(), resourceId);
                } else {
                    file = data->getNormalPrefix() + StringUtils::format("-standby-%d.csb", resourceId);
                }
            }
            
            if (file.size() > 0) {
                auto actionNode = CocosUtils::playAnimation(file, 0, false);
                Sprite* sprite(nullptr);
                if (data->getAnimationType() == UnitAnimationType::PVR) {
                    sprite = dynamic_cast<Sprite*>(actionNode);
                } else {
                    sprite = dynamic_cast<Sprite*>(actionNode->getChildren().front());
                }
                
                if (sprite) {
                    sprite->setOpacity(180);
                    
                    Point basePoint(Point::ZERO);
                    if (data->getAnimationType() == UnitAnimationType::PVR) {
                        basePoint.x = sprite->getContentSize().width / 2;
                    } else {
                        basePoint = sprite->getPosition();
                    }
                    
                    const Point& point = basePoint + Point(data->getHpBarPosX(), sprite->getContentSize().height / 2 + data->getHpBarPosY());
                    
                    auto node = Node::create();
                    actionNode->addChild(node);
                    
                    auto levelLabel = CocosUtils::createLabel(StringUtils::format(LocalHelper::getString("hint_level").c_str(), 1), BIG_FONT_SIZE);
                    levelLabel->setPosition(point + Point(0, 10));
                    node->addChild(levelLabel);
                    
                    auto nameLabel = CocosUtils::createLabel(ut->getName(), BIG_FONT_SIZE);
                    nameLabel->setPosition(point + Point(0, 35));
                    node->addChild(nameLabel);
                }
                
                return actionNode;
            }
        }
    }
    
    return nullptr;
}

#pragma mark spell ring
Node* MapLayer::createRing(int cardId, float duration)
{
    // get file name
    string file;
    if (cardId == SPELL_NAME_FIREBALL) {
        file.assign("quan-1.csb");
    } else if (cardId == SPELL_NAME_CURE) {
        file.assign("huixue-xin.csb");
    } else if (cardId == SPELL_NAME_SPEEDUP) {
        file.assign("jiasu-xin.csb");
    } else if (cardId > 0) {
        file.assign("quan-2.csb");
    }
    
    if (!file.empty()) {
        auto r = SpellRingNode::create(file, duration, [](Node* ring){
            ring->removeFromParent();
        });
        _mainLayer->addChild(r, -2000);
        return r;
    }
    
    return nullptr;
}

void MapLayer::removeStaticRing(const Point& point)
{
    if (_staticSpellRings.find(point) != _staticSpellRings.end()) {
        auto ring = _staticSpellRings.at(point);
        if (ring) {
            ring->removeFromParent();
        }
        
        _staticSpellRings.erase(point);
    }
}

void MapLayer::clearRingInfo()
{
    _spellRing.first = 0;
    _spellRing.second = nullptr;
}

Rect MapLayer::getSpellRingBoundingBox() const
{
    auto ring = _spellRing.second;
    if (ring) {
        const auto& children = ring->getChildren();
        if (children.size() > 0) {
            auto sprite = dynamic_cast<Sprite*>(children.front());
            if (sprite) {
                Point origin(ring->getPosition());
                const auto& size = sprite->getContentSize() * ring->getScale();
                origin.x = origin.x - size.width / 2;
                origin.y = origin.y - size.height / 2;
                return Rect(origin, size);
            }
        }
    }
    
    return Rect::ZERO;
}

#pragma mark areas
void MapLayer::addUnplacedArea(float x1, float x2)
{
    const float width = fabs(x1 - x2);
    if (width > 0) {
        static string file("GameImages/test/ui_hongse.png");
        static const Rect rect(0, 0, 326, 326);
        static const Rect capInsets(160, 160, 6, 6);
        auto sprite = ui::Scale9Sprite::create(file, rect, capInsets);
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
    for (auto node : _unplacedAreas) {
        node->removeFromParent();
    }
    
    _unplacedAreas.clear();
}

void MapLayer::addBuildingPlacedAreas()
{
    if (0 == _buildingPlacedAreas.size()) {
        static const string file("GameImages/test/ui_quyu.png");
        auto s = Sprite::create(file);
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

#pragma mark tiles
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
                    auto s = Sprite::create(wFile);
                    s->setPosition(x, y);
                    wbn->addChild(s);
                } else {
                    auto s = Sprite::create(bFile);
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
