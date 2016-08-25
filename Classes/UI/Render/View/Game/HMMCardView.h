//
//  HMMCardView.hpp
//  Underworld_Client
//
//  Created by wenchengye on 16/7/30.
//
//

#ifndef HMMCardView_h
#define HMMCardView_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "GameConstants.h"

class CardConfigData;
class BattleSmallResourceNode;

namespace UnderWorld{ namespace Core{
    
class HMMCardType;
    
class HMMCardView : public cocos2d::Node {
public:
    static const std::string DISPLAY_RESOURCE_KEY;
    
private:
    /** data */
    const HMMCardType* _cardType;
    const CardConfigData* _configData;
    
    /** cocos */
    cocos2d::Sprite *_iconSprite;
    cocos2d::Sprite *_qualitySprite;
    BattleSmallResourceNode* _resourceNode;
    cocos2d::Sprite *_shiningSprite;
    cocos2d::ProgressTimer* _coldDownProgress;
    std::string _currentIconFile;
    
public:
    /** init */
    static HMMCardView* create(const HMMCardType* cardType);
    virtual ~HMMCardView();
    virtual bool init(const HMMCardType* cardType);
    
    /** interface */
    void setCardUseable(bool useable);
    void setResourceEnough(microres_t amount);
    void setSelected(bool selected);
    void setColdDownProgress(int progress);
    
    /** getters */
    const HMMCardType* getCardType() const        {return _cardType;}
    
protected:
    HMMCardView();
    
    
    
};


}}

#endif /* HMMCardView_h */
