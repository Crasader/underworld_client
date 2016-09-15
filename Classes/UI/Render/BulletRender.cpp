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
#include "RenderHelper.h"

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
, _body(nullptr)
, _shadow(nullptr) {
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
    if (!_configData || !_bulletType) return false;
    
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
    _targetPos = _bullet->getTargetPos();
    
    // init params
    _currentHeight = _startHeight;
    _scale = 1.f;
    _rotation = 0.f;
    
    // init flag
    _bulletExploded = false;
    _renderingExplode = false;
    _initing = true;

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
    _body = nullptr;
    _shadow = nullptr;
    
    // init render
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
        if (!_renderingExplode) {
            _renderingExplode = true;
            renderExplodeAnimation();
        }
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
    
cocos2d::Node* BulletRender::addEffect(const EffectData& effectData, bool loop, bool toBody, const std::function<void ()>& callback) {
    cocos2d::Node* ret = RenderHelper::buildEffectNode(effectData, loop, callback);
    
    // attach node
    if (ret) {
        if (toBody) _mainNode->addChild(ret);
        else _shadowNode->addChild(ret);
    }
    
    return ret;
}

    
void BulletRender::updateParams(const Coordinate32& currentPos,
    const Coordinate32& targetPos, bool init) {
    
    if (init || !(targetPos == _targetPos)) {
        _targetPos = targetPos;
        
        const float d = sqrt(pow(abs(_startPos.x - _targetPos.x), 2) + pow(abs(_startPos.y - _targetPos.y), 2));
        const float bulletMaxHeightFactor = _bulletType->getHeight() / 100.f;
        const float bulletMaxHeight = d * bulletMaxHeightFactor;
        _alpha = atan(float(_targetPos.y - _startPos.y) / (_targetPos.x - _startPos.x));
        
        if (d == 0.f) return;
        
        _parabolaH = _startHeight;
        _parabolaA = - (2.f * bulletMaxHeight + _parabolaH + 2.0f * sqrt(pow(bulletMaxHeight, 2) + _parabolaH * bulletMaxHeight)) / pow(d, 2);
        _parabolaB = 2.0f * (bulletMaxHeight + sqrt(pow(bulletMaxHeight, 2) + _parabolaH * bulletMaxHeight)) / d;
    }
    
    const float distance = sqrt(pow(abs(currentPos.x - _startPos.x), 2) + pow(abs(currentPos.y - _startPos.y), 2));

    _currentHeight = _parabolaA * pow(distance, 2) + _parabolaB * distance + _parabolaH;
    
    const float beta = atan(2.f * _parabolaA * distance + _parabolaB);
    if (_targetPos.x == _startPos.x) {
        _rotation = _targetPos.y > _startPos.y ? -90.f : 90.f;
        _scale = abs(cos(beta) + sin(beta) / _worldRender->getCameraAngelRadians());
    } else {
        int direction1 = _targetPos.x < _startPos.x ? 1 : -1;
        const float gamma = atan(tan(direction1 == 1 ? -beta : beta)/cos(_alpha)/_worldRender->getCameraAngelRadians() + tan(_alpha));
        _rotation = gamma * 180 / M_PI * -1;
        _scale = abs(cos(_alpha) * cos(beta) / cos(gamma)) * direction1 * -1;
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
    if (!_body) {
        _body = addEffect(_configData->getResource(), true, true);
    }
    
    // set params
    if (_body) {
        _body->setRotation(_rotation);
        _body->setScaleX(_scale * _configData->getResource().getScale());
        _body->setScaleY(_configData->getResource().getScale());
    }
    
    // create shadow
    if (!_shadow) {
        _shadow = addEffect(_configData->getShadowResource(), true, false);
    }
    
    if (_shadow) {
        _shadow->setRotation(_rotation);
        _shadow->setScaleX(_scale * _configData->getShadowResource().getScale());
        _shadow->setScaleY(_configData->getShadowResource().getScale());
    }
    
}
    
void BulletRender::renderExplodeAnimation() {
    if (_body) {
        _body->removeFromParent();
        _body = nullptr;
    }
    
    if (_shadow) {
        _shadow->removeFromParent();
        _shadow = nullptr;
    }
    
    auto ret = addEffect(_configData->getExplodeResource(), false, true, std::bind(&BulletRender::explodeCallback, this));
    
    if (!ret) explodeCallback();
}
    
void BulletRender::explodeCallback() {
    _worldRender->deleteWorldObjectRender(this);
}

}}