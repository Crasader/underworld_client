//
//  HMMCardRender.hpp
//  Underworld_Client
//
//  Created by wenchengye on 16/7/30.
//
//

#ifndef HMMCardRender_h
#define HMMCardRender_h

#include "cocos2d.h"

namespace UnderWorld{ namespace Core{
    
class HMMCardView;
class HMMCard;
class Game;
    
class HMMCardRender {
private:
    /** cocos */
    HMMCardView* _cardview;
    
public:
    /** init */
    HMMCardRender();
    virtual ~HMMCardRender();
    virtual void init(const HMMCard* card);
    
    /** update */
    void render(const HMMCard* card);
    
    /** getters */
    HMMCardView* getCardView()         {return _cardview;}
};

    
}}

#endif /* HMMCardRender_h */
