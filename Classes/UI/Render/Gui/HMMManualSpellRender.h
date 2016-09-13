//
//  HMMManualSpellRender.h
//  Underworld_Client
//
//  Created by Mac on 16/9/13.
//
//

#ifndef HMMManualSpellRender_h
#define HMMManualSpellRender_h

#include "cocos2d.h"

namespace UnderWorld{ namespace Core{
    
class HMMManualSpellView;
class Spell;

class HMMManualSpellRender {
private:
    /** cocos */
    HMMManualSpellView* _spellView;
    
public:
    /** init */
    HMMManualSpellRender();
    virtual ~HMMManualSpellRender();
    virtual void init(const Spell* spell);
    
    /** update */
    void render(const Spell* spell);
    
    /** getters */
    HMMManualSpellView* getSpellView()         {return _spellView;}
};

}}
#endif /* HMMManualSpellRender_h */
