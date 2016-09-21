//
//  WorldRender.cpp
//  Underworld_Client
//
//  Created by Mac on 16/6/23.
//
//

#include <cmath>

#include "ui/CocosGUI.h"
#include "CocosUtils.h"
#include "CocosGlobal.h"
#include "WorldRender.h"
#include "WorldObjectRender.h"
#include "WorldObject.h"
#include "DataManager.h"
#include "MapParticleConfigData.h"
#include "Map.h"
#include "CoreUtils.h"
#include "UnitRender.h"
#include "BulletRender.h"
#include "SpellType.h"
#include "SpellConfigData.h"
#include "GameModeHMM.h"
#include "CardConfigData.h"
#include "RenderHelper.h"
#include "UWRender.h"

USING_NS_CC;

namespace UnderWorld{ namespace Core{
    
// =====================================================
// 	class WorldRender
// =====================================================
    
// ===================== public ========================
    
// ===================== consts ========================
    
const std::string WorldRender::TILEDMAP_LAYER_FOREGROUND_NAME("fg");
const std::string WorldRender::TILEDMAP_LAYER_LOGIC_NAME("logic");

// ===================== init ========================
    
WorldRender::WorldRender()
: _mapId(0)
, _world(nullptr)
, _pixelPerMapsizeX(0.f)
, _pixelPerMapsizeY(0.f)
, _cameraAngelDegrees(0)
, _cameraAngelRadians(0.f)
, _worldHeight(0)
, _worldWidth(0)
, _worldLayer(nullptr)
, _worldContainer(nullptr)
, _tiledMap(nullptr)
, _hmmCardRegionTipView(nullptr)
, _hmmCardPlaceTipView(nullptr)
, _hmmCardPlaceTipViewShadow(nullptr)
, _tipCardType(nullptr) {
}
    
    
WorldRender::~WorldRender() {
    if (_world) _world->removeWorldObserver(this);
    deleteAllWorldObjectRender();
    recycle();
    CC_SAFE_RELEASE(_worldLayer);
}
    
bool WorldRender::init(const World *world, const UWRender* gameRender) {
    //clean up old instances
    deleteAllWorldObjectRender();
    _events.clear();
    _mapParticles.clear();
    if (_worldLayer) _worldLayer->removeAllChildren();
    _worldContainer = _tiledMap = nullptr;
    
    _world = world;
    _gameRender = gameRender;
    
    if (!_world) return false;
    
    //init world map & environment
    if (!initWorldEnvironment()) return false;
    
    //init world objects
    for (int i = 0; i < _world->getObjectCount(); ++i) {
        if (_world->getWorldObject(i)) {
            createWorldObjectRender(_world->getWorldObject(i));
        }
    }
    
    _world->addWorldObserver(this);
    
    return true;
}
    
// ===================== override WorldObserver ========================
    
void WorldRender::onNotifyWorldEvents(const std::vector<World::EventLog>& events) {
    //save concerned event log
    for (int i = 0; i < events.size(); ++i) {
        _events.push_back(events[i]);
    }
}
    
void WorldRender::onNotifyWorldReleased() {
    _world = nullptr;
    deleteAllWorldObjectRender();
}
    
// ===================== interface ========================
    
void WorldRender::render(const World *world) {
    // if world is not released
    if (_world) {
        //handle concerned event log
        handleEvents();
        
        //render all objects
        std::unordered_map<creatureid_t, AbstractWorldObjectRender*> clone = _objectRenders;
        for (auto iter = clone.begin(); iter != clone.end(); ++iter) {
            iter->second->render();
        }
    }
    
    //recycle dead object renders
    recycle();
}
    
void WorldRender::deleteWorldObjectRender(const AbstractWorldObjectRender *objectRender) {
    assert(objectRender);
    
    if (!objectRender) return;
    
    creatureid_t id = objectRender->getObjectId();
    auto iter = _objectRenders.find(id);
    if (iter == _objectRenders.end()) return;
  
    iter->second->dettachFromWorldRender(this);
    _recycleBin.push_back(iter->second);
    _objectRenders.erase(iter);
}
    
cocos2d::Vec2 WorldRender::worldCoordinate2CocosPoint(const Coordinate32& pos,
    RenderLayer renderlayer, int32_t height) {
    
    Vec2 ret;
    if (renderlayer == RenderLayer::Ground || renderlayer == RenderLayer::Shadow) {
        ret.x = pos.x * _pixelPerMapsizeX;
        ret.y = pos.y * _pixelPerMapsizeY;
    } else if (renderlayer == RenderLayer::Land) {
        ret.x = pos.x * _pixelPerMapsizeX;
        ret.y = (pos.y + height / tanf(_cameraAngelRadians)) * _pixelPerMapsizeY;
    } else if (renderlayer == RenderLayer::Air) {
        ret.x = pos.x * _pixelPerMapsizeX;
        ret.y = (pos.y + (RENDER_LAYER_AIR_HEIGHT_IN_MAP_SIZE + height) / tanf(_cameraAngelRadians)) * _pixelPerMapsizeY;
    }
    
    return ret;
}
    
int WorldRender::worldCoordinate2Zorder(const Coordinate32 &pos,
    RenderLayer renderLayer,
    int32_t height) {
    return (_worldHeight * (int)renderLayer) +
        (_worldHeight - pos.y);
}
    
Coordinate32 WorldRender::cocosPoint2WorldCoordinate(const cocos2d::Vec2& pos) {
    return Coordinate32(pos.x / _pixelPerMapsizeX, pos.y / _pixelPerMapsizeY);
}

void WorldRender::showHMMCardRegionTips(const HMMCardType* cardType,
    const Rect32& summonRegion, const Rect32& towerRegion) {
    if (!cardType) return;
    
    hideHMMCardRegionTips();
    if (cardType->getCardClass() == kHMMCardClass_Summon
        || cardType->getCardClass() == kHMMCardClass_Tower
        || cardType->getCardClass() == kHMMCardClass_Hero) {
        const Rect32& region = cardType->getCardClass() == kHMMCardClass_Tower ? towerRegion : summonRegion;
        static string file("GameImages/battle_ui/card_region_tips.png");
        static const Rect rect(0, 0, 39, 37);
        static const Rect capInsets(4, 5, 30, 27);
        _hmmCardRegionTipView = cocos2d::ui::Scale9Sprite::create(file, rect, capInsets);
        if (_hmmCardRegionTipView) {
            int zorder = worldCoordinate2Zorder(region._origin, RenderLayer::Ground);
            cocos2d::Vec2 leftDown = worldCoordinate2CocosPoint(region._origin, RenderLayer::Ground);
            cocos2d::Vec2 rightUp = worldCoordinate2CocosPoint(Coordinate32(region.getMaxX(), region.getMaxY()), RenderLayer::Ground);
            _hmmCardRegionTipView->setPosition(leftDown);
            _hmmCardRegionTipView->setContentSize(cocos2d::Size(abs(leftDown.x - rightUp.x), abs(leftDown.y - rightUp.y)));
            _hmmCardRegionTipView->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
            _worldContainer->addChild(_hmmCardRegionTipView, zorder);
        }
        
    }
}
    
void WorldRender::hideHMMCardRegionTips() {
    if (_hmmCardRegionTipView) {
        _hmmCardRegionTipView->removeFromParent();
        _hmmCardRegionTipView = nullptr;
    }
}
    
void WorldRender::showHMMCardPlaceTips(const HMMCardType* cardType, const Coordinate32& pos, bool enable) {
    if (!cardType) return;
    
    if (_hmmCardPlaceTipView && (!_tipCardType || _tipCardType != cardType)) {
        hideHMMCardPlaceTips();
    }
    
    if (!_hmmCardPlaceTipView) {
        _tipCardType = cardType;
        createHMMCardPlaceTipsView(_tipCardType, _world,
            _hmmCardPlaceTipView, _hmmCardPlaceTipViewShadow, _gameRender->isSwaped());
        
        if (_hmmCardPlaceTipView) {
            this->getWorldContainer()->addChild(_hmmCardPlaceTipView);
            if (_hmmCardPlaceTipViewShadow) this->getWorldContainer()->addChild(_hmmCardPlaceTipViewShadow);
        }
    }
    
    if (_hmmCardPlaceTipView) {
        if (_tipCardType->getCardClass() == kHMMCardClass_Hero
            || _tipCardType->getCardClass() == kHMMCardClass_Summon
            || _tipCardType->getCardClass() == kHMMCardClass_Tower) {
            const UnitType* ut = _world->getTechTree()->findUnitTypeById(_tipCardType->getUnitSetting().getUnitTypeId());
            int size = ut->getSize();
            Coordinate32 centerPos = pos + Coordinate32(size / 2, size / 2);
            
            WorldRender::RenderLayer renderLayer(WorldRender::RenderLayer::Land);
            if (ut->getDefaultField() == kFieldType_Land) {
                renderLayer = WorldRender::RenderLayer::Land;
            } else if (ut->getDefaultField() == kFieldType_Air) {
                renderLayer = WorldRender::RenderLayer::Air;
            }
            
            int zorder = this->worldCoordinate2Zorder(pos + Coordinate32(size, 0), renderLayer);
            cocos2d::Vec2 point = this->worldCoordinate2CocosPoint(centerPos, renderLayer);
            
            _hmmCardPlaceTipView->setLocalZOrder(zorder);
            _hmmCardPlaceTipView->setPosition(point);
            
            if (_hmmCardPlaceTipViewShadow) {
                int shadowZorder = this->worldCoordinate2Zorder(centerPos, WorldRender::RenderLayer::Shadow);
                cocos2d::Vec2 shadowPoint = this->worldCoordinate2CocosPoint(centerPos, WorldRender::RenderLayer::Shadow);
                
                _hmmCardPlaceTipViewShadow->setLocalZOrder(shadowZorder);
                _hmmCardPlaceTipViewShadow->setPosition(shadowPoint);
            }
        } else if (_tipCardType->getCardClass() == kHMMCardClass_Spell) {
            int zorder = this->worldCoordinate2Zorder(pos, WorldRender::RenderLayer::Ground);
            cocos2d::Vec2 point = this->worldCoordinate2CocosPoint(pos, WorldRender::RenderLayer::Ground);
            _hmmCardPlaceTipView->setLocalZOrder(zorder);
            _hmmCardPlaceTipView->setPosition(point);
        }
    }
}

void WorldRender::hideHMMCardPlaceTips() {
    if (_hmmCardPlaceTipView) {
        _hmmCardPlaceTipView->removeFromParent();
        _hmmCardPlaceTipView = nullptr;
    }
    
    if (_hmmCardPlaceTipViewShadow) {
        _hmmCardPlaceTipViewShadow->removeFromParent();
        _hmmCardPlaceTipViewShadow = nullptr;
    }
    
    _tipCardType = nullptr;
}
    
cocos2d::Node* WorldRender::addEffect(const std::string &renderKey, bool loop,
    const Coordinate32 &pos) {
    cocos2d::Node* ret = nullptr;
    
    const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(renderKey);
    
    // create node
    do {
        // check data
        if (!data) break;
        
        ret = RenderHelper::buildEffectNode(data->getFgResource(), loop, nullptr);
    } while (0);
    
    // attach node
    if (ret) {
        int zorder = worldCoordinate2Zorder(pos, (RenderLayer)data->getSpellRenderLayer(), data->getSpellHeight());
        cocos2d::Vec2 position = worldCoordinate2CocosPoint(pos, (RenderLayer)data->getSpellRenderLayer(), data->getSpellHeight());
        ret->setPosition(position);
        _worldContainer->addChild(ret, zorder);
    }
    
    return ret;
}
    
cocos2d::Node* WorldRender::addEffect(const EffectData& data,
    bool loop,
    const Coordinate32& pos,
    RenderLayer renderLayer,
    int32_t height) {
    cocos2d::Node* ret = nullptr;
    
    ret = RenderHelper::buildEffectNode(data, loop, nullptr);
    
    if (ret) {
        int zorder = worldCoordinate2Zorder(pos, renderLayer, height);
        cocos2d::Vec2 position = worldCoordinate2CocosPoint(pos, renderLayer, height);
        ret->setPosition(position);
        _worldContainer->addChild(ret, zorder);

    }
    
    return ret;
}
    
// ===================== private ========================

// ===================== inner interface ========================
    
void WorldRender::createWorldObjectRender(const WorldObject *object) {
    assert(object);
    
    if (!object) return;
    
    auto iter = _objectRenders.find(object->getId());
    if (iter != _objectRenders.end()) return;
    
    AbstractWorldObjectRender* objectRender = nullptr;
    if (M_INSTANCE_OF(object, const Unit*)) {
        objectRender = new UnitRender();
    } else if (M_INSTANCE_OF(object, const Bullet*)) {
        objectRender = new BulletRender();
    }
    
    if (objectRender && objectRender->init(object, this)) {
        objectRender->attach2WorldRender(this);
        _objectRenders.insert(std::make_pair(objectRender->getObject()->getId(), objectRender));
    } else {
        M_SAFE_DELETE(objectRender);
    }
}
    
void WorldRender::deleteAllWorldObjectRender() {
    for (auto iter = _objectRenders.begin(); iter != _objectRenders.end(); ++iter) {
        iter->second->dettachFromWorldRender(this);
        _recycleBin.push_back(iter->second);
    }
    _objectRenders.clear();
}
    
bool WorldRender::initWorldEnvironment() {
    _mapId = _world->getMap()->getMapId();

    //-------- base --------//
    if (!_worldLayer) {
        _worldLayer = LayerColor::create();
        _worldLayer->retain();
    }
    
    //-------- background --------//
    std::vector<std::string> bgFiles;
    getWorldBgFile(_mapId, bgFiles);
    float offsetX = 0.f;
    for (int i = 0; i < bgFiles.size(); ++i) {
        auto bg = Sprite::create(bgFiles[i]);
        
        if (!bg) continue;
        
        bg->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        bg->setPosition(offsetX, 0.f);
        _worldLayer->addChild(bg, IN_LAYER_WORLD_BACKGROUND_ZORDER);
        offsetX += bg->getContentSize().width;
    }
    
    //-------- tiled map --------//
    std::string tmxFileName = getWorldTMXTiledFile(_mapId);
    if (!tmxFileName.empty()) _tiledMap = experimental::TMXTiledMap::create(tmxFileName);
    if (!_tiledMap) return false;
    _worldLayer->addChild(_tiledMap, IN_LAYER_WORLD_TILEMAP_ZORDER);
    _worldLayer->setContentSize(_tiledMap->getContentSize());
    
    //-------- foreground --------//
    _worldContainer = _tiledMap->getLayer(TILEDMAP_LAYER_FOREGROUND_NAME);
    if (_worldContainer) {
        experimental::TMXLayer* layer = dynamic_cast<experimental::TMXLayer*>(_worldContainer);
        if (layer) {
            //TODO: reorder tiles zorder
        }
    } else {
        _worldContainer = Node::create();
        _tiledMap->addChild(_worldContainer);
    }
    
    //-------- logic --------//
    auto logicLayer = _tiledMap->getLayer(TILEDMAP_LAYER_LOGIC_NAME);
    if (logicLayer) {
        logicLayer->setVisible(false);
        logicLayer->removeFromParent();
    }
    
    //-------- effect --------//
    const auto& particles = DataManager::getInstance()->getMapParticleConfigData(_mapId);
    for (int i = 0; i < particles.size(); ++i) {
        addMapParticle(particles.at(i));
    }
    
    //-------- data --------//
    _pixelPerMapsizeX = getTile2PixelScale().width / Map::TILE_2_ELEMENT_SCALE;
    _pixelPerMapsizeY = getTile2PixelScale().height / Map::TILE_2_ELEMENT_SCALE;
    _cameraAngelDegrees = CAMERA_ANGEL_DEGREES;
    _cameraAngelRadians = _cameraAngelDegrees * M_PI / 180;
    _worldHeight = _world->getMap()->getMapElementHeight();
    _worldWidth = _world->getMap()->getMapElementWidth();
    
    return true;
}
    
const cocos2d::Size& WorldRender::getWorldSizeInTile() {
    return _tiledMap->getMapSize();
}
    
const cocos2d::Size& WorldRender::getTile2PixelScale() {
    return _tiledMap->getTileSize();
}
    
void WorldRender::renderSpellPattern(const SpellPattern* sp,
    creatureid_t targetId, const Coordinate32& pos) {
    if (!sp) return;
    
    if (sp->getClass() == kSpellPatternClass_Target) {
        auto iter = _objectRenders.find(targetId);
        if (iter != _objectRenders.end()) {
            UnitRender* ur = dynamic_cast<UnitRender*>(iter->second);
            if (ur) ur->addEffect(sp->getRenderKey(), false);
        }
    } else if (sp->getClass() == kSpellPatternClass_All
        || sp->getClass() == kSpellPatternClass_TargetPositionCircle) {
        addEffect(sp->getRenderKey(), false, pos);
    }
}
    
void WorldRender::createHMMCardPlaceTipsView(const HMMCardType *cardType, const World* world, cocos2d::Node*& outputTipsView, cocos2d::Node*& outputShadowView, bool swaped) {
    if (!cardType) return;
    
    if (cardType->getCardClass() == kHMMCardClass_Hero
        || cardType->getCardClass() == kHMMCardClass_Summon
        || cardType->getCardClass() == kHMMCardClass_Tower) {
        const UnitType* ut = world->getTechTree()->findUnitTypeById(cardType->getUnitSetting().getUnitTypeId());
        if (ut) {
            UnitView* uv = UnitView::create(ut);
            uv->setBodyAnimationPose(UnitAnimationPose::Stand);
            uv->setDirection(Unit::Direction::kDirection_Up);
            uv->setIsAlly(true);
            uv->buildAnimation();
            uv->setOpacity(UNIT_PLACE_TIPS_VIEW_OPACITY);
            uv->runAnimation(true, nullptr);
            outputTipsView = uv->getBodyNode();
            outputShadowView = uv->getShadowNode();
        }
    } else if (cardType->getCardClass() == kHMMCardClass_Spell) {
        const CardConfigData* ccd = DataManager::getInstance()->getCardConfigData(cardType->getId());
        
        if (ccd) {
            outputTipsView = RenderHelper::buildEffectNode(ccd->getTips(), true, nullptr);
            outputShadowView = nullptr;
        }
    }
    
    // add label
    if (outputTipsView) {
        cocos2d::Node* wrapper = cocos2d::Node::create();
        wrapper->addChild(outputTipsView);
        cocos2d::Label* label = CocosUtils::createLabel(cardType->getName(), DEFAULT_FONT_SIZE);
        label->setPosition(0.f, 75.f);
        label->setScaleX(swaped ? -1.f : 1.f);
        wrapper->addChild(label);
        outputTipsView = wrapper;
    }
}
    
// ===================== resources ========================

void WorldRender::getWorldBgFile(int mapId, std::vector<std::string> &output) {
    static std::string background_file_prefix = "map/bg";
    static std::string background_file_midfix = "_";
    static std::string background_file_postfix = ".png";
    
    int fileIndex = 1;
    do {
        std::string fileName = background_file_prefix + UnderWorldCoreUtils::to_string(mapId)
        + background_file_midfix + UnderWorldCoreUtils::to_string(fileIndex) + background_file_postfix;
        if (!FileUtils::getInstance()->isFileExist(fileName)) break;
        
        output.push_back(fileName);
        
    } while (0);
}

std::string WorldRender::getWorldTMXTiledFile(int mapId) {
    static std::string tmx_file_prefix = "map/";
    static std::string tmx_file_postfix = ".tmx";
    
    std::string fileName = tmx_file_prefix + UnderWorldCoreUtils::to_string(mapId) + tmx_file_postfix;
    
    if (!FileUtils::getInstance()->isFileExist(fileName)) return "";
    
    return fileName;
}
    
// ===================== cocos ========================
    
void WorldRender::addMapParticle(const MapParticleConfigData* particleData) {
    const string& fileName = particleData->getName();
    Node* effect = nullptr;
    if (".csb" == FileUtils::getInstance()->getFileExtension(fileName)) {
        effect = CocosUtils::playAnimation(fileName, 0.f, true, 0, -1, nullptr);
    } else {
        auto particle = ParticleSystemQuad::create(particleData->getName());
        if(particle) _mapParticles.insert(particle);
        
        effect = particle;
    }
    
    if (!effect) return;
    
    int worldWidthInPixel = getWorldSizeInTile().width * getTile2PixelScale().width;
    int worldHightInPixel = getWorldSizeInTile().height * getTile2PixelScale().height;
    auto pos = Point( worldWidthInPixel/ 2, worldHightInPixel/ 2) + Point(particleData->getPosX(), particleData->getPosY());
    effect->setPosition(pos);
    effect->setScaleX(particleData->getScaleX());
    effect->setScaleY(particleData->getScaleY());
    //TODO: add effect 2 world container & consider zorder
    _tiledMap->addChild(effect);
}

// ===================== misc ========================
    
void WorldRender::handleEvents() {
    static const frame_t effectDelayThreshold = GameConstants::second2Frame(EFFECT_RENDER_DELAY_THRESHOLD_IN_SECOND);
    
    std::unordered_map<creatureid_t, int> usefulCreateLog;
    std::unordered_set<creatureid_t> eliminateObject;
    std::vector<int> usefulSpellPatternCastLog;
    
    frame_t cf = _world->getClock()->getFrameCount();
    for (int i = 0; i < _events.size(); ++i) {
        if (_events[i]._type == World::EventLogType::ObjectCreate) {
            if (eliminateObject.find(_events[i]._id) == eliminateObject.end()) {
                usefulCreateLog.insert(std::make_pair(_events[i]._id, i));
            }
        } else if (_events[i]._type == World::EventLogType::ObjectEliminate) {
            {
                auto iter = _objectRenders.find(_events[i]._id);
                if (iter != _objectRenders.end()) iter->second->markObjectReleased();
            }
            {
                eliminateObject.insert(_events[i]._id);
                auto iter = usefulCreateLog.find(_events[i]._id);
                if (iter != usefulCreateLog.end()) usefulCreateLog.erase(iter);
            }
        } else if (_events[i]._type == World::EventLogType::SpellPatternCast) {
            if (_events[i]._spellPattern
                && _events[i]._frame + effectDelayThreshold > cf) {
                SpellPatternClass spc = _events[i]._spellPattern->getClass();
                if (spc == kSpellPatternClass_Target && _events[i]._spellTargetId != World::INVALID_ID) {
                    usefulSpellPatternCastLog.push_back(i);
                } else if ((spc == kSpellPatternClass_All || spc == kSpellPatternClass_TargetPositionCircle) && !(_events[i]._castPos == Coordinate32::ZERO)) {
                    usefulSpellPatternCastLog.push_back(i);
                }
            }
        }
    }
    
    for (auto iter = usefulCreateLog.begin(); iter != usefulCreateLog.end(); ++iter){
        createWorldObjectRender(_events[iter->second]._instance);
    }
    
    for (int i = 0; i < usefulSpellPatternCastLog.size(); ++i) {
        renderSpellPattern(_events[i]._spellPattern,
            _events[i]._spellTargetId, _events[i]._castPos);
    }
    
    _events.clear();
}
    
void WorldRender::recycle() {
    for (int i = 0; i < _recycleBin.size(); ++i) {
        delete _recycleBin[i];
    }
    _recycleBin.clear();
}

}}