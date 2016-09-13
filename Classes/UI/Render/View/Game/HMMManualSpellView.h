//
//  HMMManualSpellView.h
//  Underworld_Client
//
//  Created by Mac on 16/9/13.
//
//

#ifndef HMMManualSpellView_h
#define HMMManualSpellView_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "GameConstants.h"

namespace UnderWorld{ namespace Core{
    
class Spell;

class HMMManualSpellView : public cocos2d::Node {
private:
    /** data */
    const Spell* _spell;
    
    /** cocos */
    cocos2d::Sprite* _iconSprite;
    cocos2d::ProgressTimer* _coldDownProgress;
    std::string _currentIconFile;
    
public:
    /** init */
    static HMMManualSpellView* create(const Spell* spell);
    virtual ~HMMManualSpellView();
    virtual bool init(const Spell* spell);
    
    /** interface */
    void setSpellUseable(bool useable);
    void setColdDownProgress(int progress);
    
    /** getters */
    const Spell* getSpell() const        {return _spell;}
    
protected:
    HMMManualSpellView();
    
};
    
}}

#endif /* HMMManualSpellView_h */
