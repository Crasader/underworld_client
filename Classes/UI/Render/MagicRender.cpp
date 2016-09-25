//
//  MagicRender.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/9/25.
//
//

#include "MagicRender.h"
#include "Magic.h"
#include "MagicType.h"
#include "WorldRender.h"

namespace UnderWorld{ namespace Core{
    
MagicRender::MagicRender()
: _magic(nullptr)
, _magicType(nullptr)
, _effectNode(nullptr)
, _worldRender(nullptr) {
}
    
MagicRender::~MagicRender() {
    CC_SAFE_RELEASE_NULL(_effectNode);
}
    
bool MagicRender::init(const WorldObject* object, WorldRender* worldRender) {
    if (!AbstractWorldObjectRender::init(object, worldRender)) return false;
    
    // init refs
    _worldRender = worldRender;
    
    // init data
    _magic = dynamic_cast<const Magic*>(object);
    if (!_magic) return false;
    
    _magicType = _magic->getMagicType();
    if (_magicType) _magicRenderKey = _magicType->getRenderKey();
    if (!_magicType || _magicRenderKey.empty()) return false;
    
    // init cocos
    // clean up
    CC_SAFE_RELEASE_NULL(_effectNode);
    
    return true;
}
    
void MagicRender::render() {
    AbstractWorldObjectRender::render();
    
    if (AbstractWorldObjectRender::isObjectReleased() && _worldRender) {
        _worldRender->deleteWorldObjectRender(this);
        return;
    }
}
    
void MagicRender::attach2WorldRender(WorldRender *worldRender) {
    if (worldRender && worldRender->getWorldContainer()) {
        if (!_effectNode) {
            _effectNode = worldRender->addEffect(_magicRenderKey,
                true, AbstractWorldObjectRender::getPos());
            if (_effectNode) _effectNode->retain();
        }
    }
}
    
void MagicRender::dettachFromWorldRender(WorldRender *worldRender) {
    if (_effectNode) {
        _effectNode->removeFromParent();
        CC_SAFE_RELEASE_NULL(_effectNode);
    }
}

}}