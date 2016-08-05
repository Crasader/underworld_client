//
//  WorldRender.cpp
//  Underworld_Client
//
//  Created by Mac on 16/6/23.
//
//

#include <cmath>

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
, _worldLayer(nullptr)
, _worldContainer(nullptr)
, _tiledMap(nullptr) {
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
    
// ===================== interface ========================
    
void WorldRender::render(const World *world) {
    //handle concerned event log
    handleEvents();
    
    //render all objects
    std::unordered_map<creatureid_t, AbstractWorldObjectRender*> clone = _objectRenders;
    for (auto iter = clone.begin(); iter != clone.end(); ++iter) {
        iter->second->render();
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
    return (_world->getMap()->getMapElementHeight() * (int)renderLayer) +
        (_world->getMap()->getMapElementHeight() - pos.y);
}
    
cocos2d::Node* WorldRender::addEffect(const std::string &renderKey, bool loop,
    const Coordinate32 &pos) {
    cocos2d::Node* ret = nullptr;
    
    const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(renderKey);
    
    // create node
    do {
        // check data
        if (!data) break;
        
        // check file
        const vector<string>& files = data->getReceiverResourceNames();
        if (files.size() <= 0 || files.at(0).empty()) break;
        
        const std::string& file = files.at(0);
        
        //check file name
        const size_t found = file.find_last_of(".");
        if (found == string::npos) break;
        
        const string& suffix = file.substr(found + 1);
        if ("csb" == suffix) {
            function<void(Node*)> callback = nullptr;
            if (!loop) {
                callback = [](Node* sender) { if (sender) sender->removeFromParent(); };
            }
            ret = CocosUtils::playAnimation(file, 0.f , loop, 0, -1, callback);
        } else if ("plist" == suffix) {
            ParticleSystemQuad *particle = ParticleSystemQuad::create(file);
            if (particle) {
                if (!loop) particle->setAutoRemoveOnFinish(true);
                ret = particle;
            }
        }
        
    } while (0);
    
    // attach node
    if (ret) {
        //TODO: postiion and order
        int zorder = worldCoordinate2Zorder(pos, RenderLayer::Land);
        cocos2d::Vec2 position = worldCoordinate2CocosPoint(pos, RenderLayer::Land);
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