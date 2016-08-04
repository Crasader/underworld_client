//
//  WorldObjectRender.cpp
//  Underworld_Client
//
//  Created by Mac on 16/6/24.
//
//

#include "WorldObject.h"
#include "WorldObjectRender.h"
#include "World.h"


namespace UnderWorld{ namespace Core{
    
// ===============================
// 	class AbstractWorldObjectRender
// ===============================

// ===================== public ========================
    
bool AbstractWorldObjectRender::init(const WorldObject* object, WorldRender* worldRender) {
    assert(object && worldRender);
    
    if (!object || !worldRender) return false;
    
    _object = object;
    _pos = _object ? _object->getPos() : Coordinate32::ZERO;
    _objectId = _object ? object->getId() : World::INVALID_ID;
    _objectReleased = false;
    _worldRender = worldRender;
    
    return true;
}
    
void AbstractWorldObjectRender::render() {
    if (isObjectReleased()) setObject(nullptr);
    
    if (_object) _pos = _object->getPos();
}
    
// ===================== protected ========================
    
AbstractWorldObjectRender::AbstractWorldObjectRender()
: _object(nullptr)
, _pos(Coordinate32::ZERO)
, _objectReleased(false)
, _objectId(World::INVALID_ID) {
}
    
}}

