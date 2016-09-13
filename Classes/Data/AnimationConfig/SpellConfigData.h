//
//  SpellConfigData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SpellConfigData_h
#define SpellConfigData_h

#include <iostream>
#include <vector>
#include <unordered_map>

#include "GameConstants.h"

namespace tinyxml2 { class XMLElement; }

class SpellConfigData
{
public:
    typedef enum {
        kNone = 0,
        kLeft = 1,
        kRight = 2,
    } Direction;
    
    typedef enum {
        kFoot = 0,
        kBody = 1,
        kHead = 2,
    } Position;
    
    typedef UnderWorld::Core::mapsize_t uw_mapsize_t;
    
public:
    explicit SpellConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~SpellConfigData();
    
    // key
    const std::string& getRenderKey() const        {return _renderKey;}
    
    // position
    Direction getSpellDirection() const            {return _spellDirection;}
    Position getSpellPosition() const              {return _spellPosition;}
    int getSpellRenderLayer() const                {return _spellRenderLayer;}
    uw_mapsize_t getSpellHeight() const            {return _spellHeight;}
    
    // resource
    bool isLoop() const                            {return _loop;}
    const std::string& getFgResource() const       {return _foregroundResource;}
    const std::string& getBgResource() const       {return _backgroundResource;}
    const std::string& effectSound() const         {return _effectSound;}
    float getScale() const                         {return _scale;}
    
private:
    std::string _renderKey;
    
    /** this two attribute is useful, when effect is on unit */
    Direction _spellDirection;
    Position _spellPosition;
    
    /** this two attribute is useful, when effect is in world space, not on unit */
    int _spellRenderLayer;
    UnderWorld::Core::mapsize_t _spellHeight;
    
    bool _loop;
    std::string _foregroundResource;
    std::string _backgroundResource;
    std::string _effectSound;
    float _scale;
};

#endif /* SpellConfigData_h */
