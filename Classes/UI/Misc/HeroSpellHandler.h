//
//  HeroSpellHandler.h
//  Underworld_Client
//
//  Created by Andy on 16/1/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroSpellHandler_h
#define HeroSpellHandler_h

namespace UnderWorld { namespace Core {
    class Unit;
}}

class HeroSpellHandler
{
public:
    HeroSpellHandler(UnderWorld::Core::Unit* unit);
    virtual ~HeroSpellHandler();
    
private:
    
};

#endif /* HeroSpellHandler_h */
