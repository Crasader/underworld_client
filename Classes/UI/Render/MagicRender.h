//
//  MagicRender.h
//  Underworld_Client
//
//  Created by wenchengye on 16/9/25.
//
//

#ifndef MagicRender_h
#define MagicRender_h

#include "WorldObjectRender.h"
#include "Magic.h"

namespace UnderWorld{ namespace Core{

class WorldRender;
    
class MagicRender : public AbstractWorldObjectRender {
private:
    /** data */
    const Magic* _magic;
    const MagicType* _magicType;
    std::string _magicRenderKey;
    
    /** cocos */
    cocos2d::Node* _effectNode;
    
    /** refs */
    WorldRender* _worldRender;
    
public:
    /** init */
    MagicRender();
    virtual ~MagicRender();
    virtual bool init(const WorldObject* object, WorldRender* worldRender) override;
    
    /** override AbstractWorldObjectRender */
    virtual void render() override;
    virtual void attach2WorldRender(WorldRender* worldRender) override;
    virtual void dettachFromWorldRender(WorldRender* worldRender) override;
    
};
    
}}


#endif /* MagicRender_h */
