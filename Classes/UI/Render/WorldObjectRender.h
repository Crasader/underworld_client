//
//  WorldObjectRender.h
//  Underworld_Client
//
//  Created by Mac on 16/6/24.
//
//

#ifndef WorldObjectRender_h
#define WorldObjectRender_h

#include "cocos2d.h"

#include "Coordinate.h"
#include "GameConstants.h"

namespace UnderWorld{ namespace Core{
    
class WorldObject;
class WorldRender;
    
// ===============================
// 	class AbstractWorldObjectRender
//
//	World Object Render, superclass of all object Renders
// ===============================

class AbstractWorldObjectRender {
private:
    const WorldObject* _object;
    Coordinate32 _pos;
    bool _objectReleased;
    creatureid_t _objectId;
    
protected:
    WorldRender* _worldRender;
    
public:
    /** init */
    virtual ~AbstractWorldObjectRender() {};
    virtual bool init(const WorldObject* object, WorldRender* worldRender);
    
    /** getter */
    const Coordinate32& getPos() const           {return _pos;}
    bool isObjectReleased() const                {return _objectReleased;}
    const WorldObject* getObject() const         {return _object;}
    creatureid_t getObjectId() const             {return _objectId;}
    
    /** setter */
    void markObjectReleased()                    {_objectReleased = true;}
    
    /** interface */
    virtual void render();
    virtual void attach2WorldRender(WorldRender* worldRender) = 0;
    virtual void dettachFromWorldRender(WorldRender* worldRender) = 0;
    
    
protected:
    AbstractWorldObjectRender();
    
    /** getter */
    WorldRender* getWorldRender()                {return _worldRender;}
    
    /** setter */
    void setPos(Coordinate32& pos)               {_pos = pos;}
    void setObject(const WorldObject* object)    {_object = object;}

};
    
}}

#endif /* WorldObjectRender_h */
