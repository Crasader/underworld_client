//
//  HeroSpellHandler.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroSpellHandler.h"
#include "Spell.h"

using namespace UnderWorld::Core;

HeroSpellHandler::HeroSpellHandler(Spell* spell)
:_observer(nullptr)
,_spell(spell)
{
    
}

HeroSpellHandler::~HeroSpellHandler()
{
    
}

void HeroSpellHandler::registerObserver(HeroSpellHandlerObserver *observer)
{
    _observer = observer;
}

void HeroSpellHandler::update()
{
    if (_spell && _observer) {
        _observer->onHeroSpellHandlerUpdatingCD(_spell);
    }
}
