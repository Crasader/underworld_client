//
//  BulletRender.cpp
//  Underworld_Client
//
//  Created by Mac on 16/7/18.
//
//

#include <cmath>

#include "BulletRender.h"
#include "DataManager.h"
#include "WorldRender.h"
#include "CocosUtils.h"

namespace UnderWorld{ namespace Core{

BulletRender::BulletRender()
: _bullet(nullptr)
, _bulletType(nullptr)
, _configData(nullptr)
, _renderLayer(WorldRender::RenderLayer::Land)
, _pathType(BulletPathType::Parabola)
, _startHeight(0)
, _targetHeight(0)
, _parabolaA(0.f)
, _parabolaB(0.f)
, _parabolaH(0.f)
, _alpha(0.f)
, _currentHeight(0.f)
, _scale(0.f)
, _rotation(0.f)
, _bulletExploded(false)
, _renderingExplode(false)
, _initing(false)
, _explodeLog(nullptr)
, _mainNode(nullptr)
, _shadowNode(nullptr)
, _bodyNode(nullptr){
}
    
BulletRender::~BulletRender() {
    CC_SAFE_RELEASE(_mainNode);
    CC_SAFE_RELEASE(_shadowNode);
}
    
bool BulletRender::init(const WorldObject *object, WorldRender *worldRender) {
    if (!AbstractWorldObjectRender::init(object, worldRender)) return false;
    
    // init data
    _bullet = dynamic_cast<const Bullet*>(object);
    if (!_bullet) return false;
    
    _bulletType = _bullet->getBulletType();
    _bulletRenderKey = _bulletType ? _bulletType->getRenderKey() : "";
    _configData = DataManager::getInstance()->getBRConfigData(_bulletRenderKey);
    if (!_configData || _bulletType) return false;
    
    if (_bullet->getBornField() == kFieldType_Air
        && _bullet->getTargetField() == kFieldType_Air) {
        _renderLayer = WorldRender::RenderLayer::Air;
    } else {
        _renderLayer = WorldRender::RenderLayer::Land;
    }
    
    if (_bullet->getBornField() != _bullet->getTargetField()) {
        _pathType = BulletPathType::Line;
    } else {
        _pathType = BulletPathType::Parabola;
    }
    
    _startHeight = (_pathType == BulletPathType::Line && _bullet->getBornField() == kFieldType_Air) ? WorldRender::RENDER_LAYER_AIR_HEIGHT_IN_MAP_SIZE : _bullet->getBornHeight();
    _targetHeight = (_pathType == BulletPathType::Line && _bullet->getTargetField() == kFieldType_Air) ? WorldRender::RENDER_LAYER_AIR_HEIGHT_IN_MAP_SIZE : _bullet->getTargetHeight();
    _startPos = _bullet->getBornPos();
    
    // init status
    _bulletExploded = false;
    _renderingExplode = false;

    // init events
    _eventLogs.clear();
    _explodeLog = nullptr;
    
    // init cocos
    if (!_mainNode) {
        _mainNode = cocos2d::Node::create();
        _mainNode->retain();
    }
    _mainNode->removeAllChildren();
    if (!_shadowNode) {
        _shadowNode = cocos2d::Node::create();
        _shadowNode->retain();
    }
    _shadowNode->removeAllChildren();
    _bodyNode = nullptr;
    
    // init render
    _initing = true;
    render();
    
    return true;
}
    
void BulletRender::render() {
    AbstractWorldObjectRender::render();
    
    if (_renderingExplode) return;
    
    // handle events
    _explodeLog = nullptr;
    for (int i = 0; i < _eventLogs.size(); ++i) {
        if (_eventLogs[i]._type == Bullet::EventLogType::BulletExplode) {
            _explodeLog = &_eventLogs[i];
        }
    }
    
    const Coordinate32* currentPos = nullptr;
    const Coordinate32* targetPos = nullptr;
    
    if (isObjectReleased() || _explodeLog) {
        currentPos = _explodeLog ? &_explodeLog->_explodePos : &getPos();
        targetPos = isObjectReleased() ? &_targetPos : &_bullet->getTargetPos();
    } else {
        currentPos = &getPos();
        targetPos = &_bullet->getTargetPos();
    }
    
    updateParams(*currentPos, *targetPos, _initing);
    _bulletExploded = isObjectReleased() || _bullet->isExploded();
    
    renderPosition(*currentPos);
    if (_bulletExploded) {
        _renderingExplode = true;
        renderExplodeAnimation();
    } else {
        renderFlyingAnimation();
    }
    
    _eventLogs.clear();
    
    if (_initing) _initing = false;
}
    
void BulletRender::attach2WorldRender(WorldRender* worldRender) {
    if (worldRender->getWorldContainer()) {
        if (_mainNode) worldRender->getWorldContainer()->addChild(_mainNode);
        if (_shadowNode) worldRender->getWorldContainer()->addChild(_shadowNode);
    }
}
    
void BulletRender::dettachFromWorldRender(WorldRender* worldRender) {
    if (worldRender->getWorldContainer()) {
        if (_mainNode) worldRender->getWorldContainer()->removeChild(_mainNode);
        if (_shadowNode) worldRender->getWorldContainer()->removeChild(_shadowNode);
    }
}
    
void BulletRender::onNotifyBulletEvents(const std::vector<Bullet::EventLog>& events) {
    for (int i = 0; i < events.size(); ++i) {
        _eventLogs.push_back(events[i]);
    }
}
    
cocos2d::Node* BulletRender::addEffect(const std::string&file, bool loop, const std::function<void ()>& callback) {
    cocos2d::Node* ret = nullptr;
    
    // create node
    do {
        // check file
        if (file.empty()) break;
        
        //check file name
        const size_t found = file.find_last_of(".");
        if (found == string::npos) break;
        
        const string& suffix = file.substr(found + 1);
        if ("csb" == suffix) {
            function<void(cocos2d::Node*)> func = nullptr;
            if (!loop) {
                func = [callback](cocos2d::Node* sender) {
                    if (sender) {
                        sender->removeFromParent();
                    }
                    //TODO: check if this bug, sender->removeFromParent() cause sender release
                    // check whether func is release with sender
                    callback();
                };
            }
            ret = CocosUtils::playCSBAnimation(file, loop, 0, func);
        } else if ("plist" == suffix) {
            cocos2d::ParticleSystemQuad *particle = cocos2d::ParticleSystemQuad::create(file);
            if (particle) {
                if (!loop) particle->setAutoRemoveOnFinish(true);
                ret = particle;
            }
            
            //TODO: figure out and exec callback
        }
        
    } while (0);
    
    // attach node
    if (ret) {
        //TODO: position
        _mainNode->addChild(ret);
    }
    
    return ret;
}

    
void BulletRender::updateParams(const Coordinate32& currentPos,
    const Coordinate32& targetPos, bool init) {
    if (init || !(targetPos == _targetPos)) {
        //TODO: update parabola
        const float d = sqrt(pow(abs(_startPos.x - _targetPos.x), 2) + pow(abs(_startPos.y - _targetPos.y), 2));
        const float bulletMaxHeightFactor = _bulletType->getHeight();
        
        _parabolaH = _startHeight;
        _parabolaA = - (2.f * d * bulletMaxHeightFactor + _parabolaH + 2.f * sqrt(pow(d * bulletMaxHeightFactor, 2) + d * _parabolaH * bulletMaxHeightFactor)) / pow(d, 2);
        _parabolaB = 2.0f * (d * bulletMaxHeightFactor + sqrt(pow(d * bulletMaxHeightFactor, 2) + d * _parabolaH * bulletMaxHeightFactor)) / d;
    }
    
    const float distance = sqrt(pow(abs(currentPos.x - _startPos.x), 2) + pow(abs(currentPos.y - _startPos.y), 2));
    
    _currentHeight = _parabolaA * pow(distance, 2) + _parabolaB * distance + _parabolaH;
    
    const float beta = atan(2.f * _parabolaA * distance + _parabolaB);
    if (_targetPos.x == _startPos.x) {
        _rotation = _targetPos.y > _startPos.y ? -90.f : 90.f;
        _scale = abs(cos(beta) + sin(beta) / _worldRender->getCameraAngelRadians());
    } else {
        const float gamma = atan(tan(beta)/cos(_alpha)/_worldRender->getCameraAngelRadians() + tan(_alpha));
        int direction = _targetPos.x < _startPos.x ? 1 : -1;
        _rotation = gamma * 180 / M_PI * direction;
        _scale = abs(cos(_alpha) * cos(beta) / cos(gamma)) * direction;
    }
}
    
void BulletRender::renderPosition(const Coordinate32& currentPos) {
    
    cocos2d::Vec2 shadowPoint =
        _worldRender->worldCoordinate2CocosPoint(currentPos,
            WorldRender::RenderLayer::Shadow);
    int shadowZorder = _worldRender->worldCoordinate2Zorder(currentPos,
        WorldRender::RenderLayer::Shadow);
    _shadowNode->setPosition(shadowPoint);
    _shadowNode->setLocalZOrder(shadowZorder);
    
    cocos2d::Vec2 mainPoint = _worldRender->worldCoordinate2CocosPoint(currentPos,
        _renderLayer, _currentHeight);
    int mainZorder = _worldRender->worldCoordinate2Zorder(currentPos, _renderLayer);
    _mainNode->setPosition(mainPoint);
    _mainNode->setLocalZOrder(mainZorder);
}
    
void BulletRender::renderFlyingAnimation() {
    // create bodyNode
    if (!_bodyNode) {
        const std::string& file = _configData->getResource();
        _bodyNode = addEffect(file, true);
    }
    
    // set params
    if (_bodyNode) {
        _bodyNode->setRotation(_rotation);
        _bodyNode->setScaleX(_scale);
    }
    
}
    
void BulletRender::renderExplodeAnimation() {
    if (_bodyNode) {
        _bodyNode->removeFromParent();
        _bodyNode = nullptr;
    }
    
    std::string file;  //TODO: explode file
    auto ret = addEffect(file, false, std::bind(&BulletRender::explodeCallback, this));
    
    if (!ret) explodeCallback();
}
    
void BulletRender::explodeCallback() {
    _worldRender->deleteWorldObjectRender(this);
}

}}