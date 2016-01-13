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
    class Spell;
}}

class HeroSpellHandlerObserver
{
public:
    virtual ~HeroSpellHandlerObserver() {}
    virtual void onHeroSpellHandlerUpdatingCD(const UnderWorld::Core::Spell* spell) = 0;
};

class HeroSpellHandler
{
public:
    HeroSpellHandler(UnderWorld::Core::Spell* spell);
    virtual ~HeroSpellHandler();
    void registerObserver(HeroSpellHandlerObserver *observer);
    void update();
    
private:
    HeroSpellHandlerObserver *_observer;
    const UnderWorld::Core::Spell* _spell;
};

#endif /* HeroSpellHandler_h */
